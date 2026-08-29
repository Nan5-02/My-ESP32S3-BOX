#ifndef __APP_ES8311_H__
#define __APP_ES8311_H__

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_err.h"

/* ES8311 与 XL9555 共用的 I2C 总线；使用其他板卡时按原理图修改。 */
#define CONFIG_APP_ES8311_I2C_PORT          I2C_NUM_0
#define CONFIG_APP_ES8311_I2C_SDA_PIN       GPIO_NUM_48
#define CONFIG_APP_ES8311_I2C_SCL_PIN       GPIO_NUM_45
#define CONFIG_APP_ES8311_I2S_PORT          I2S_NUM_0
#define CONFIG_APP_ES8311_I2S_MCLK_PIN      GPIO_NUM_2
#define CONFIG_APP_ES8311_I2S_BCLK_PIN      GPIO_NUM_17
#define CONFIG_APP_ES8311_I2S_WS_PIN        GPIO_NUM_47
#define CONFIG_APP_ES8311_I2S_DOUT_PIN      GPIO_NUM_15
#define CONFIG_APP_ES8311_I2S_DIN_PIN       GPIO_NUM_16
#define CONFIG_APP_ES8311_PA_PIN            GPIO_NUM_4
#define CONFIG_APP_ES8311_PA_REVERTED       false
#define CONFIG_APP_ES8311_SAMPLE_RATE       16000
#define CONFIG_APP_ES8311_MCLK_MULTIPLE     256
#define CONFIG_APP_ES8311_BITS_PER_SAMPLE   16
#define CONFIG_APP_ES8311_CHANNEL_COUNT     2
#define CONFIG_APP_ES8311_CHANNEL_MASK      0x03
#define CONFIG_APP_ES8311_OUTPUT_VOLUME     70
#define CONFIG_APP_ES8311_INPUT_GAIN_DB     0.0f
#define CONFIG_APP_ES8311_WORK_MODE         ESP_CODEC_DEV_WORK_MODE_BOTH

/** @brief 初始化 I2C、I2S 和 ES8311；可重复调用。 */
esp_err_t App_ES8311_Init(void);

/** @brief 关闭 ES8311 并删除应用层创建的 I2S 通道和 I2C 总线。 */
esp_err_t App_ES8311_Deinit(void);

/** @brief 设置 ES8311 DAC 输出音量，范围为 0 到 100。 */
esp_err_t App_ES8311_Set_Output_Volume(int volume);

/** @brief 设置或取消 ES8311 DAC 输出静音。 */
esp_err_t App_ES8311_Set_Output_Mute(bool mute);

#endif /* __APP_ES8311_H__ */