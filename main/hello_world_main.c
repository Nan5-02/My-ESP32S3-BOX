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
#include "app_memory.h"
#include "app_wifi.h"
#include "app_nvs_flash.h"
#include "app_es8311.h"
#include "drv_xl9555.h"


void app_main(void)
{
    printf("Hello world!\n");
    App_NVS_Flash_Init();
    App_Wifi_Init();
    App_ES8311_Init();
    App_Memory_Init();
}
