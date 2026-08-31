#ifndef __APP_XL9555_H__
#define __APP_XL9555_H__

#include <stdbool.h>

#include "esp_err.h"

/** XL9555 IO0_5 控制 ES8311 外部功放使能，高电平有效。 */
#define APP_XL9555_ES8311_PA_PIN  5

/** @brief 初始化 XL9555，并将 ES8311 功放保持关闭。 */
esp_err_t App_XL9555_Init(void);

/** @brief 设置 ES8311 外部功放使能状态。 */
esp_err_t App_XL9555_Set_ES8311_PA_Enable(bool enable);

#endif /* __APP_XL9555_H__ */