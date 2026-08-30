#include "driver/i2s_std.h"
#include "esp_log.h"
#include "i2s_bus.h"

static i2s_chan_handle_t s_i2s_tx_handle;
static i2s_chan_handle_t s_i2s_rx_handle;
    
static const char *TAG = "i2s_bus";

esp_err_t Board_I2S_Bus_Init(void)
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

void Board_I2S_Bus_Deinit(void)
{
    if (s_i2s_tx_handle != NULL) {
        i2s_del_channel(s_i2s_tx_handle);
        s_i2s_tx_handle = NULL;
    }
    if (s_i2s_rx_handle != NULL) {
        i2s_del_channel(s_i2s_rx_handle);
        s_i2s_rx_handle = NULL;
    }
}

i2s_chan_handle_t *Board_Get_I2S_Tx_Handle(void)
{
    return &s_i2s_tx_handle;
}

i2s_chan_handle_t *Board_Get_I2S_Rx_Handle(void)
{
    return &s_i2s_rx_handle;
}