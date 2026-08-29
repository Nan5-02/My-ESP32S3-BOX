#include <stdint.h>
#include <string.h>
#include <stdio.h>
// ESP-IDF 头文件
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

// 系统时间获取函数（ms）
// Systime_Get_MS(0) - 获取当前时间
// Systime_Get_MS(start) - 获取从start到现在的差值
uint32_t Systime_Get_MS(uint32_t ms)
{
    uint32_t tick = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    if (ms == 0) {
        return tick;
    }
    if (tick >= ms) {
        return tick - ms;
    } else {
        return (0xFFFFFFFFU - ms) + tick + 1;
    }
}
