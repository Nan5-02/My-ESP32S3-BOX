#include "drv_xl9555.h"

#include <stdlib.h>

#include "board/i2c_bus.h"

/* XL9555 的寄存器地址；每组端口 0/1 连续占用两个字节。 */
#define XL9555_REG_INPUT_PORT_0        0x00
#define XL9555_REG_OUTPUT_PORT_0       0x02
#define XL9555_REG_CONFIGURATION_0     0x06

typedef struct {
	i2c_master_dev_handle_t i2c_device; /**< 由共享 I2C bus 创建的 XL9555 从设备。 */
	uint16_t output_mask;                /**< 缓存输出锁存器，支持单引脚读改写。 */
	uint16_t direction_mask;             /**< 缓存方向寄存器，1 为输入、0 为输出。 */
} drv_xl9555_context_t;

static drv_xl9555_context_t *s_context;

static esp_err_t write_registers(uint8_t reg, uint16_t value)
{
	uint8_t buffer[] = {
		reg,
		(uint8_t)(value & 0xff),
		(uint8_t)(value >> 8),
	};
	return i2c_master_transmit(s_context->i2c_device, buffer, sizeof(buffer),
							   DRV_XL9555_I2C_TIMEOUT_MS);
}

static esp_err_t read_registers(uint8_t reg, uint16_t *value)
{
	uint8_t data[2];
	esp_err_t ret = i2c_master_transmit_receive(s_context->i2c_device, &reg, sizeof(reg), data,
												 sizeof(data), DRV_XL9555_I2C_TIMEOUT_MS);
	if (ret == ESP_OK) {
		*value = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
	}
	return ret;
}

/* XL9555 的 INT 为低电平有效开漏输出，空闲时由 ESP32 内部上拉保持高电平。 */
static esp_err_t init_interrupt_pin(void)
{
	const gpio_config_t config = {
		.pin_bit_mask = UINT64_C(1) << DRV_XL9555_INT_GPIO,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	return gpio_config(&config);
}

esp_err_t Drv_XL9555_Init(void)
{
	if (s_context != NULL) {
		return ESP_OK;
	}
	i2c_master_bus_handle_t i2c_bus = *Board_Get_I2C_Bus();
	if (i2c_bus == NULL) {
		return ESP_ERR_INVALID_STATE;
	}

	drv_xl9555_context_t *context = calloc(1, sizeof(*context));
	if (context == NULL) {
		return ESP_ERR_NO_MEM;
	}

	esp_err_t ret = init_interrupt_pin();
	if (ret != ESP_OK) {
		free(context);
		return ret;
	}

	i2c_device_config_t device_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = DRV_XL9555_I2C_ADDRESS,
		.scl_speed_hz = 100000,
	};
	ret = i2c_master_bus_add_device(i2c_bus, &device_config, &context->i2c_device);
	if (ret != ESP_OK) {
		free(context);
		return ret;
	}

	/* 上电后引脚默认为输入；读回缓存使单引脚操作不覆盖当前硬件状态。 */
	s_context = context;
	ret = read_registers(XL9555_REG_OUTPUT_PORT_0, &context->output_mask);
	if (ret == ESP_OK) {
		ret = read_registers(XL9555_REG_CONFIGURATION_0, &context->direction_mask);
	}
	if (ret != ESP_OK) {
		i2c_master_bus_rm_device(context->i2c_device);
		free(context);
		s_context = NULL;
		return ret;
	}

	return ESP_OK;
}

esp_err_t Drv_XL9555_Deinit(void)
{
	if (s_context == NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	esp_err_t ret = i2c_master_bus_rm_device(s_context->i2c_device);
	free(s_context);
	s_context = NULL;
	return ret;
}

esp_err_t Drv_XL9555_Set_Direction(uint16_t direction_mask)
{
	if (s_context == NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	esp_err_t ret = write_registers(XL9555_REG_CONFIGURATION_0, direction_mask);
	if (ret == ESP_OK) {
		s_context->direction_mask = direction_mask;
	}
	return ret;
}

esp_err_t Drv_XL9555_Set_Pin_Direction(uint8_t pin, bool input)
{
	if (s_context == NULL || pin >= DRV_XL9555_PIN_COUNT) {
		return ESP_ERR_INVALID_ARG;
	}
	uint16_t direction_mask = input ? s_context->direction_mask | (UINT16_C(1) << pin) :
									  s_context->direction_mask & ~(UINT16_C(1) << pin);
	return Drv_XL9555_Set_Direction(direction_mask);
}

esp_err_t Drv_XL9555_Write_Output(uint16_t output_mask)
{
	if (s_context == NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	esp_err_t ret = write_registers(XL9555_REG_OUTPUT_PORT_0, output_mask);
	if (ret == ESP_OK) {
		s_context->output_mask = output_mask;
	}
	return ret;
}

esp_err_t Drv_XL9555_Set_Output_Level(uint8_t pin, bool level)
{
	if (s_context == NULL || pin >= DRV_XL9555_PIN_COUNT) {
		return ESP_ERR_INVALID_ARG;
	}
	uint16_t output_mask = level ? s_context->output_mask | (UINT16_C(1) << pin) :
								   s_context->output_mask & ~(UINT16_C(1) << pin);
	return Drv_XL9555_Write_Output(output_mask);
}

esp_err_t Drv_XL9555_Read_Input(uint16_t *input_mask)
{
	if (s_context == NULL || input_mask == NULL) {
		return ESP_ERR_INVALID_ARG;
	}
	return read_registers(XL9555_REG_INPUT_PORT_0, input_mask);
}

esp_err_t Drv_XL9555_Get_Input_Level(uint8_t pin, bool *level)
{
	if (s_context == NULL || level == NULL || pin >= DRV_XL9555_PIN_COUNT) {
		return ESP_ERR_INVALID_ARG;
	}
	uint16_t input_mask;
	esp_err_t ret = Drv_XL9555_Read_Input(&input_mask);
	if (ret == ESP_OK) {
		*level = (input_mask & (UINT16_C(1) << pin)) != 0;
	}
	return ret;
}
