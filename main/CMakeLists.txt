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
)

# 注册组件
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    REQUIRES esp_wifi esp_event esp_netif nvs_flash esp_hw_support spi_flash
)