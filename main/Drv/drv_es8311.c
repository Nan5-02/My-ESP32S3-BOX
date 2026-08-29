#include <stdlib.h>

#include "esp_codec_dev_defaults.h"
#include "esp_log.h"

#include "drv_es8311.h"

static const char *TAG = "drv_es8311";

/* 保存本驱动创建的各层 codec 接口，便于按依赖关系逆序释放。 */
struct drv_es8311 {
	esp_codec_dev_handle_t codec_dev;       /* 面向应用的统一 codec 设备。 */
	const audio_codec_if_t *codec_if;       /* ES8311 寄存器和音频功能接口。 */
	const audio_codec_data_if_t *data_if;   /* 基于 I2S 的 PCM 数据接口。 */
	const audio_codec_ctrl_if_t *ctrl_if;   /* 基于 I2C 的寄存器控制接口。 */
	bool opened;                            /* 标记 codec_dev 是否已经打开。 */
};
/* 将 esp_codec_dev 的返回值统一转换成 ESP-IDF 的 esp_err_t。 */
static esp_err_t codec_result(int result)
{
	return result == ESP_CODEC_DEV_OK ? ESP_OK : ESP_FAIL;
}

/*
 * 释放已经成功创建的资源。该函数允许部分初始化状态，因此也用于初始化失败回滚。
 * GPIO 接口由 codec_if 接管，I2C 总线和 I2S 通道由调用者持有，均不在这里删除。
 */
static void release_codec(drv_es8311_handle_t handle)
{
	if (handle == NULL) {
		return;
	}
	if (handle->opened) {
		esp_codec_dev_close(handle->codec_dev);
	}
	if (handle->codec_dev != NULL) {
		esp_codec_dev_delete(handle->codec_dev);
	}
	if (handle->codec_if != NULL) {
		audio_codec_delete_codec_if(handle->codec_if);
	}
	if (handle->data_if != NULL) {
		audio_codec_delete_data_if(handle->data_if);
	}
	if (handle->ctrl_if != NULL) {
		audio_codec_delete_ctrl_if(handle->ctrl_if);
	}
	free(handle);
}

esp_err_t Drv_ES8311_Init(const drv_es8311_config_t *config, drv_es8311_handle_t *out_handle)
{
	/* 根据工作模式确定需要的 I2S 通道以及顶层设备能力。 */
	bool input_enabled = config != NULL &&
						 (config->work_mode & ESP_CODEC_DEV_WORK_MODE_ADC) != 0;
	bool output_enabled = config != NULL &&
						  (config->work_mode & ESP_CODEC_DEV_WORK_MODE_DAC) != 0;
	if (config == NULL || out_handle == NULL || config->i2c_bus == NULL ||
		(!input_enabled && !output_enabled) ||
		(input_enabled && config->rx_handle == NULL) ||
		(output_enabled && config->tx_handle == NULL) ||
		config->sample_rate_hz == 0 || config->bits_per_sample == 0 ||
		config->channel_count == 0) {
		return ESP_ERR_INVALID_ARG;
	}

	/* 先清空输出，确保任何失败路径都不会向调用者返回无效句柄。 */
	*out_handle = NULL;
	drv_es8311_handle_t handle = calloc(1, sizeof(*handle));
	if (handle == NULL) {
		return ESP_ERR_NO_MEM;
	}

	/* I2C 控制接口负责访问 ES8311 默认地址 0x30 的寄存器。 */
	audio_codec_i2c_cfg_t i2c_config = {
		.port = config->i2c_port,
		.addr = ES8311_CODEC_DEFAULT_ADDR,
		.bus_handle = config->i2c_bus,
	};
	handle->ctrl_if = audio_codec_new_i2c_ctrl(&i2c_config);

	/* I2S 数据接口复用调用者配置好的 RX/TX 通道，不重新配置引脚和时钟。 */
	audio_codec_i2s_cfg_t i2s_config = {
		.port = config->i2s_port,
		.rx_handle = config->rx_handle,
		.tx_handle = config->tx_handle,
	};
	handle->data_if = audio_codec_new_i2s_data(&i2s_config);

	/* GPIO 接口用于可选的外部功放使能控制。 */
	const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
	if (handle->ctrl_if == NULL || handle->data_if == NULL || gpio_if == NULL) {
		release_codec(handle);
		return ESP_ERR_NO_MEM;
	}

	/* ES8311 工作在 I2S 从机模式，由 ESP32-S3 提供 BCLK、WS 和可选 MCLK。 */
	es8311_codec_cfg_t codec_config = {
		.ctrl_if = handle->ctrl_if,
		.gpio_if = gpio_if,
		.codec_mode = config->work_mode,
		.master_mode = false,
		.use_mclk = config->use_mclk,
		.pa_pin = config->pa_pin,
		.pa_reverted = config->pa_reverted,
		.hw_gain = {
			.pa_voltage = 5.0,
			.codec_dac_voltage = 3.3,
		},
		.mclk_div = config->mclk_multiple,
	};
	handle->codec_if = es8311_codec_new(&codec_config);
	if (handle->codec_if == NULL) {
		release_codec(handle);
		return ESP_ERR_NO_MEM;
	}

	/* 将 ES8311 功能接口和 I2S 数据接口组合成统一设备句柄。 */
	esp_codec_dev_cfg_t device_config = {
		.dev_type = (input_enabled ? ESP_CODEC_DEV_TYPE_IN : ESP_CODEC_DEV_TYPE_NONE) |
					(output_enabled ? ESP_CODEC_DEV_TYPE_OUT : ESP_CODEC_DEV_TYPE_NONE),
		.codec_if = handle->codec_if,
		.data_if = handle->data_if,
	};
	handle->codec_dev = esp_codec_dev_new(&device_config);
	if (handle->codec_dev == NULL) {
		release_codec(handle);
		return ESP_ERR_NO_MEM;
	}

	/* 打开设备时，组件会按 PCM 格式配置 ES8311 和 I2S 数据接口。 */
	esp_codec_dev_sample_info_t sample_config = {
		.bits_per_sample = config->bits_per_sample,
		.channel = config->channel_count,
		.channel_mask = config->channel_mask,
		.sample_rate = config->sample_rate_hz,
		.mclk_multiple = config->mclk_multiple,
	};
	if (esp_codec_dev_open(handle->codec_dev, &sample_config) != ESP_CODEC_DEV_OK) {
		ESP_LOGE(TAG, "Failed to open ES8311");
		release_codec(handle);
		return ESP_FAIL;
	}
	handle->opened = true;

	/* 只初始化当前工作模式支持的音频方向。 */
	esp_err_t ret = ESP_OK;
	if (output_enabled) {
		ret = Drv_ES8311_Set_Output_Volume(handle, config->output_volume);
	}
	if (ret == ESP_OK && input_enabled) {
		ret = Drv_ES8311_Set_Input_Gain(handle, config->input_gain_db);
	}
	if (ret != ESP_OK) {
		release_codec(handle);
		return ret;
	}

	*out_handle = handle;
	return ESP_OK;
}

esp_err_t Drv_ES8311_Deinit(drv_es8311_handle_t handle)
{
	if (handle == NULL) {
		return ESP_ERR_INVALID_ARG;
	}
	/* 先保存关闭结果，再释放句柄；无论关闭是否成功都不能泄漏资源。 */
	esp_err_t ret = codec_result(esp_codec_dev_close(handle->codec_dev));
	handle->opened = false;
	release_codec(handle);
	return ret;
}

esp_err_t Drv_ES8311_Set_Output_Volume(drv_es8311_handle_t handle, int volume)
{
	if (handle == NULL || volume < 0 || volume > 100) {
		return ESP_ERR_INVALID_ARG;
	}
	return codec_result(esp_codec_dev_set_out_vol(handle->codec_dev, volume));
}

esp_err_t Drv_ES8311_Set_Input_Gain(drv_es8311_handle_t handle, float gain_db)
{
	if (handle == NULL) {
		return ESP_ERR_INVALID_ARG;
	}
	return codec_result(esp_codec_dev_set_in_gain(handle->codec_dev, gain_db));
}

esp_err_t Drv_ES8311_Set_Output_Mute(drv_es8311_handle_t handle, bool mute)
{
	if (handle == NULL) {
		return ESP_ERR_INVALID_ARG;
	}
	return codec_result(esp_codec_dev_set_out_mute(handle->codec_dev, mute));
}

esp_err_t Drv_ES8311_Set_Input_Mute(drv_es8311_handle_t handle, bool mute)
{
	if (handle == NULL) {
		return ESP_ERR_INVALID_ARG;
	}
	return codec_result(esp_codec_dev_set_in_mute(handle->codec_dev, mute));
}

esp_err_t Drv_ES8311_Write(drv_es8311_handle_t handle, const void *data, size_t size)
{
	/* esp_codec_dev 的长度参数为 int，拒绝无法安全转换的超大缓冲区。 */
	if (handle == NULL || data == NULL || size == 0 || size > INT32_MAX) {
		return ESP_ERR_INVALID_ARG;
	}
	return codec_result(esp_codec_dev_write(handle->codec_dev, (void *)data, (int)size));
}

esp_err_t Drv_ES8311_Read(drv_es8311_handle_t handle, void *data, size_t size)
{
	/* esp_codec_dev 的长度参数为 int，拒绝无法安全转换的超大缓冲区。 */
	if (handle == NULL || data == NULL || size == 0 || size > INT32_MAX) {
		return ESP_ERR_INVALID_ARG;
	}
	return codec_result(esp_codec_dev_read(handle->codec_dev, data, (int)size));
}

esp_codec_dev_handle_t Drv_ES8311_Get_Codec_Handle(drv_es8311_handle_t handle)
{
	return handle == NULL ? NULL : handle->codec_dev;
}
