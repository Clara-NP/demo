# =============================================================================
# led_project 板上最小构建（STM32F446 + HAL，arm-none-eabi）
# -----------------------------------------------------------------------------
# 目标：让 led_project 能编过并链接出 .elf / .bin，用于验证工程骨架可用。
#
# 边界：
#   - vendor 源码只从 docs/templates/framework/third/ 引用，不复制、不修改
#   - 启动文件（startup_stm32f446xx.s）与链接脚本（src/ld/*.ld）按原文件引用，不修改
#   - HAL 初始化目前直接写在 main.c：framework/port/stm32f446/ 落地后必须迁出
#
# 用法：./build.sh [clean]
# =============================================================================

cmake_minimum_required(VERSION 3.16)

# ---- 工具链：必须在 project() 之前设置 ----
set(ARM_TOOLCHAIN_ROOT "/home/clara/099-tools/arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi"
    CACHE PATH "arm-none-eabi 工具链根目录")
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER       "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER     "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(led_project C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- 路径 ----
set(REPO_ROOT    ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(THIRD_DIR    ${REPO_ROOT}/docs/templates/framework/third)
set(HAL_DIR      ${THIRD_DIR}/STM32F4xx_HAL)
set(HAL_SRC      ${HAL_DIR}/STM32F4xx_HAL_Driver/Src)
set(HAL_INC      ${HAL_DIR}/STM32F4xx_HAL_Driver/Inc)
set(DEVICE_INC   ${HAL_DIR}/CMSIS/Device/ST/STM32F4xx/Include)
set(CMSIS_INC    ${THIRD_DIR}/CMSIS/5.8.0/CMSIS/Core/Include)

# stm32f4xx_hal_conf.h（HAL 模块开关）所在目录
set(HAL_CONF_DIR ${REPO_ROOT}/docs/templates/framework/source/platform/stm32-hal/stm32f4xx)

# 平台实现（platformInit / SystemClock_Config / sysTickInit）
set(PLATFORM_DIR  ${REPO_ROOT}/docs/templates/framework/source/platform/stm32-hal)
set(PLATFORM_SRCS
    ${PLATFORM_DIR}/platform_private.c
    ${PLATFORM_DIR}/stm32f4xx/system_clock_config.c
    ${PLATFORM_DIR}/stm32f4xx/sys_tick.c
)

set(STARTUP_FILE ${HAL_DIR}/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f446xx.s)
set(LD_SCRIPT    ${CMAKE_CURRENT_SOURCE_DIR}/src/ld/STM32F446XX_FLASH.ld)

# ---- 选项与宏 ----
set(CPU_OPTS  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
set(HW_DEFS   STM32F446xx USE_HAL_DRIVER)
set(WARN_OPTS -Wall -Wextra -Werror -Wconversion -Wshadow -Wundef)
set(SECTION_OPTS -ffunction-sections -fdata-sections)

# -----------------------------------------------------------------------------
# vendor：HAL 最小子集 + CMSIS 系统文件
# 第三方代码不套用本仓库的 -Werror 策略，避免升级 HAL 后被厂商告警卡住
# -----------------------------------------------------------------------------
add_library(hal STATIC
    ${HAL_SRC}/stm32f4xx_hal.c
    ${HAL_SRC}/stm32f4xx_hal_cortex.c
    ${HAL_SRC}/stm32f4xx_hal_rcc.c
    ${HAL_SRC}/stm32f4xx_hal_rcc_ex.c
    ${HAL_SRC}/stm32f4xx_hal_gpio.c
    ${HAL_DIR}/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
)

target_include_directories(hal PUBLIC
    ${HAL_INC}
    ${DEVICE_INC}
    ${CMSIS_INC}
    ${HAL_CONF_DIR}
)

target_compile_definitions(hal PUBLIC ${HW_DEFS})
target_compile_options(hal PRIVATE ${CPU_OPTS} ${SECTION_OPTS})

# -----------------------------------------------------------------------------
# 应用
# -----------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    main.c
    ${STARTUP_FILE}
    ${PLATFORM_SRCS}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PLATFORM_DIR})

# platform_private.c 自身不 include HAL 头，由构建期强制注入，避免改动该文件
set_source_files_properties(${PLATFORM_DIR}/platform_private.c
    PROPERTIES COMPILE_OPTIONS "-include;stm32f4xx_hal.h")

target_compile_definitions(${PROJECT_NAME} PRIVATE ${HW_DEFS} CONFIG_STM32_HAL_ENABLE=1)
target_compile_options(${PROJECT_NAME} PRIVATE ${CPU_OPTS} ${WARN_OPTS} ${SECTION_OPTS})
target_link_libraries(${PROJECT_NAME} hal)

# 链接脚本为非 RWX 设计但 PROJECT 段合并后会被 ld 判为 RWX，显式关掉该提示
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_link_options(${PROJECT_NAME} PRIVATE
    ${CPU_OPTS}
    -T${LD_SCRIPT}
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,--no-warn-rwx-segments
    -Wl,-Map=${PROJECT_NAME}.map
    -specs=nano.specs
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-objcopy
            -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
    VERBATIM
)
