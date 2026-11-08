# STM32F4 系列共用这一份。具体芯片仍由 soc 选择 startup 与 ld。

set(PLATFORM_CPU_OPTS
    -mcpu=cortex-m4
    -mthumb
)

set(PLATFORM_INC_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)

# 供 CMSIS 启动文件调用的 SystemInit。工程不点名这个文件。
set(PLATFORM_C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/system_init.c
)
