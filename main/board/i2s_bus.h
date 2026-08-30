#ifndef __I2S_BUS_H__
#define __I2S_BUS_H__

#include "driver/i2s_std.h"

#define CONFIG_APP_ES8311_I2S_PORT          I2S_NUM_0
#define CONFIG_APP_ES8311_I2S_MCLK_PIN      -1
#define CONFIG_APP_ES8311_I2S_BCLK_PIN      GPIO_NUM_21
#define CONFIG_APP_ES8311_I2S_WS_PIN        GPIO_NUM_13
#define CONFIG_APP_ES8311_I2S_DOUT_PIN      GPIO_NUM_14
#define CONFIG_APP_ES8311_I2S_DIN_PIN       GPIO_NUM_47
#define CONFIG_APP_ES8311_SAMPLE_RATE       16000
#define CONFIG_APP_ES8311_BITS_PER_SAMPLE   16
#define CONFIG_APP_ES8311_PA_PIN            -1
#define CONFIG_APP_ES8311_PA_REVERTED       false
#define CONFIG_APP_ES8311_MCLK_MULTIPLE     256
#define CONFIG_APP_ES8311_CHANNEL_COUNT     2
#define CONFIG_APP_ES8311_CHANNEL_MASK      0x03
#define CONFIG_APP_ES8311_OUTPUT_VOLUME     70
#define CONFIG_APP_ES8311_INPUT_GAIN_DB     0.0f
#define CONFIG_APP_ES8311_WORK_MODE         ESP_CODEC_DEV_WORK_MODE_BOTH

esp_err_t Board_I2S_Bus_Init(void);
void Board_I2S_Bus_Deinit(void);
i2s_chan_handle_t *Board_Get_I2S_Tx_Handle(void);
i2s_chan_handle_t *Board_Get_I2S_Rx_Handle(void);

#endif /* __I2S_BUS_H__ */