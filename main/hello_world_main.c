/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtos_task.h"
#include "i2c_bus.h"
#include "i2s_bus.h"
#include "app_xl9555.h"
#include "app_es8311.h"
#include "app_memory.h"




void app_main(void)
{
    printf("Hello world!\n");
    Board_I2C_Bus_Init();
    Board_I2S_Bus_Init();
    App_XL9555_Init();
    App_ES8311_Init();
    App_Memory_Init();
    Rtos_Task_Create();
}
