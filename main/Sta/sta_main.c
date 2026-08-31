#include "rtos_task.h"
#include "rtos_msg.h"

#include "sta_main.h"
#include "app_system_time.h"
#include "app_memory.h"
#include "app_es8311.h"
#include "drv_xl9555.h"

// ============ 通信任务 ============
void main_task(void *pvParameters)
{
    IO_MSG_t msg;
    uint32_t last_tick_ms = Systime_Get_MS(0);

    for (;;) {
        if(pdTRUE == Rtos_msg_receive(STAMAIN_TASK, &msg, pdMS_TO_TICKS(2))){
            switch(msg.type) {
                default:
                    break;
            }
        }
        if (Systime_Get_MS(last_tick_ms) >= 3) {
            last_tick_ms = Systime_Get_MS(0);
        }

        /* 即使消息队列持续有数据，也必须让 CPU0 的 IDLE 任务获得执行机会。 */
        vTaskDelay(1);
    }
}