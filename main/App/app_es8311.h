#ifndef __APP_ES8311_H__
#define __APP_ES8311_H__

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_err.h"

/* ES8311 与 XL9555 共用的 I2C 总线；使用其他板卡时按原理图修改。 */




/** @brief 初始化 I2C、I2S 和 ES8311；可重复调用。 */
esp_err_t App_ES8311_Init(void);

/** @brief 关闭 ES8311 并删除应用层创建的 I2S 通道和 I2C 总线。 */
esp_err_t App_ES8311_Deinit(void);

/** @brief 设置 ES8311 DAC 输出音量，范围为 0 到 100。 */
esp_err_t App_ES8311_Set_Output_Volume(int volume);

/** @brief 设置或取消 ES8311 DAC 输出静音。 */
esp_err_t App_ES8311_Set_Output_Mute(bool mute);

#endif /* __APP_ES8311_H__ */