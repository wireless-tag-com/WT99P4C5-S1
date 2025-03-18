/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdatomic.h>
#include "esp_log.h"
#include "driver/bitscrambler.h"
#include "bitscrambler_private.h"
#include "hal/bitscrambler_ll.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "bitscrambler";

#define BITSCRAMBLER_BINARY_VER 1 //max version we're compatible with
#define BITSCRAMBLER_HW_REV 0

// After a reset, it can take a few cycles for the BitScrambler to actually be
// reset. We check this many times for this; if it takes longer the hardware
// is broken or something.
#define BITSCRAMBLER_RESET_ITERATIONS 10000

/*
Format of a V1 BitScrambler program image:
- Header, as defined by bitscrambler_program_hdr_t below. Size is hdr->hdr_len words.
- Program lines. A line is 9 32-bit words, we have hdr->inst_ct lines.
- LUT data. LUT is hdr->lut_word_ct 32-bit words in size.
*/
typedef struct {
    uint8_t version;
    uint8_t hw_rev;
    uint8_t hdr_len;        //in 32-bit words
    uint8_t inst_ct;        //0-8
    uint16_t lut_word_ct;   //in 32-bit words
    uint8_t lut_width;      //0, 1, 2
    uint8_t prefetch;       //prefetch enabled?
    uint16_t trailing_bits; //in bits
    uint8_t eof_on;
    uint8_t unused;
} bitscrambler_program_hdr_t;

#define INST_LEN_WORDS BITSCRAMBLER_LL_INST_LEN_WORDS

// For now, hardware only has one TX and on RX unit. Need to make this more flexible if we get
// non-specific and/or more channels.
atomic_flag tx_in_use = ATOMIC_FLAG_INIT;
atomic_flag rx_in_use = ATOMIC_FLAG_INIT;

// Claim both TX and RX channels for loopback use
// Returns true on success, false if any of the two directions already is claimed.
static bool claim_channel_loopback(void)
{
    bool old_val_tx = atomic_flag_test_and_set(&tx_in_use);
    if (old_val_tx) {
        return false;
    }
    bool old_val_rx = atomic_flag_test_and_set(&rx_in_use);
    if (old_val_rx) {
        atomic_flag_clear(&tx_in_use);
        return false;
    }
    return true;
}

// Claim a channel using the direction it indicated.
// Returns true on success, false if the direction already is claimed
static bool claim_channel(bitscrambler_direction_t dir)
{
    if (dir == BITSCRAMBLER_DIR_TX) {
        bool old_val = atomic_flag_test_and_set(&tx_in_use);
        if (old_val) {
            return false;
        }
    } else if (dir == BITSCRAMBLER_DIR_RX) {
        bool old_val = atomic_flag_test_and_set(&rx_in_use);
        if (old_val) {
            return false;
        }
    }
    return true;
}

//Initialize the BitScrambler object and hardware using the given config.
static esp_err_t init_from_config(bitscrambler_t *bs, const bitscrambler_config_t *config)
{
    bs->cfg = *config;                  //Copy config over
    bs->hw = BITSCRAMBLER_LL_GET_HW(0); //there's only one as of now; if there's more, we need to handle them as a pool.

    //Attach to indicated peripheral.
    bitscrambler_ll_select_peripheral(bs->hw, bs->cfg.dir, config->attach_to);
    bitscrambler_ll_enable(bs->hw, bs->cfg.dir);

    return ESP_OK;
}

static void enable_clocks(bitscrambler_t *bs)
{
    PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_BITSCRAMBLER_MODULE, ref_count) {
        if (ref_count == 0) { //we're the first to enable the BitScrambler module
            bitscrambler_ll_set_bus_clock_sys_enable(1);
            bitscrambler_ll_reset_sys();
            bitscrambler_ll_mem_power_by_pmu();
        }
        if (bs->cfg.dir == BITSCRAMBLER_DIR_RX || bs->loopback) {
            bitscrambler_ll_set_bus_clock_rx_enable(1);
            bitscrambler_ll_reset_rx();
        }
        if (bs->cfg.dir == BITSCRAMBLER_DIR_TX || bs->loopback) {
            bitscrambler_ll_set_bus_clock_tx_enable(1);
            bitscrambler_ll_reset_tx();
        }
    }
}

static void disable_clocks(bitscrambler_t *bs)
{
    PERIPH_RCC_RELEASE_ATOMIC(PERIPH_BITSCRAMBLER_MODULE, ref_count) {
        if (bs->cfg.dir == BITSCRAMBLER_DIR_RX || bs->loopback) {
            bitscrambler_ll_set_bus_clock_rx_enable(0);
        }
        if (bs->cfg.dir == BITSCRAMBLER_DIR_TX || bs->loopback) {
            bitscrambler_ll_set_bus_clock_tx_enable(0);
        }
        if (ref_count == 0) { //we're the last to disable the BitScrambler module
            bitscrambler_ll_set_bus_clock_sys_enable(0);
            bitscrambler_ll_mem_force_power_off();
        }
    }
}

//Private function: init an existing BitScrambler object as a loopback BitScrambler.
esp_err_t bitscrambler_init_loopback(bitscrambler_handle_t handle, const bitscrambler_config_t *config)
{
    if (!claim_channel_loopback()) {
        return ESP_ERR_NOT_FOUND;
    }

    assert(config->dir == BITSCRAMBLER_DIR_TX);
    handle->loopback = true;
    enable_clocks(handle);
    esp_err_t r = init_from_config(handle, config);
    //Loopback mode also needs RX channel set to the selected peripheral, even if it's not used.
    bitscrambler_ll_select_peripheral(handle->hw, BITSCRAMBLER_DIR_RX, config->attach_to);
    return r;
}

esp_err_t bitscrambler_new(const bitscrambler_config_t *config, bitscrambler_handle_t *handle)
{
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    // Allocate memory for private data
    bitscrambler_t *bs = calloc(1, sizeof(bitscrambler_t));
    if (!bs) {
        return ESP_ERR_NO_MEM;
    }

    // Claim channel
    if (!claim_channel(config->dir)) {
        free(bs);
        return ESP_ERR_NOT_FOUND;
    }

    enable_clocks(bs);
    // Do initialization of BS object.
    esp_err_t r = init_from_config(bs, config);
    if (r != ESP_OK) {
        bitscrambler_free(bs);
        return r;
    }

    // Done.
    *handle = bs;
    return ESP_OK;
}

esp_err_t bitscrambler_load_program(bitscrambler_handle_t bs, const void *program_bin)
{
    if (!bs || !program_bin) {
        return ESP_ERR_INVALID_ARG;
    }
    bitscrambler_program_hdr_t hdr;

    //Parse the program header. There are two versions, V0 is generated by the C assembler while
    //v1 is generated by the Python assembler.
    int inst_len_bytes = INST_LEN_WORDS * sizeof(uint32_t); //note this is different for v1 and v0
    memcpy(&hdr, program_bin, sizeof(bitscrambler_program_hdr_t));
    if (hdr.version != BITSCRAMBLER_BINARY_VER) {
        ESP_LOGE(TAG, "Bitscrambler binary version %d not supported!", hdr.version);
        return ESP_ERR_INVALID_ARG;
    }
    if (hdr.hw_rev != BITSCRAMBLER_HW_REV) {
        ESP_LOGE(TAG, "Bitscrambler hardware rev %d not supported!", hdr.hw_rev);
        return ESP_ERR_INVALID_ARG;
    }

    bitscrambler_ll_set_state(bs->hw, bs->cfg.dir, BITSCRAMBLER_SET_STATE_HALT);

    //Load the program
    const uint8_t *p = (const uint8_t*)program_bin;
    p += hdr.hdr_len * sizeof(uint32_t); //skip header
    uint32_t instr[INST_LEN_WORDS];
    for (int inst = 0; inst < hdr.inst_ct; inst++) {
        //v0 doesn't have the words 32-bit aligned, so memcpy to work around that
        memcpy(instr, p, INST_LEN_WORDS * sizeof(uint32_t));
        p += inst_len_bytes;
        for (int w = 0; w < INST_LEN_WORDS; w++) {
            bitscrambler_ll_instmem_write(bs->hw, bs->cfg.dir, inst, w, instr[w]);
        }
    }

    ESP_LOGD(TAG, "Loaded %d instructions", hdr.inst_ct);
    //Load the LUT.
    bitscrambler_ll_set_lut_width(bs->hw, bs->cfg.dir, BITSCRAMBLER_LUT_WIDTH_32BIT);
    uint32_t *lut = (uint32_t*)p;
    for (int w = 0; w < hdr.lut_word_ct; w++) {
        bitscrambler_ll_lutmem_write(bs->hw, bs->cfg.dir, w, lut[w]);
    }

    //Set options from header
    bitscrambler_ll_set_lut_width(bs->hw, bs->cfg.dir, hdr.lut_width);
    bitscrambler_ll_enable_prefetch_on_reset(bs->hw, bs->cfg.dir, hdr.prefetch);
    // the bitscrambler assembler (bsasm.py) treats 'eof_on=1' as "upstream", a.k.a, read from upstream
    // quote from bsasm.py: {'op': 'eof_on', 'default': 1, 'enum': {'upstream': 1, 'downstream': 0}},
    bitscrambler_eof_mode_t eof_mode = hdr.eof_on ? BITSCRAMBLER_EOF_MODE_READ : BITSCRAMBLER_EOF_MODE_WRITE;
    bitscrambler_ll_set_eof_mode(bs->hw, bs->cfg.dir, eof_mode);
    bitscrambler_ll_set_tailing_bits(bs->hw, bs->cfg.dir, hdr.trailing_bits);
    //fixed options
    bitscrambler_ll_set_dummy_mode(bs->hw, bs->cfg.dir, BITSCRAMBLER_DUMMY_MODE_DUMMY);
    bitscrambler_ll_set_halt_mode(bs->hw, bs->cfg.dir, BITSCRAMBLER_HALT_IGNORE_WRITES);
    //enable loopback mode if requested
    bitscrambler_ll_enable_loopback(bs->hw, bs->loopback);

    return ESP_OK;
}

esp_err_t bitscrambler_load_lut(bitscrambler_handle_t handle, void *lut, size_t size_bytes)
{
    if (!handle || !lut) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t *lut_words = (uint32_t*)lut;
    bitscrambler_lut_width_t lut_width = bitscrambler_ll_get_lut_width(handle->hw, handle->cfg.dir);
    bitscrambler_ll_set_lut_width(handle->hw, handle->cfg.dir, BITSCRAMBLER_LUT_WIDTH_32BIT);
    size_t size_words = (size_bytes + 3) / 4;
    for (int w = 0; w < size_words; w++) {
        bitscrambler_ll_lutmem_write(handle->hw, handle->cfg.dir, w, lut_words[w]);
    }
    bitscrambler_ll_set_lut_width(handle->hw, handle->cfg.dir, lut_width);
    return ESP_OK;
}

esp_err_t bitscrambler_register_extra_clean_up(bitscrambler_handle_t handle, bitscrambler_extra_clean_up_func_t clean_up, void* user_ctx)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    handle->extra_clean_up = clean_up;
    handle->clean_up_user_ctx = user_ctx;
    return ESP_OK;
}

void bitscrambler_free(bitscrambler_handle_t handle)
{
    if (!handle) {
        return;
    }
    disable_clocks(handle);
    if (handle->loopback) {
        atomic_flag_clear(&tx_in_use);
        atomic_flag_clear(&rx_in_use);
    } else if (handle->cfg.dir == BITSCRAMBLER_DIR_TX) {
        atomic_flag_clear(&tx_in_use);
    } else if (handle->cfg.dir == BITSCRAMBLER_DIR_RX) {
        atomic_flag_clear(&rx_in_use);
    }
    if (handle->extra_clean_up) {
        handle->extra_clean_up(handle, handle->clean_up_user_ctx);
    }
    free(handle);
}

esp_err_t bitscrambler_start(bitscrambler_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    bitscrambler_ll_set_state(handle->hw, handle->cfg.dir, BITSCRAMBLER_SET_STATE_RUN);
    return ESP_OK;
}

esp_err_t bitscrambler_reset(bitscrambler_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    bitscrambler_ll_set_state(handle->hw, handle->cfg.dir, BITSCRAMBLER_SET_STATE_HALT);
    //If the halt bit is set, the Bitscrambler should (eventually) go to idle state. If it
    //does not, something got stuck.
    int timeout = BITSCRAMBLER_RESET_ITERATIONS;
    while ((bitscrambler_ll_get_current_state(handle->hw, handle->cfg.dir) != BITSCRAMBLER_STATE_IDLE) && timeout != 0) {
        timeout--;
    }
    if (timeout == 0) {
        ESP_LOGE(TAG, "bitscrambler_reset: Timeout waiting for idle!");
        ret = ESP_ERR_TIMEOUT;
    }
    //Reset the fifos & eof trace ctrs
    bitscrambler_ll_reset_fifo(handle->hw, handle->cfg.dir);
    bitscrambler_ll_clear_eof_trace(handle->hw, handle->cfg.dir);

    return ret;
}
