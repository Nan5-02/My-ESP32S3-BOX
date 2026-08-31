#include <stdlib.h>

#include "esp_codec_dev_defaults.h"
#include "esp_log.h"

#include "board/i2c_bus.h"
#include "board/i2s_bus.h"
#include "drv_es8311.h"

static const char *TAG = "drv_es8311";

typedef struct {
    esp_codec_dev_handle_t codec_dev;
    const audio_codec_if_t *codec_if;
    const audio_codec_data_if_t *data_if;
    const audio_codec_ctrl_if_t *ctrl_if;
    bool opened;
} drv_es8311_context_t;

static drv_es8311_context_t *s_context;

static esp_err_t codec_result(int result)
{
    return result == ESP_CODEC_DEV_OK ? ESP_OK : ESP_FAIL;
}

static void release_codec(drv_es8311_context_t *context)
{
    if (context == NULL) {
        return;
    }
    if (context->opened) {
        esp_codec_dev_close(context->codec_dev);
    }
    if (context->codec_dev != NULL) {
        esp_codec_dev_delete(context->codec_dev);
    }
    if (context->codec_if != NULL) {
        audio_codec_delete_codec_if(context->codec_if);
    }
    if (context->data_if != NULL) {
        audio_codec_delete_data_if(context->data_if);
    }
    if (context->ctrl_if != NULL) {
        audio_codec_delete_ctrl_if(context->ctrl_if);
    }
    free(context);
}

esp_err_t Drv_ES8311_Init(void)
{
    if (s_context != NULL) {
        return ESP_OK;
    }

    i2c_master_bus_handle_t i2c_bus = *Board_Get_I2C_Bus();
    i2s_chan_handle_t tx_handle = *Board_Get_I2S_Tx_Handle();
    i2s_chan_handle_t rx_handle = *Board_Get_I2S_Rx_Handle();
    if (i2c_bus == NULL || tx_handle == NULL || rx_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    drv_es8311_context_t *context = calloc(1, sizeof(*context));
    if (context == NULL) {
        return ESP_ERR_NO_MEM;
    }

    /* 添加 I2C 设备 */
    audio_codec_i2c_cfg_t i2c_config = {
        .port = CONFIG_APP_I2C_PORT,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_bus,
    };
    context->ctrl_if = audio_codec_new_i2c_ctrl(&i2c_config);

    /* 添加 I2S 设备 */
    audio_codec_i2s_cfg_t i2s_config = {
        .port = CONFIG_APP_ES8311_I2S_PORT,
        .rx_handle = rx_handle,
        .tx_handle = tx_handle,
    };
    context->data_if = audio_codec_new_i2s_data(&i2s_config);
    
    /* 添加 GPIO 接口 */
    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    if (context->ctrl_if == NULL || context->data_if == NULL || gpio_if == NULL) {
        release_codec(context);
        return ESP_ERR_NO_MEM;
    }

    /* 添加 ES8311 编解码器 */
    es8311_codec_cfg_t codec_config = {
        .ctrl_if = context->ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = CONFIG_APP_ES8311_WORK_MODE,
        .master_mode = false,
        .use_mclk = CONFIG_APP_ES8311_I2S_MCLK_PIN >= 0,
        .pa_pin = CONFIG_APP_ES8311_PA_PIN,
        .pa_reverted = CONFIG_APP_ES8311_PA_REVERTED,
        .hw_gain = {
            .pa_voltage = 5.0,
            .codec_dac_voltage = 3.3,
        },
        .mclk_div = CONFIG_APP_ES8311_MCLK_MULTIPLE,
    };
    context->codec_if = es8311_codec_new(&codec_config);
    if (context->codec_if == NULL) {
        release_codec(context);
        return ESP_FAIL;
    }

    /* 创建编解码器设备 */
    const bool input_enabled = CONFIG_APP_ES8311_WORK_MODE != ESP_CODEC_DEV_WORK_MODE_DAC;
    const bool output_enabled = CONFIG_APP_ES8311_WORK_MODE != ESP_CODEC_DEV_WORK_MODE_ADC;
    esp_codec_dev_cfg_t device_config = {
        .dev_type = (input_enabled ? ESP_CODEC_DEV_TYPE_IN : ESP_CODEC_DEV_TYPE_NONE) |
                    (output_enabled ? ESP_CODEC_DEV_TYPE_OUT : ESP_CODEC_DEV_TYPE_NONE),
        .codec_if = context->codec_if,
        .data_if = context->data_if,
    };
    context->codec_dev = esp_codec_dev_new(&device_config);
    if (context->codec_dev == NULL) {
        release_codec(context);
        return ESP_ERR_NO_MEM;
    }

    /* 配置采样参数 */
    esp_codec_dev_sample_info_t sample_config = {
        .bits_per_sample = CONFIG_APP_ES8311_BITS_PER_SAMPLE,
        .channel = CONFIG_APP_ES8311_CHANNEL_COUNT,
        .channel_mask = CONFIG_APP_ES8311_CHANNEL_MASK,
        .sample_rate = CONFIG_APP_ES8311_SAMPLE_RATE,
        .mclk_multiple = CONFIG_APP_ES8311_MCLK_MULTIPLE,
    };
    if (esp_codec_dev_open(context->codec_dev, &sample_config) != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "Failed to open ES8311");
        release_codec(context);
        return ESP_FAIL;
    }
    context->opened = true;
    s_context = context;

    esp_err_t ret = ESP_OK;
    if (output_enabled) {
        ret = Drv_ES8311_Set_Output_Volume(CONFIG_APP_ES8311_OUTPUT_VOLUME);
    }
    if (ret == ESP_OK && input_enabled) {
        ret = Drv_ES8311_Set_Input_Gain(CONFIG_APP_ES8311_INPUT_GAIN_DB);
    }
    if (ret != ESP_OK) {
        Drv_ES8311_Deinit();
    }
    return ret;
}

esp_err_t Drv_ES8311_Deinit(void)
{
    if (s_context == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = codec_result(esp_codec_dev_close(s_context->codec_dev));
    s_context->opened = false;
    release_codec(s_context);
    s_context = NULL;
    return ret;
}

esp_err_t Drv_ES8311_Set_Output_Volume(int volume)
{
    if (s_context == NULL || volume < 0 || volume > 100) {
        return ESP_ERR_INVALID_ARG;
    }
    return codec_result(esp_codec_dev_set_out_vol(s_context->codec_dev, volume));
}

esp_err_t Drv_ES8311_Set_Input_Gain(float gain_db)
{
    if (s_context == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return codec_result(esp_codec_dev_set_in_gain(s_context->codec_dev, gain_db));
}

esp_err_t Drv_ES8311_Set_Output_Mute(bool mute)
{
    if (s_context == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return codec_result(esp_codec_dev_set_out_mute(s_context->codec_dev, mute));
}

esp_err_t Drv_ES8311_Set_Input_Mute(bool mute)
{
    if (s_context == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return codec_result(esp_codec_dev_set_in_mute(s_context->codec_dev, mute));
}

esp_err_t Drv_ES8311_Write(const void *data, size_t size)
{
    if (s_context == NULL || data == NULL || size == 0 || size > INT32_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    return codec_result(esp_codec_dev_write(s_context->codec_dev, (void *)data, (int)size));
}

esp_err_t Drv_ES8311_Read(void *data, size_t size)
{
    if (s_context == NULL || data == NULL || size == 0 || size > INT32_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    return codec_result(esp_codec_dev_read(s_context->codec_dev, data, (int)size));
}

esp_codec_dev_handle_t Drv_ES8311_Get_Codec_Handle(void)
{
    return s_context == NULL ? NULL : s_context->codec_dev;
}
