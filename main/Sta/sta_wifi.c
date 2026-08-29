#include "rtos_task.h"
#include "rtos_msg.h"

#include "sta_wifi.h"
#include "app_system_time.h"
#include "app_wifi.h"
#include "app_nvs_flash.h"

// ============ 通信任务 ============
void wifi_task(void *pvParameters)
{
    IO_MSG_t msg;

    App_NVS_Flash_Init();
    App_Wifi_Init();

    uint32_t last_tick_ms = Systime_Get_MS(0);

    for (;;) {
        if(pdTRUE == Rtos_msg_receive(WIFI_TASK, &msg, pdMS_TO_TICKS(2))){
            switch(msg.type) {
                default:
                    break;
            }
        }
        if (Systime_Get_MS(last_tick_ms) >= 3) {
            last_tick_ms = Systime_Get_MS(0);
        }

        /* 防止消息队列持续非空时任务循环占满所在 CPU。 */
        vTaskDelay(1);
    }
}