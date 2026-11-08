# 驱动源文件。新增驱动时在这里追加，工程不再逐个列举。

set(DRIVER_C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/gpio/gpio.c
)

set(DRIVER_INC_DIRS
    ${CMAKE_CURRENT_LIST_DIR}
)
