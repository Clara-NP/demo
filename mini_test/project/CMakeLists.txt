# mini_test：芯片与路径由 build.sh 从 project.yaml 解析后传入。

cmake_minimum_required(VERSION 3.16)

if(NOT PROJECT_ID OR NOT PLATFORM_DIR OR NOT STARTUP_FILE OR NOT LD_SCRIPT)
    message(FATAL_ERROR "缺少 PROJECT_ID、PLATFORM_DIR、STARTUP_FILE 或 LD_SCRIPT，请用 ./build.sh 构建")
endif()

# 编译工具链。必须位于 project() 之前。
# toolchain 不是 null 时，build.sh 传入 CMAKE_TOOLCHAIN_FILE，这里不再设置编译器。
set(ARM_TOOLCHAIN_ROOT "/usr" CACHE PATH "arm-none-eabi 工具链根目录，其 bin/ 下有 gcc 与 objcopy")
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
    set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()
set(OBJCOPY_CMD "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-objcopy")
set(SIZE_CMD "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-size")

project(${PROJECT_ID} C ASM)

include("${PLATFORM_DIR}/platform.cmake")

set(ASM_SOURCES "${STARTUP_FILE}")

# C 源文件。平台自己的源文件由 platform.cmake 给出。
set(C_SOURCES
    main.c
    ${PLATFORM_C_SOURCES}
)

# 头文件目录。
set(INC_DIRS
    ${PLATFORM_INC_DIRS}
)

if(NOT EXISTS "${LD_SCRIPT}")
    message(FATAL_ERROR "链接脚本不存在: ${LD_SCRIPT}")
endif()

# 编译宏（-D）。当前没有；需要时写成 NAME 或 NAME=value。
set(COMPILE_DEFS
)

# 编译参数。CPU 参数来自平台，编译和链接使用同一组。
set(WARN_OPTS
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wshadow
    -Wundef
)
set(COMPILE_OPTS
    ${PLATFORM_CPU_OPTS}
    ${WARN_OPTS}
    -ffunction-sections
    -fdata-sections
    -ffreestanding
)

# 链接参数。
set(LINK_OPTS
    ${PLATFORM_CPU_OPTS}
    -T${LD_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${PROJECT_NAME}.map
)

message(STATUS "linker script: ${LD_SCRIPT}")

# 生成目标。这里只引用上面的列表。
add_executable(${PROJECT_NAME}
    ${C_SOURCES}
    ${ASM_SOURCES}
)

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
