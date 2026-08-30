#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** XL9555 的 8 位写地址和读地址；ESP-IDF 使用下方的 7 位地址。 */
#define DRV_XL9555_I2C_WRITE_ADDRESS_8BIT  0x40
#define DRV_XL9555_I2C_READ_ADDRESS_8BIT   0x41
#define DRV_XL9555_I2C_ADDRESS             (DRV_XL9555_I2C_WRITE_ADDRESS_8BIT >> 1)

/** XL9555 的 INT 输出连接到 ESP32-S3 GPIO3；低电平有效、开漏输出。 */
#define DRV_XL9555_INT_GPIO                GPIO_NUM_3

/** XL9555 端口位数和 I2C 传输超时时间。 */
#define DRV_XL9555_PIN_COUNT               16
#define DRV_XL9555_I2C_TIMEOUT_MS          100

/**
 * @brief 在板级共享 I2C master bus 上添加 XL9555 设备。
 * @note 不会创建或删除 I2C 总线，因此可与 ES8311 共用 IO48/IO45 总线。
 */
esp_err_t Drv_XL9555_Init(void);

/** @brief 移除 XL9555 的 I2C 设备句柄，不影响共用的 I2C 总线。 */
esp_err_t Drv_XL9555_Deinit(void);

/** @brief 设置全部 16 个引脚方向；bit=1 为输入，bit=0 为输出。 */
esp_err_t Drv_XL9555_Set_Direction(uint16_t direction_mask);

/** @brief 设置单个引脚方向；pin 范围为 0 到 15。 */
esp_err_t Drv_XL9555_Set_Pin_Direction(uint8_t pin, bool input);

/** @brief 写入全部输出锁存器；每一位对应一个 XL9555 引脚。 */
esp_err_t Drv_XL9555_Write_Output(uint16_t output_mask);

/** @brief 设置单个输出引脚电平；pin 范围为 0 到 15。 */
esp_err_t Drv_XL9555_Set_Output_Level(uint8_t pin, bool level);

/** @brief 读取全部引脚当前输入电平。 */
esp_err_t Drv_XL9555_Read_Input(uint16_t *input_mask);

/** @brief 读取单个引脚当前输入电平；pin 范围为 0 到 15。 */
esp_err_t Drv_XL9555_Get_Input_Level(uint8_t pin, bool *level);

#ifdef __cplusplus
}
#endif
