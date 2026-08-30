#include "driver/i2c_master.h"
#include "esp_log.h"

#include "i2c_bus.h"

static const char *TAG = "i2c_bus";

static i2c_master_bus_handle_t s_i2c_bus;

i2c_master_bus_handle_t *Board_Get_I2C_Bus(void)
{
    return &s_i2c_bus;
}

void Board_I2C_Bus_Init(void)
{
    if (s_i2c_bus == NULL || s_i2c_bus == 0) {
        i2c_master_bus_config_t config = {
            .i2c_port = CONFIG_APP_I2C_PORT,
            .sda_io_num = CONFIG_APP_I2C_SDA_PIN,
            .scl_io_num = CONFIG_APP_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };
        esp_err_t ret = i2c_new_master_bus(&config, &s_i2c_bus);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(ret));
        }
    }
}
