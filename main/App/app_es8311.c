#include "esp_log.h"

#include "board/i2c_bus.h"
#include "drv_es8311.h"
#include "drv_xl9555.h"
#include "app_es8311.h"

static const char *TAG = "app_es8311";

esp_err_t App_ES8311_Init(void)
{
    esp_err_t ret = Drv_ES8311_Init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ES8311 initialization failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}

esp_err_t App_ES8311_Deinit(void)
{
    esp_err_t ret = ESP_OK;
    esp_err_t codec_ret = Drv_ES8311_Deinit();
    return ret != ESP_OK ? ret : codec_ret;
}

esp_err_t App_ES8311_Set_Output_Volume(int volume)
{
    return Drv_ES8311_Set_Output_Volume(volume);
}

esp_err_t App_ES8311_Set_Output_Mute(bool mute)
{
    return Drv_ES8311_Set_Output_Mute(mute);
}
