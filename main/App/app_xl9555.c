#include "esp_log.h"

#include "drv_xl9555.h"
#include "app_xl9555.h"


static const char *TAG = "app_xl9555";

esp_err_t App_XL9555_Init(void)
{
    esp_err_t ret = Drv_XL9555_Init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize XL9555: %s", esp_err_to_name(ret));
        return ret;
    }
    /* 先预置低电平再切换输出，避免切换方向时误使能功放。 */
    ret = Drv_XL9555_Set_Output_Level(APP_XL9555_ES8311_PA_PIN, false);
    if (ret == ESP_OK) {
        ret = Drv_XL9555_Set_Pin_Direction(APP_XL9555_ES8311_PA_PIN, false);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ES8311 PA pin: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t App_XL9555_Set_ES8311_PA_Enable(bool enable)
{
    return Drv_XL9555_Set_Output_Level(APP_XL9555_ES8311_PA_PIN, enable);
}