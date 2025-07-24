/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_spiffs.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "esp_vfs_fat.h"
#include "usb/usb_host.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "esp_lcd_ek79007.h"
#include "bsp/wt99p4c5_s1_board.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_lcd_touch_gt911.h"
#include "bsp_err_check.h"
#include "esp_codec_dev_defaults.h"
#include "esp_eth_driver.h"
#include "esp_eth_netif_glue.h"
#include "esp_netif.h"
#include "esp_event.h"

static const char *TAG = "WT99P4C5_S1_BOARD";

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
static lv_indev_t *disp_indev = NULL;
#endif // (BSP_CONFIG_NO_GRAPHIC_LIB == 0)

static sdmmc_card_t *bsp_sdcard = NULL; // uSD card handle
static bool i2c_initialized = false;
static bool spi_sd_initialized = false;
static sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL; // SD LDO handle
static TaskHandle_t usb_host_task;                  // USB Host Library task
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0))
static i2c_master_bus_handle_t i2c_handle = NULL; // I2C Handle
#endif
static i2s_chan_handle_t i2s_tx_chan = NULL;
static i2s_chan_handle_t i2s_rx_chan = NULL;
static const audio_codec_data_if_t *i2s_data_if = NULL; /* Codec data interface */
static bsp_lcd_handles_t disp_handles;
static esp_ldo_channel_handle_t disp_phy_pwr_chan = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_io_handle_t tp_io_handle = NULL;

static esp_eth_handle_t eth_handle = NULL;
static esp_eth_netif_glue_handle_t glue = NULL;
static esp_netif_t *eth_netif = NULL;

/* Can be used for `i2s_std_gpio_config_t` and/or `i2s_std_config_t` initialization */
#define BSP_I2S_GPIO_CFG           \
    {                              \
        .mclk = BSP_I2S_MCLK,      \
        .bclk = BSP_I2S_SCLK,      \
        .ws = BSP_I2S_LCLK,        \
        .dout = BSP_I2S_DOUT,      \
        .din = BSP_I2S_DSIN,       \
        .invert_flags =            \
            {                      \
                .mclk_inv = false, \
                .bclk_inv = false, \
                .ws_inv = false,   \
            },                     \
    }

/* This configuration is used by default in `bsp_extra_audio_init()` */
#define BSP_I2S_DUPLEX_MONO_CFG(_sample_rate)                                                         \
    {                                                                                                 \
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(_sample_rate),                                          \
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO), \
        .gpio_cfg = BSP_I2S_GPIO_CFG,                                                                 \
    }

esp_err_t bsp_i2c_init(void)
{
    /* I2C was initialized before */
    if (i2c_initialized) {
        return ESP_OK;
    }

    i2c_master_bus_config_t i2c_bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .i2c_port = BSP_I2C_NUM,
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&i2c_bus_conf, &i2c_handle));

    i2c_initialized = true;

    return ESP_OK;
}

esp_err_t bsp_i2c_deinit(void)
{
    if (i2c_initialized && i2c_handle) {
        BSP_ERROR_CHECK_RETURN_ERR(i2c_del_master_bus(i2c_handle));
        i2c_initialized = false;
    }
    return ESP_OK;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    return i2c_handle;
}

sdmmc_card_t *bsp_sdcard_get_handle(void)
{
    return bsp_sdcard;
}

void bsp_sdcard_get_sdmmc_host(const int slot, sdmmc_host_t *config)
{
    assert(config);

    sdmmc_host_t host_config = SDMMC_HOST_DEFAULT();
    host_config.slot = slot;
    host_config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    memcpy(config, &host_config, sizeof(sdmmc_host_t));
}

void bsp_sdcard_get_sdspi_host(const int slot, sdmmc_host_t *config)
{
    assert(config);

    sdmmc_host_t host_config = SDSPI_HOST_DEFAULT();
    host_config.slot = slot;

    memcpy(config, &host_config, sizeof(sdmmc_host_t));
}

void bsp_sdcard_sdmmc_get_slot(const int slot, sdmmc_slot_config_t *config)
{
    assert(config);
    memset(config, 0, sizeof(sdmmc_slot_config_t));

    /* SD card is connected to Slot 0 pins. Slot 0 uses IO MUX, so not specifying the pins here */
    config->cd = SDMMC_SLOT_NO_CD;
    config->wp = SDMMC_SLOT_NO_WP;
    config->width = 4;
    config->flags = 0;
}

void bsp_sdcard_sdspi_get_slot(const spi_host_device_t spi_host, sdspi_device_config_t *config)
{
    assert(config);
    memset(config, 0, sizeof(sdspi_device_config_t));

    config->gpio_cs = BSP_SD_SPI_CS;
    config->gpio_cd = SDSPI_SLOT_NO_CD;
    config->gpio_wp = SDSPI_SLOT_NO_WP;
    config->gpio_int = GPIO_NUM_NC;
    config->gpio_wp_polarity = SDSPI_IO_ACTIVE_LOW;
    config->host_id = spi_host;
}

esp_err_t bsp_sdcard_sdmmc_mount(bsp_sdcard_cfg_t *cfg)
{
    sdmmc_host_t sdhost = {0};
    sdmmc_slot_config_t sdslot = {0};
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {.format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 64 * 1024};
    assert(cfg);

    if (!cfg->mount) {
        cfg->mount = &mount_config;
    }

    if (!cfg->host) {
        bsp_sdcard_get_sdmmc_host(SDMMC_HOST_SLOT_0, &sdhost);
        cfg->host = &sdhost;
    }

    if (!cfg->slot.sdmmc) {
        bsp_sdcard_sdmmc_get_slot(SDMMC_HOST_SLOT_0, &sdslot);
        cfg->slot.sdmmc = &sdslot;
    }

    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4,
    };
    esp_err_t ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
        return ret;
    }
    cfg->host->pwr_ctrl_handle = pwr_ctrl_handle;

#if !CONFIG_FATFS_LONG_FILENAMES
    ESP_LOGW(TAG, "Warning: Long filenames on SD card are disabled in menuconfig!");
#endif

    return esp_vfs_fat_sdmmc_mount(BSP_SD_MOUNT_POINT, cfg->host, cfg->slot.sdmmc, cfg->mount, &bsp_sdcard);
}

esp_err_t bsp_sdcard_sdspi_mount(bsp_sdcard_cfg_t *cfg)
{
    sdmmc_host_t sdhost = {0};
    sdspi_device_config_t sdslot = {0};
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {.format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 64 * 1024};
    assert(cfg);

    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = BSP_SD_SPI_CLK,
        .mosi_io_num = BSP_SD_SPI_MOSI,
        .miso_io_num = BSP_SD_SPI_MISO,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = 4000,
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_SDSPI_HOST, &buscfg, SDSPI_DEFAULT_DMA), TAG, "SPI init failed");
    spi_sd_initialized = true;

    if (!cfg->mount) {
        cfg->mount = &mount_config;
    }

    if (!cfg->host) {
        bsp_sdcard_get_sdspi_host(SDMMC_HOST_SLOT_0, &sdhost);
        cfg->host = &sdhost;
    }

    if (!cfg->slot.sdspi) {
        bsp_sdcard_sdspi_get_slot(BSP_SDSPI_HOST, &sdslot);
        cfg->slot.sdspi = &sdslot;
    }

    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4,
    };
    esp_err_t ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
        return ret;
    }
    cfg->host->pwr_ctrl_handle = pwr_ctrl_handle;

    ESP_LOGW(TAG, "Warning: Long filenames on SD card are disabled in menuconfig!");

    return esp_vfs_fat_sdspi_mount(BSP_SD_MOUNT_POINT, cfg->host, cfg->slot.sdspi, cfg->mount, &bsp_sdcard);
}

esp_err_t bsp_sdcard_mount(void)
{
    bsp_sdcard_cfg_t cfg = {0};
    return bsp_sdcard_sdmmc_mount(&cfg);
}

esp_err_t bsp_sdcard_unmount(void)
{
    esp_err_t ret = ESP_OK;

    if (pwr_ctrl_handle) {
        ret |= sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
        pwr_ctrl_handle = NULL;
    }

    ret |= esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, bsp_sdcard);
    bsp_sdcard = NULL;

    if (spi_sd_initialized) {
        ret |= spi_bus_free(BSP_SDSPI_HOST);
        spi_sd_initialized = false;
    }

    return ret;
}

esp_err_t bsp_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_BSP_SPIFFS_MOUNT_POINT,
        .partition_label = CONFIG_BSP_SPIFFS_PARTITION_LABEL,
        .max_files = CONFIG_BSP_SPIFFS_MAX_FILES,
        .format_if_mount_failed = false,
    };

    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    BSP_ERROR_CHECK_RETURN_ERR(ret_val);

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

esp_err_t bsp_spiffs_unmount(void)
{
    return esp_vfs_spiffs_unregister(CONFIG_BSP_SPIFFS_PARTITION_LABEL);
}

esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config)
{
    if (i2s_tx_chan && i2s_rx_chan) {
        /* Audio was initialized before */
        return ESP_OK;
    }

    /* Setup I2S peripheral */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(CONFIG_BSP_I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true; // Auto clear the legacy data in the DMA buffer
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &i2s_tx_chan, &i2s_rx_chan));

    /* Setup I2S channels */
    const i2s_std_config_t std_cfg_default = BSP_I2S_DUPLEX_MONO_CFG(22050);
    const i2s_std_config_t *p_i2s_cfg = &std_cfg_default;
    if (i2s_config != NULL) {
        p_i2s_cfg = i2s_config;
    }

    if (i2s_tx_chan != NULL) {
        ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_tx_chan, p_i2s_cfg));
        ESP_ERROR_CHECK(i2s_channel_enable(i2s_tx_chan));
    }

    if (i2s_rx_chan != NULL) {
        ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_rx_chan, p_i2s_cfg));
        ESP_ERROR_CHECK(i2s_channel_enable(i2s_rx_chan));
    }

    audio_codec_i2s_cfg_t i2s_cfg = {
        .port = CONFIG_BSP_I2S_NUM,
        .tx_handle = i2s_tx_chan,
        .rx_handle = i2s_rx_chan,
    };
    i2s_data_if = audio_codec_new_i2s_data(&i2s_cfg);

    return ESP_OK;
}

esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    if (i2s_data_if == NULL) {
        /* Initilize I2C */
        ESP_ERROR_CHECK(bsp_i2c_init());
        /* Configure I2S peripheral and Power Amplifier */
        ESP_ERROR_CHECK(bsp_audio_init(NULL));
    }
    assert(i2s_data_if);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_handle,
    };
    const audio_codec_ctrl_if_t *i2c_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(i2c_ctrl_if);

    esp_codec_dev_hw_gain_t gain = {
        .pa_voltage = 5.0,
        .codec_dac_voltage = 3.3,
    };

    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = i2c_ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_TYPE_OUT,
        .pa_pin = BSP_POWER_AMP_IO,
        .pa_reverted = false,
        .master_mode = false,
        .use_mclk = true,
        .digital_mic = false,
        .invert_mclk = false,
        .invert_sclk = false,
        .hw_gain = gain,
    };
    const audio_codec_if_t *es8311_dev = es8311_codec_new(&es8311_cfg);
    assert(es8311_dev);

    esp_codec_dev_cfg_t codec_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN_OUT,
        .codec_if = es8311_dev,
        .data_if = i2s_data_if,
    };
    return esp_codec_dev_new(&codec_dev_cfg);
}

esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    if (i2s_data_if == NULL) {
        /* Initilize I2C */
        ESP_ERROR_CHECK(bsp_i2c_init());
        /* Configure I2S peripheral and Power Amplifier */
        ESP_ERROR_CHECK(bsp_audio_init(NULL));
    }
    assert(i2s_data_if);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_handle,
    };
    const audio_codec_ctrl_if_t *i2c_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(i2c_ctrl_if);

    esp_codec_dev_hw_gain_t gain = {
        .pa_voltage = 5.0,
        .codec_dac_voltage = 3.3,
    };

    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = i2c_ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,
        .pa_pin = BSP_POWER_AMP_IO,
        .pa_reverted = false,
        .master_mode = false,
        .use_mclk = true,
        .digital_mic = false,
        .invert_mclk = false,
        .invert_sclk = false,
        .hw_gain = gain,
    };

    const audio_codec_if_t *es8311_dev = es8311_codec_new(&es8311_cfg);
    assert(es8311_dev);

    esp_codec_dev_cfg_t codec_es8311_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN,
        .codec_if = es8311_dev,
        .data_if = i2s_data_if,
    };
    return esp_codec_dev_new(&codec_es8311_dev_cfg);
}

// Bit number used to represent command and parameter
#define LCD_LEDC_CH CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH

esp_err_t bsp_display_brightness_init(void)
{
    // Setup LEDC peripheral for PWM backlight control
    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num = BSP_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = 1,
        .duty = 0,
        .hpoint = 0,
    };
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = 1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    BSP_ERROR_CHECK_RETURN_ERR(ledc_timer_config(&LCD_backlight_timer));
    BSP_ERROR_CHECK_RETURN_ERR(ledc_channel_config(&LCD_backlight_channel));
    return ESP_OK;
}

esp_err_t bsp_display_brightness_deinit(void)
{
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = 1,
        .deconfigure = 1,
    };
    BSP_ERROR_CHECK_RETURN_ERR(ledc_timer_pause(LEDC_LOW_SPEED_MODE, 1));
    BSP_ERROR_CHECK_RETURN_ERR(ledc_timer_config(&LCD_backlight_timer));
    return ESP_OK;
}

esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    if (brightness_percent < 0) {
        brightness_percent = 0;
    }

    ESP_LOGI(TAG, "Setting LCD backlight: %d%%", brightness_percent);
    uint32_t duty_cycle = (1023 * brightness_percent) / 100; // LEDC resolution set to 10bits, thus: 100% = 1023
    BSP_ERROR_CHECK_RETURN_ERR(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty_cycle));
    BSP_ERROR_CHECK_RETURN_ERR(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH));
    return ESP_OK;
}

esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(100);
}

static esp_err_t bsp_enable_dsi_phy_power(void)
{
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    esp_ldo_channel_config_t ldo_cfg = {
        .chan_id = BSP_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = BSP_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_cfg, &disp_phy_pwr_chan), TAG, "Acquire LDO channel for DPHY failed");
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");

    return ESP_OK;
}

esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    bsp_lcd_handles_t handles;
    ret = bsp_display_new_with_handles(config, &handles);

    *ret_panel = handles.panel;
    *ret_io = handles.io;

    return ret;
}

esp_err_t bsp_display_new_with_handles(const bsp_display_config_t *config, bsp_lcd_handles_t *ret_handles)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t disp_panel = NULL;

    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Brightness init failed");
    ESP_RETURN_ON_ERROR(bsp_enable_dsi_phy_power(), TAG, "DSI PHY power failed");

    /* create MIPI DSI bus first, it will initialize the DSI PHY as well */
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = BSP_LCD_MIPI_DSI_LANE_NUM,
        .phy_clk_src = config->dsi_bus.phy_clk_src,
        .lane_bit_rate_mbps = config->dsi_bus.lane_bit_rate_mbps,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus), TAG, "New DSI bus init failed");

    if (config->hdmi_resolution != BSP_HDMI_RES_NONE) {
        ESP_LOGW(TAG, "Please select HDMI in menuconfig, if you want to use it.");
    }

    ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
    // we use DBI interface to send LCD commands and parameters
    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,   // according to the LCD spec
        .lcd_param_bits = 8, // according to the LCD spec
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io), err, TAG, "New panel IO failed");

    // create EK79007 control panel
    ESP_LOGI(TAG, "Install EK79007 LCD control panel");

    esp_lcd_dpi_panel_config_t dpi_config = EK79007_1024_600_PANEL_60HZ_CONFIG(LCD_COLOR_PIXEL_FORMAT_RGB565);
    dpi_config.num_fbs = CONFIG_BSP_LCD_DPI_BUFFER_NUMS;

    ek79007_vendor_config_t vendor_config = {
        .mipi_config =
            {
                .dsi_bus = mipi_dsi_bus,
                .dpi_config = &dpi_config,
            },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = BSP_LCD_COLOR_SPACE,
        .reset_gpio_num = BSP_LCD_RST,
        .flags.reset_active_high = 1,
        .vendor_config = &vendor_config,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_ek79007(io, &lcd_dev_config, &disp_panel), err, TAG, "New LCD panel EK79007 failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(disp_panel), err, TAG, "LCD panel reset failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(disp_panel), err, TAG, "LCD panel init failed");

    /* Return all handles */
    ret_handles->io = io;
    disp_handles.io = io;
    ret_handles->mipi_dsi_bus = mipi_dsi_bus;
    disp_handles.mipi_dsi_bus = mipi_dsi_bus;
    ret_handles->panel = disp_panel;
    disp_handles.panel = disp_panel;
    ret_handles->control = NULL;
    disp_handles.control = NULL;

    ESP_LOGI(TAG, "Display initialized");

    return ret;

err:
    bsp_display_delete();
    return ret;
}

void bsp_display_delete(void)
{
    if (disp_handles.panel) {
        esp_lcd_panel_del(disp_handles.panel);
        disp_handles.panel = NULL;
    }
    if (disp_handles.io) {
        esp_lcd_panel_io_del(disp_handles.io);
        disp_handles.io = NULL;
    }
    if (disp_handles.mipi_dsi_bus) {
        esp_lcd_del_dsi_bus(disp_handles.mipi_dsi_bus);
        disp_handles.mipi_dsi_bus = NULL;
    }

    if (disp_phy_pwr_chan) {
        esp_ldo_release_channel(disp_phy_pwr_chan);
        disp_phy_pwr_chan = NULL;
    }

    bsp_display_brightness_deinit();
}

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
    /* Initilize I2C */
    BSP_ERROR_CHECK_RETURN_ERR(bsp_i2c_init());

    esp_lcd_touch_io_gt911_config_t dev_addr = {
        .dev_addr = 0x5D,
    };

    /* Initialize touch */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = BSP_LCD_TOUCH_RST, // Shared with LCD reset
        .int_gpio_num = BSP_LCD_TOUCH_INT,
        .levels =
            {
                .reset = 1,
                .interrupt = 0,
            },
        .flags =
            {
                .swap_xy = 0,
                .mirror_x = 1,
                .mirror_y = 1,
            },
        .driver_data = (void *)&dev_addr,
    };
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    tp_io_config.scl_speed_hz = CONFIG_BSP_I2C_CLK_SPEED_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), TAG, "");
    return esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, ret_touch);
}

void bsp_touch_delete(void)
{
    if (tp) {
        esp_lcd_touch_del(tp);
    }
    if (tp_io_handle) {
        esp_lcd_panel_io_del(tp_io_handle);
        tp_io_handle = NULL;
    }
}

static lv_display_t *bsp_display_lcd_init(const bsp_display_cfg_t *cfg)
{
    assert(cfg != NULL);
    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_new_with_handles(&cfg->hw_cfg, &disp_handles));

    uint32_t display_hres = BSP_LCD_H_RES;
    uint32_t display_vres = BSP_LCD_V_RES;

    ESP_LOGI(TAG, "Display resolution %ldx%ld", display_hres, display_vres);

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {.io_handle = disp_handles.io,
                                              .panel_handle = disp_handles.panel,
                                              .control_handle = disp_handles.control,
                                              .buffer_size = cfg->buffer_size,
                                              .double_buffer = cfg->double_buffer,
                                              .hres = display_hres,
                                              .vres = display_vres,
                                              .monochrome = false,
                                              /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
                                              .rotation =
                                                  {
                                                      .swap_xy = false,
                                                      .mirror_x = true,
                                                      .mirror_y = true,
                                                  },
                                              .flags = {
                                                  .buff_dma = cfg->flags.buff_dma,
                                                  .buff_spiram = cfg->flags.buff_spiram,
                                                  .sw_rotate = false, /* Avoid tearing is not supported for SW rotation */
                                                  .direct_mode = true,
                                              }};

    const lvgl_port_display_dsi_cfg_t dpi_cfg = {
        .flags =
            {
                .avoid_tearing = true,
            },
    };

    return lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);
}

static lv_indev_t *bsp_display_indev_init(lv_display_t *disp)
{
    /* Add touch input (for selected screen) */
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = tp,
    };

    return lvgl_port_add_touch(&touch_cfg);
}

lv_display_t *bsp_display_start(void)
{
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .hw_cfg =
            {
                .hdmi_resolution = BSP_HDMI_RES_NONE,
                .dsi_bus =
                    {
                        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
                        .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
                    },
            },
        .flags =
            {
                .buff_dma = true,
                .buff_spiram = true,
                .sw_rotate = true,
            },
    };
    return bsp_display_start_with_config(&cfg);
}

lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg)
{
    lv_display_t *disp;

    assert(cfg != NULL);
    BSP_ERROR_CHECK_RETURN_NULL(lvgl_port_init(&cfg->lvgl_port_cfg));

    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_brightness_init());

    BSP_ERROR_CHECK_RETURN_NULL(bsp_touch_new(NULL, &tp));
    assert(tp);

    BSP_NULL_CHECK(disp = bsp_display_lcd_init(cfg), NULL);
    BSP_NULL_CHECK(disp_indev = bsp_display_indev_init(disp), NULL);
    return disp;
}

void bsp_display_stop(lv_display_t *display)
{
    lvgl_port_remove_touch(disp_indev);
    lvgl_port_remove_disp(display);
    lvgl_port_deinit();

    /* Deinit touch */
    bsp_touch_delete();

    /* Deinit display */
    bsp_display_delete();

    /* Deinit I2C if initialized */
    bsp_i2c_deinit();
}

lv_indev_t *bsp_display_get_input_dev(void)
{
    return disp_indev;
}

void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation)
{
    lv_disp_set_rotation(disp, rotation);
}

bool bsp_display_lock(uint32_t timeout_ms)
{
    return lvgl_port_lock(timeout_ms);
}

void bsp_display_unlock(void)
{
    lvgl_port_unlock();
}

static void usb_lib_task(void *arg)
{
    while (1) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            ESP_ERROR_CHECK(usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            ESP_LOGI(TAG, "USB: All devices freed");
            // Continue handling USB events to allow device reconnection
            // The only way this task can be stopped is by calling bsp_usb_host_stop()
        }
    }
}

esp_err_t bsp_usb_host_start(bsp_usb_host_power_mode_t mode, bool limit_500mA)
{
    // Install USB Host driver. Should only be called once in entire application
    ESP_LOGI(TAG, "Installing USB Host");
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    BSP_ERROR_CHECK_RETURN_ERR(usb_host_install(&host_config));

    // Create a task that will handle USB library events
    if (xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, &usb_host_task) != pdTRUE) {
        ESP_LOGE(TAG, "Creating USB host lib task failed");
        abort();
    }

    return ESP_OK;
}

esp_err_t bsp_usb_host_stop(void)
{
    usb_host_uninstall();
    if (usb_host_task) {
        vTaskSuspend(usb_host_task);
        vTaskDelete(usb_host_task);
    }
    return ESP_OK;
}

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
        case ETHERNET_EVENT_CONNECTED:
            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
            ESP_LOGI(TAG, "Ethernet Link Up");
            ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet Link Down");
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet Started");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet Stopped");
            break;
        default:
            break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

esp_err_t bsp_eth_init(void)
{
    esp_err_t ret = ESP_OK;

    esp_netif_init();
    esp_event_loop_create_default();

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = CONFIG_BSP_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_BSP_ETH_PHY_RST_GPIO;
    // Init vendor specific MAC config to default
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    // Update vendor specific MAC config based on board configuration
    esp32_emac_config.smi_gpio.mdc_num = CONFIG_BSP_ETH_MDC_GPIO;
    esp32_emac_config.smi_gpio.mdio_num = CONFIG_BSP_ETH_MDIO_GPIO;

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_GOTO_ON_FALSE(esp_eth_driver_install(&config, &eth_handle) == ESP_OK, ESP_FAIL, err, TAG, "Ethernet driver install failed");

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&cfg);
    glue = esp_eth_new_netif_glue(eth_handle);
    ESP_GOTO_ON_FALSE(esp_netif_attach(eth_netif, glue) == ESP_OK, ESP_FAIL, err, TAG, "Failed to attach Ethernet driver to netif");

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    return ESP_OK;

err:
    if (eth_handle != NULL) {
        esp_eth_driver_uninstall(eth_handle);
    }
    return ret;
}

esp_eth_handle_t bsp_eth_get_handle(void)
{
    return eth_handle;
}

void bsp_eth_deinit(void)
{
    esp_eth_stop(eth_handle);
    esp_eth_del_netif_glue(glue);
    esp_netif_destroy(eth_netif);
    esp_netif_deinit();

    esp_eth_mac_t *mac = NULL;
    esp_eth_phy_t *phy = NULL;
    esp_eth_get_mac_instance(eth_handle, &mac);
    esp_eth_get_phy_instance(eth_handle, &phy);
    esp_eth_driver_uninstall(eth_handle);
    if (mac) {
        mac->del(mac);
    }
    if (phy) {
        phy->del(phy);
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    ESP_ERROR_CHECK(esp_event_loop_delete_default());

    eth_handle = NULL;
    glue = NULL;
    eth_netif = NULL;
}
