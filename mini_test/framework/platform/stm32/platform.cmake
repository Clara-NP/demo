# STM32 各系列共用的源文件。系列库由 SOC_DEVICE 选择子目录。

if(NOT SOC_DEVICE)
    message(FATAL_ERROR "缺少 SOC_DEVICE，请用 ./build.sh 构建")
endif()

set(PLATFORM_INC_DIRS
    ${CMAKE_CURRENT_LIST_DIR}
)

set(PLATFORM_C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/platform_private.c
    ${CMAKE_CURRENT_LIST_DIR}/sys_tick.c
)

if(SOC_DEVICE MATCHES "^STM32F4")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/stm32f4xx ${CMAKE_BINARY_DIR}/stm32f4xx)
else()
    message(FATAL_ERROR "没有与 ${SOC_DEVICE} 对应的 STM32 系列目录")
endif()

list(APPEND PLATFORM_C_SOURCES ${PLATFORM_FAMILY_C_SOURCES})
list(APPEND PLATFORM_INC_DIRS ${PLATFORM_FAMILY_INC_DIRS})
