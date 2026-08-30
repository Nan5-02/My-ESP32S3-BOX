#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "esp_codec_dev.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t Drv_ES8311_Init(void);
esp_err_t Drv_ES8311_Deinit(void);
esp_err_t Drv_ES8311_Set_Output_Volume(int volume);
esp_err_t Drv_ES8311_Set_Input_Gain(float gain_db);
esp_err_t Drv_ES8311_Set_Output_Mute(bool mute);
esp_err_t Drv_ES8311_Set_Input_Mute(bool mute);
esp_err_t Drv_ES8311_Write(const void *data, size_t size);
esp_err_t Drv_ES8311_Read(void *data, size_t size);
esp_codec_dev_handle_t Drv_ES8311_Get_Codec_Handle(void);

#ifdef __cplusplus
}
#endif
