#include <string.h>
#include <ctype.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include "wifi.h"

#include "esp_log.h"
static const char *TAG = "APP_WIFI";

#define ESP_WIFI_SSID "ace3pro"
#define ESP_WIFI_PASS "20040421jhb"
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

static void event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "WiFi event received: base=%s, id=%d", event_base, event_id);
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WiFi STA started");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WiFi STA connected");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGW(TAG, "WiFi STA disconnected, reason=%d",
                 ((const wifi_event_sta_disconnected_t *)event_data)->reason);
        break;
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "WiFi STA got IP: " IPSTR,
                 IP2STR(&((const ip_event_got_ip_t *)event_data)->ip_info.ip));
        break;
        
    }

}

void App_Wifi_Init(void)
{
    ESP_LOGI(TAG, "App WiFi init start");

    // 初始化TCP/IP适配器
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "TCP/IP adapter initialized");

    // 创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "Event loop created");

    // 创建默认WiFi STA网络接口
    esp_netif_create_default_wifi_sta();
    ESP_LOGI(TAG, "WiFi STA netif created");

    // 使用默认配置初始化WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "WiFi initialized");

    // 注册WiFi事件处理函数
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    ESP_LOGI(TAG, "WiFi event handlers registered");

    // 配置WiFi连接参数
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
#ifdef CONFIG_ESP_WIFI_WPA3_COMPATIBLE_SUPPORT
            .disable_wpa3_compatible_mode = 0,
#endif
        },
    };

    // 设置WiFi模式为STA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // 设置WiFi配置
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    // 启动WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi started: SSID=%s", ESP_WIFI_SSID);
    ESP_LOGI(TAG, "App WiFi init complete");
}