#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "esp_log.h"

#include "drv_es8311.h"
#include "app_es8311.h"

static const char *TAG = "app_es8311";

/* 应用层持有总线和 codec 句柄，确保后续音量控制和反初始化可用。 */
static i2c_master_bus_handle_t s_i2c_bus;
static i2s_chan_handle_t s_i2s_tx_handle;
static i2s_chan_handle_t s_i2s_rx_handle;
static drv_es8311_handle_t s_es8311_handle;

static esp_err_t init_i2c_bus(void)
{
    i2c_master_bus_config_t config = {
        .i2c_port = CONFIG_APP_ES8311_I2C_PORT,
        .sda_io_num = CONFIG_APP_ES8311_I2C_SDA_PIN,
        .scl_io_num = CONFIG_APP_ES8311_I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    return i2c_new_master_bus(&config, &s_i2c_bus);
}

static esp_err_t init_i2s_channels(void)
{
    i2s_chan_config_t channel_config = I2S_CHANNEL_DEFAULT_CONFIG(
        CONFIG_APP_ES8311_I2S_PORT, I2S_ROLE_MASTER);
    channel_config.auto_clear = true;
    esp_err_t ret = i2s_new_channel(&channel_config, &s_i2s_tx_handle, &s_i2s_rx_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    i2s_std_config_t standard_config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(CONFIG_APP_ES8311_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            CONFIG_APP_ES8311_BITS_PER_SAMPLE, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = CONFIG_APP_ES8311_I2S_MCLK_PIN,
            .bclk = CONFIG_APP_ES8311_I2S_BCLK_PIN,
            .ws = CONFIG_APP_ES8311_I2S_WS_PIN,
            .dout = CONFIG_APP_ES8311_I2S_DOUT_PIN,
            .din = CONFIG_APP_ES8311_I2S_DIN_PIN,
        },
    };
    ret = i2s_channel_init_std_mode(s_i2s_tx_handle, &standard_config);
    if (ret == ESP_OK) {
        ret = i2s_channel_init_std_mode(s_i2s_rx_handle, &standard_config);
    }
    if (ret == ESP_OK) {
        ret = i2s_channel_enable(s_i2s_tx_handle);
    }
    if (ret == ESP_OK) {
        ret = i2s_channel_enable(s_i2s_rx_handle);
    }
    return ret;
}

esp_err_t App_ES8311_Init(void)
{
    if (s_es8311_handle != NULL) {
        return ESP_OK;
    }

    esp_err_t ret = init_i2c_bus();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C initialization failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = init_i2s_channels();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2S initialization failed: %s", esp_err_to_name(ret));
        App_ES8311_Deinit();
        return ret;
    }

    drv_es8311_config_t es8311_config = DRV_ES8311_CONFIG_DEFAULT(
        s_i2c_bus, s_i2s_tx_handle, s_i2s_rx_handle);
    es8311_config.pa_pin = CONFIG_APP_ES8311_PA_PIN;
    es8311_config.pa_reverted = CONFIG_APP_ES8311_PA_REVERTED;
    es8311_config.sample_rate_hz = CONFIG_APP_ES8311_SAMPLE_RATE;
    es8311_config.mclk_multiple = CONFIG_APP_ES8311_MCLK_MULTIPLE;
    es8311_config.bits_per_sample = CONFIG_APP_ES8311_BITS_PER_SAMPLE;
    es8311_config.channel_count = CONFIG_APP_ES8311_CHANNEL_COUNT;
    es8311_config.channel_mask = CONFIG_APP_ES8311_CHANNEL_MASK;
    es8311_config.output_volume = CONFIG_APP_ES8311_OUTPUT_VOLUME;
    es8311_config.input_gain_db = CONFIG_APP_ES8311_INPUT_GAIN_DB;
    es8311_config.work_mode = CONFIG_APP_ES8311_WORK_MODE;

    ret = Drv_ES8311_Init(&es8311_config, &s_es8311_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ES8311 initialization failed: %s", esp_err_to_name(ret));
        App_ES8311_Deinit();
        return ret;
    }
    ESP_LOGI(TAG, "ES8311 initialized successfully");
    return ESP_OK;
}

esp_err_t App_ES8311_Deinit(void)
{
    esp_err_t ret = ESP_OK;
    if (s_es8311_handle != NULL) {
        ret = Drv_ES8311_Deinit(s_es8311_handle);
        s_es8311_handle = NULL;
    }
    if (s_i2s_tx_handle != NULL) {
        i2s_channel_disable(s_i2s_tx_handle);
        i2s_del_channel(s_i2s_tx_handle);
        s_i2s_tx_handle = NULL;
    }
    if (s_i2s_rx_handle != NULL) {
        i2s_channel_disable(s_i2s_rx_handle);
        i2s_del_channel(s_i2s_rx_handle);
        s_i2s_rx_handle = NULL;
    }
    if (s_i2c_bus != NULL) {
        i2c_del_master_bus(s_i2c_bus);
        s_i2c_bus = NULL;
    }
    return ret;
}

esp_err_t App_ES8311_Set_Output_Volume(int volume)
{
    if (s_es8311_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return Drv_ES8311_Set_Output_Volume(s_es8311_handle, volume);
}

esp_err_t App_ES8311_Set_Output_Mute(bool mute)
{
    if (s_es8311_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return Drv_ES8311_Set_Output_Mute(s_es8311_handle, mute);
}