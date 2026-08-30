#ifndef __I2C_BUS_H__
#define __I2C_BUS_H__
#include "driver/i2c_master.h"

#define CONFIG_APP_I2C_PORT          I2C_NUM_0
#define CONFIG_APP_I2C_SDA_PIN       GPIO_NUM_48
#define CONFIG_APP_I2C_SCL_PIN       GPIO_NUM_45

i2c_master_bus_handle_t *Board_Get_I2C_Bus(void);
void Board_I2C_Bus_Init(void);

#endif /* __I2C_BUS_H__ */