#include "rtos_task.h"
#include "rtos_msg.h"
#include "esp_log.h"
#include "sta_wifi.h"
#include "sta_main.h"
#include "sta_audio.h"

static const char *TAG = "RTOS_TASK";

// ============ 任务创建入口 ============
void Rtos_Task_Create(void)
{
    uint8_t ret = 0;
    // 创建消息队列
    ret = Rtos_msg_create();
    ESP_LOGI(TAG, "Message queues created ret=%d", ret);


    xTaskCreate(main_task, "main_task", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIO, NULL);
    ESP_LOGI(TAG, "Task created: main_task");

    // 创建通信任务
    xTaskCreate(wifi_task, "wifi_task", WIFI_TASK_STACK_SIZE, NULL, WIFI_TASK_PRIO, NULL);
    ESP_LOGI(TAG, "Task created: wifi_task");

    xTaskCreate(audio_task, "audio_task", AUDIO_TASK_STACK_SIZE, NULL, AUDIO_TASK_PRIO, NULL);
    ESP_LOGI(TAG, "Task created: audio_task");


}
