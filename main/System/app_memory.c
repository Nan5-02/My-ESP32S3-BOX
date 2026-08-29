#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_memory.h"

#include "app_memory.h"

static const char *TAG = "App_Memory";

void App_Memory_Init(void)
{
    App_Memory_Log_Flash_Info();
    App_Memory_Log_Psram_Info();
}

void App_Memory_Log_Flash_Info(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_size;

    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "This is ESP32 chip with %d CPU cores, WiFi%s%s, silicon revision %d",
             chip_info.cores,
             (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
             chip_info.revision);

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    ESP_LOGI(TAG, "silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        ESP_LOGE(TAG, "Get flash size failed");
        return;
    }

    ESP_LOGI(TAG, "%" PRIu32 "MB %s flash", flash_size / (uint32_t)(1024 * 1024),
             (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    ESP_LOGI(TAG, "Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());
}

void App_Memory_Log_Psram_Info(void)
{
    size_t psram_total_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    if (psram_total_size > 0)
    {
        ESP_LOGI(TAG, "PSRAM total size: %u bytes, minimum free size: %u bytes",
                 (unsigned int)psram_total_size,
                 (unsigned int)heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
    }
    else
    {
        ESP_LOGW(TAG, "PSRAM is unavailable");
    }
}