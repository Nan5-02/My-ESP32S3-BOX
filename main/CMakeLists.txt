# 收集所有源文件
file(GLOB_RECURSE SOURCES
    "*.c"
)

# 所有头文件目录
set(INCLUDE_DIRS
    "."
    "Drv"
    "App"
    "Sta"
    "Wifi"
    "System"
    "Rtos"
    "common"
    "board"
)

# 注册组件
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    # 网络、存储、ES8311 codec 以及新版 I2C/I2S 驱动依赖。
    REQUIRES esp_wifi esp_event esp_netif nvs_flash esp_hw_support spi_flash
             esp_codec_dev esp_driver_gpio esp_driver_i2c esp_driver_i2s
)