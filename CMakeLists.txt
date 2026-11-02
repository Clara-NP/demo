# =============================================================================
# host 冒烟构建（smoke）：只为证明"模板代码能过编译器"，不代表产品构建
# -----------------------------------------------------------------------------
#   smoke_app  ：框架 + 模块 + 工程 全链路编译链接（不运行，含死循环 main）
#   smoke_test ：模块纯逻辑单测（自带假总线，不链接 port/host）
#
# 用法：
#   cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
# =============================================================================

cmake_minimum_required(VERSION 3.16)
project(mcu_host_smoke C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

set(FW_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/docs/templates/framework_demo)
set(MOD_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/libs/drivers/devices/tmpl_sensor)
set(PRJ_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/docs/templates/project_demo)
set(PORT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/port/host)

set(HOST_ONLY_DEFS _POSIX_C_SOURCE=199309L)

set(WARN_OPTS -Wall -Wextra -Werror -Wconversion -Wshadow -Wundef)

# -----------------------------------------------------------------------------
# 全链路：框架 + 模块 + 工程侧
# -----------------------------------------------------------------------------
add_executable(smoke_app
    ${FW_DIR}/components/console/command.c
    ${FW_DIR}/components/sacp/attribute/sacp_attribute.c
    ${MOD_DIR}/tmpl_sensor.c
    ${MOD_DIR}/cli_tmpl_sensor.c
    ${PORT_DIR}/os_host.c
    ${PORT_DIR}/i2c_host.c
    ${PRJ_DIR}/src/main.c
    ${PRJ_DIR}/src/system/board.c
    ${PRJ_DIR}/src/sacp/sacp_tmpl_sensor.c
)

target_include_directories(smoke_app PRIVATE
    ${FW_DIR}
    ${FW_DIR}/components
    ${FW_DIR}/drivers/bus
    ${MOD_DIR}
    ${PRJ_DIR}/src
    ${PORT_DIR}
)

target_compile_definitions(smoke_app PRIVATE ${HOST_ONLY_DEFS})
target_compile_options(smoke_app PRIVATE ${WARN_OPTS})

# -----------------------------------------------------------------------------
# 单测：模块纯逻辑 + 假总线（用例自带 mock，不链 port/host）
# -----------------------------------------------------------------------------
add_executable(smoke_test
    ${MOD_DIR}/tmpl_sensor.c
    ${PRJ_DIR}/test/test_tmpl_sensor.c
)

target_include_directories(smoke_test PRIVATE
    ${FW_DIR}
    ${FW_DIR}/drivers/bus
    ${MOD_DIR}
)

target_compile_options(smoke_test PRIVATE ${WARN_OPTS})

add_test(NAME tmpl_sensor.unit COMMAND smoke_test)
