#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "esp_codec_dev.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief ES8311 驱动的不透明句柄，内部资源只能通过本驱动 API 管理。 */
typedef struct drv_es8311 *drv_es8311_handle_t;

/** @brief ES8311 初始化配置。I2C 总线和 I2S 通道均由调用者创建并持有。 */
typedef struct {
	i2c_master_bus_handle_t i2c_bus;       /**< 已初始化的 I2C master bus 句柄。 */
	i2s_chan_handle_t tx_handle;           /**< 已初始化并启用的 I2S TX 通道；仅录音时可为 NULL。 */
	i2s_chan_handle_t rx_handle;           /**< 已初始化并启用的 I2S RX 通道；仅播放时可为 NULL。 */
	int i2c_port;                           /**< I2C 控制器编号。 */
	int i2s_port;                           /**< I2S 控制器编号。 */
	int pa_pin;                             /**< 功放使能 GPIO；不使用时设为 -1。 */
	bool pa_reverted;                       /**< true 表示功放使能电平为低电平。 */
	bool use_mclk;                          /**< 是否向 ES8311 提供外部 MCLK。 */
	uint32_t sample_rate_hz;                /**< PCM 采样率，单位 Hz。 */
	uint32_t mclk_multiple;                 /**< MCLK 与采样率的倍数，常用值为 256 或 384。 */
	uint8_t bits_per_sample;                /**< 每个采样点的有效位数。 */
	uint8_t channel_count;                  /**< PCM 通道数。 */
	uint32_t channel_mask;                  /**< 有效声道位掩码，例如双声道为 0x03。 */
	int output_volume;                      /**< 初始输出音量，范围 0 到 100。 */
	float input_gain_db;                    /**< 初始麦克风输入增益，单位 dB。 */
	esp_codec_dec_work_mode_t work_mode;    /**< ADC、DAC 或 ADC/DAC 双向工作模式。 */
} drv_es8311_config_t;

/**
 * @brief 生成 ES8311 默认配置。
 *
 * 默认使用 16 kHz、16-bit、双声道、256 倍 MCLK、双向收发模式。调用者可在
 * 初始化前覆盖结构体字段。传入的总线和通道不会在反初始化时被本驱动删除。
 */
#define DRV_ES8311_CONFIG_DEFAULT(i2c_bus_handle, i2s_tx_handle, i2s_rx_handle) \
	{                                                                            \
		.i2c_bus = (i2c_bus_handle),                                              \
		.tx_handle = (i2s_tx_handle),                                             \
		.rx_handle = (i2s_rx_handle),                                             \
		.i2c_port = 0,                                                            \
		.i2s_port = 0,                                                            \
		.pa_pin = -1,                                                             \
		.pa_reverted = false,                                                     \
		.use_mclk = true,                                                         \
		.sample_rate_hz = 16000,                                                  \
		.mclk_multiple = 256,                                                     \
		.bits_per_sample = 16,                                                    \
		.channel_count = 2,                                                       \
		.channel_mask = 0x03,                                                     \
		.output_volume = 70,                                                      \
		.input_gain_db = 24.0f,                                                   \
		.work_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,                                \
	}

/**
 * @brief 创建并打开 ES8311 codec 设备。
 * @param config 初始化配置。
 * @param out_handle 成功时返回驱动句柄。
 * @return ESP_OK 成功；ESP_ERR_INVALID_ARG 参数错误；ESP_ERR_NO_MEM 内存不足；
 *         ESP_FAIL codec 创建、打开或初始参数设置失败。
 */
esp_err_t Drv_ES8311_Init(const drv_es8311_config_t *config, drv_es8311_handle_t *out_handle);

/**
 * @brief 关闭 ES8311 并释放本驱动创建的 codec 接口资源。
 * @note 不会删除调用者传入的 I2C 总线和 I2S 通道。
 */
esp_err_t Drv_ES8311_Deinit(drv_es8311_handle_t handle);

/** @brief 设置输出音量，volume 的有效范围为 0 到 100。 */
esp_err_t Drv_ES8311_Set_Output_Volume(drv_es8311_handle_t handle, int volume);

/** @brief 设置麦克风输入增益，单位为 dB。 */
esp_err_t Drv_ES8311_Set_Input_Gain(drv_es8311_handle_t handle, float gain_db);

/** @brief 设置或取消 DAC 输出静音。 */
esp_err_t Drv_ES8311_Set_Output_Mute(drv_es8311_handle_t handle, bool mute);

/** @brief 设置或取消 ADC 输入静音。 */
esp_err_t Drv_ES8311_Set_Input_Mute(drv_es8311_handle_t handle, bool mute);

/**
 * @brief 通过 codec 数据接口写入 PCM 数据。
 * @note esp_codec_dev 可能在原缓冲区执行软件音量处理，因此缓冲区必须可写。
 */
esp_err_t Drv_ES8311_Write(drv_es8311_handle_t handle, const void *data, size_t size);

/** @brief 通过 codec 数据接口读取 PCM 数据。 */
esp_err_t Drv_ES8311_Read(drv_es8311_handle_t handle, void *data, size_t size);

/** @brief 获取底层 esp_codec_dev 句柄，供组件高级 API 使用；驱动仍保留所有权。 */
esp_codec_dev_handle_t Drv_ES8311_Get_Codec_Handle(drv_es8311_handle_t handle);

#ifdef __cplusplus
}
#endif
