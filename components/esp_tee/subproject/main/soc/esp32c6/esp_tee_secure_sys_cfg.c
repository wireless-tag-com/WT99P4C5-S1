/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include <stdint.h>

#include "riscv/rv_utils.h"
#include "riscv/encoding.h"

#include "esp_cpu.h"
#include "esp_log.h"
#include "hal/apm_hal.h"
#include "hal/clk_gate_ll.h"

#include "esp_tee.h"
#include "esp_tee_intr.h"

#define _m2u_switch(arg0, arg1)           \
  ({                                                           \
    register uintptr_t ra asm("ra") = (uintptr_t)(arg0);       \
    register uintptr_t a1 asm("a1") = (uintptr_t)(arg1);    \
    asm volatile("ecall" : :"r"(ra), "r"(a1) : );                       \
  })

#define SET_BIT(t, n)  (t |= (1UL << (n)))
#define CLR_BIT(t, n)  (t &= ~(1UL << (n)))

static const char *TAG = "esp_tee_secure_sys_cfg";

extern uint32_t _vector_table;

void esp_tee_soc_secure_sys_init(void)
{
    ESP_LOGI(TAG, "Current privilege level - %d", esp_cpu_get_curr_privilege_level());

    /* NOTE: M/U-mode PLIC Special Configuration Register
     * Bit 0: Use the external PLIC registers (legacy) from the SoC (default)
     * Bit 1: Use the internal PLIC registers as per the new SoC address map
     */
    REG_SET_BIT(PLIC_MXINT_CONF_REG, BIT(0));
    REG_SET_BIT(PLIC_UXINT_CONF_REG, BIT(0));

    /* Setting the M-mode vector table */
    rv_utils_set_mtvec((uint32_t)&_vector_table);

    /* Disable global interrupts */
    RV_CLEAR_CSR(mstatus, MSTATUS_UIE);
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);

    /* Clear all interrupts */
    RV_WRITE_CSR(mie, 0x00);
    RV_WRITE_CSR(uie, 0x00);

    /* All interrupts except the TEE secure interrupt are delegated to the U-mode */
    uint32_t mideleg_val = UINT32_MAX;
    CLR_BIT(mideleg_val, TEE_SECURE_INUM);
    RV_WRITE_CSR(mideleg, mideleg_val);

    /* TODO: IDF-8958
     * The values for the secure interrupt number and priority and
     * the interrupt priority threshold (for both M and U mode) need
     * to be investigated further
     */
#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
    /* TODO: Currently, we do not allow interrupts to be set up with a priority greater than 7, see intr_alloc.c */
    esprv_int_set_priority(TEE_SECURE_INUM, 7);
    esprv_int_set_type(TEE_SECURE_INUM, ESP_CPU_INTR_TYPE_LEVEL);
    esprv_int_set_threshold(1);
    esprv_int_enable(BIT(TEE_SECURE_INUM));
#endif

    ESP_LOGD(TAG, "Initial interrupt config -");
    ESP_LOGD(TAG, "mideleg: 0x%08x", RV_READ_CSR(mideleg));
    ESP_LOGD(TAG, "mie: 0x%08x | uie: 0x%08x", RV_READ_CSR(mie), RV_READ_CSR(uie));
    ESP_LOGD(TAG, "mstatus: 0x%08x | ustatus: 0x%08x", RV_READ_CSR(mstatus), RV_READ_CSR(ustatus));
    ESP_LOGD(TAG, "[PLIC] MX: 0x%08x | UX: 0x%08x", REG_READ(PLIC_MXINT_ENABLE_REG), REG_READ(PLIC_UXINT_ENABLE_REG));

    /* PMP, PMA and APM configuration to isolate the resources between TEE and REE. */
    esp_tee_configure_region_protection();
    esp_tee_configure_apm_protection();

    /* Protect secure interrupt sources */
    esp_tee_protect_intr_src(ETS_LP_APM_M0_INTR_SOURCE);    // LP_APM_M0
    esp_tee_protect_intr_src(ETS_LP_APM_M1_INTR_SOURCE);    // LP_APM_M1
    esp_tee_protect_intr_src(ETS_HP_APM_M0_INTR_SOURCE);    // HP_APM_M0
    esp_tee_protect_intr_src(ETS_HP_APM_M1_INTR_SOURCE);    // HP_APM_M1
    esp_tee_protect_intr_src(ETS_HP_APM_M2_INTR_SOURCE);    // HP_APM_M2
    esp_tee_protect_intr_src(ETS_HP_APM_M3_INTR_SOURCE);    // HP_APM_M3
    esp_tee_protect_intr_src(ETS_LP_APM0_INTR_SOURCE);      // LP_APM0
    esp_tee_protect_intr_src(ETS_EFUSE_INTR_SOURCE);        // eFuse
    esp_tee_protect_intr_src(ETS_AES_INTR_SOURCE);          // AES
    esp_tee_protect_intr_src(ETS_SHA_INTR_SOURCE);          // SHA

    /* Disable AES/SHA peripheral clocks; they will be toggled as needed when the peripheral is in use */
    periph_ll_disable_clk_set_rst(PERIPH_AES_MODULE);
    periph_ll_disable_clk_set_rst(PERIPH_SHA_MODULE);
}

IRAM_ATTR inline void esp_tee_switch_to_ree(uint32_t ree_entry_addr)
{
    /* 2nd argument is used as magic value to detect very first M2U switch */
    /* TBD: clean this up and use proper temporary register instead of a1 */
    /* Switch to non-secure world and launch App. */
    _m2u_switch(ree_entry_addr, ESP_TEE_M2U_SWITCH_MAGIC << 12);
}
