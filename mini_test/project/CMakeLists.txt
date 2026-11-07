# mini_test：自包含的 STM32F446 闪灯。不引用本目录以外的源码或头文件。

cmake_minimum_required(VERSION 3.16)

# 工程名。elf、bin、map 都使用这个名字。
set(PROJECT_NAME mini_test)

# 编译工具链。必须位于 project() 之前，CMake 才会使用这里的交叉编译器。
set(ARM_TOOLCHAIN_ROOT "/usr" CACHE PATH "arm-none-eabi 工具链根目录，其 bin/ 下有 gcc 与 objcopy")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(OBJCOPY_CMD "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-objcopy")
set(SIZE_CMD "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-size")

project(${PROJECT_NAME} C ASM)

# C 源文件。
set(C_SOURCES
    main.c
)

# 头文件目录。当前没有工程头文件；增加目录时追加到此列表。
set(INC_DIRS
)

# 汇编源文件（.s）。
set(ASM_SOURCES
    ../framework/startup.s
)

# 链接脚本（.ld）。
set(LD_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446XX_FLASH.ld)

# 编译宏（-D）。当前没有；需要时写成 NAME 或 NAME=value。
set(COMPILE_DEFS
)

# 编译参数。CPU_OPTS 同时交给链接，保证编译和链接使用同一颗核。
set(CPU_OPTS
    -mcpu=cortex-m4
    -mthumb
)
set(WARN_OPTS
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wshadow
    -Wundef
)
set(COMPILE_OPTS
    ${CPU_OPTS}
    ${WARN_OPTS}
    -ffunction-sections
    -fdata-sections
    -ffreestanding
)

# 链接参数。
set(LINK_OPTS
    ${CPU_OPTS}
    -T${LD_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${PROJECT_NAME}.map
)

# 生成目标。这里只引用上面的列表。
add_executable(${PROJECT_NAME}
    ${C_SOURCES}
    ${ASM_SOURCES}
)

# 空列表不能传给 target_*，否则 CMake 会因缺少参数报错。
if(INC_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIRS})
endif()

if(COMPILE_DEFS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFS})
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTS})

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_link_options(${PROJECT_NAME} PRIVATE ${LINK_OPTS})

# 构建后：由 elf 生成 bin，并打印各段占用。
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${OBJCOPY_CMD} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${SIZE_CMD} $<TARGET_FILE:${PROJECT_NAME}>
    VERBATIM
)
