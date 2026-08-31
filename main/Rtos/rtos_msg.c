#include "rtos_msg.h"
#include "esp_log.h"

static const char *TAG = "RTOS_MSG";

QueueHandle_t task_msg_queue_handle[TASK_TYPE_MAX] = {NULL};

uint8_t Rtos_msg_create(void)
{
    uint8_t ret = 0;
    task_msg_queue_handle[WIFI_TASK] = xQueueCreate( WIFI_TASK_MSG_QUEUE_LENGTH, WIFI_TASK_MSG_QUEUE_SIZE);
    if(task_msg_queue_handle[WIFI_TASK] == NULL){
        ESP_LOGE(TAG, "WIFI Task Queue Create Fail\r\n");
        ret++;
    }
    task_msg_queue_handle[STAMAIN_TASK] = xQueueCreate(STAMAIN_TASK_MSG_QUEUE_LENGTH, STAMAIN_TASK_MSG_QUEUE_SIZE);
    if (task_msg_queue_handle[STAMAIN_TASK] == NULL)
    {
        ESP_LOGE(TAG, "STA Main Task Queue Create Fail\r\n");
        ret++;
    }
    task_msg_queue_handle[AUDIO_TASK] = xQueueCreate(AUDIO_TASK_MSG_QUEUE_LENGTH, AUDIO_TASK_MSG_QUEUE_SIZE);
    if (task_msg_queue_handle[AUDIO_TASK] == NULL) {
        ESP_LOGE(TAG, "Audio Task Queue Create Fail\r\n");
        ret++;
    }
    return ret;
}


BaseType_t Rtos_msg_send(TaskType_Enu task_type, IO_MSG_t *msg, TickType_t ticks_to_wait)
{
    BaseType_t ret;

    if ((task_type >= TASK_TYPE_MAX) || (msg == NULL)) {
        ESP_LOGE(TAG, "Rtos_msg_send invalid param, task=%d msg=%p", task_type, (void *)msg);
        return pdFAIL;
    }
    if (task_msg_queue_handle[task_type] == NULL) {
        ESP_LOGE(TAG, "Rtos_msg_send queue NULL, task=%d", task_type);
        return pdFAIL;
    }
    ret = xQueueSend(task_msg_queue_handle[task_type], msg, ticks_to_wait);
    if (ret != pdTRUE) {
        ESP_LOGE(TAG, "Rtos_msg_send failed, task=%d type=%u subtype=%u wait=%lu ret=%d",
                 task_type, (unsigned)msg->type, (unsigned)msg->subtype,
                 (unsigned long)ticks_to_wait, (int)ret);
    }
    return ret;
}

BaseType_t Rtos_msg_send_from_isr(TaskType_Enu task_type, IO_MSG_t *msg, BaseType_t *pxHigherPriorityTaskWoken)
{
    if ((task_type >= TASK_TYPE_MAX) || (msg == NULL)) {
        return pdFAIL;
    }
    if (task_msg_queue_handle[task_type] == NULL) {
        return pdFAIL;
    }
    BaseType_t ret = xQueueSendFromISR(task_msg_queue_handle[task_type], msg, pxHigherPriorityTaskWoken);
    if (ret != pdTRUE) {
        ESP_EARLY_LOGE(TAG, "Rtos_msg_send_from_isr failed, task=%d type=%u subtype=%u ret=%d",
                       task_type, (unsigned)msg->type, (unsigned)msg->subtype, (int)ret);
    }
    return ret;
}

BaseType_t Rtos_msg_receive(TaskType_Enu task_type, IO_MSG_t *msg, TickType_t ticks_to_wait)
{
    BaseType_t ret;

    if ((task_type >= TASK_TYPE_MAX) || (msg == NULL)) {
        ESP_LOGE(TAG, "Rtos_msg_receive invalid param, task=%d msg=%p", task_type, (void *)msg);
        return pdFAIL;
    }
    if (task_msg_queue_handle[task_type] == NULL) {
        ESP_LOGE(TAG, "Rtos_msg_receive queue NULL, task=%d", task_type);
        return pdFAIL;
    }
    ret = xQueueReceive(task_msg_queue_handle[task_type], msg, ticks_to_wait);
    if (ret != pdTRUE) {
        ESP_LOGD(TAG, "Rtos_msg_receive timeout, task=%d wait=%lu ret=%d",
                 task_type, (unsigned long)ticks_to_wait, (int)ret);
    }
    return ret;
}


