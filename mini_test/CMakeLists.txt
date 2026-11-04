# mini_test：自包含的 STM32F446 闪灯。不引用本目录以外的源码或头文件。

cmake_minimum_required(VERSION 3.16)

set(ARM_TOOLCHAIN_ROOT "/usr" CACHE PATH "arm-none-eabi 工具链根目录，其 bin/ 下有 gcc 与 objcopy")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(mini_test C ASM)

set(CPU_OPTS -mcpu=cortex-m4 -mthumb)
set(WARN_OPTS -Wall -Wextra -Werror -Wconversion -Wshadow -Wundef)
set(LD_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446XX_FLASH.ld)

add_executable(mini_test main.c startup.s)

target_compile_options(mini_test PRIVATE
    ${CPU_OPTS}
    ${WARN_OPTS}
    -ffunction-sections
    -fdata-sections
    -ffreestanding
)

set_target_properties(mini_test PROPERTIES SUFFIX ".elf")

target_link_options(mini_test PRIVATE
    ${CPU_OPTS}
    -T${LD_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=mini_test.map
)

add_custom_command(TARGET mini_test POST_BUILD
    COMMAND ${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-objcopy
            -O binary $<TARGET_FILE:mini_test> mini_test.bin
    COMMAND ${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-size $<TARGET_FILE:mini_test>
    VERBATIM
)
