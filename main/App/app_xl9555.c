#include "esp_log.h"

#include "drv_xl9555.h"


static const char *TAG = "app_xl9555";

void App_XL9555_Init(void)
{
    esp_err_t ret = Drv_XL9555_Init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize XL9555: %s", esp_err_to_name(ret));
    }
}