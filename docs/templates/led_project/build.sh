#!/bin/sh
# @file build.sh
# @brief led_project 板上构建入口（arm-none-eabi + CMake + HAL）
#
# @note 前置条件：
#   1) arm-none-eabi 工具链可用（默认路径见下方 ARM_TOOLCHAIN_ROOT，可用环境变量覆盖）
#   2) docs/templates/framework/third/ 下存在 HAL 与 CMSIS（只引用，不复制）
#
# @note 用法：./build.sh [clean]

set -eu

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="$ROOT_DIR/build"
ARM_TOOLCHAIN_ROOT=${ARM_TOOLCHAIN_ROOT:-/home/clara/099-tools/arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi}
GCC="$ARM_TOOLCHAIN_ROOT/bin/arm-none-eabi-gcc"

if [ "${1:-}" = "clean" ]; then
    rm -rf "$BUILD_DIR"
    echo "cleaned: $BUILD_DIR"
    exit 0
fi

if [ ! -x "$GCC" ]; then
    echo "error: 找不到交叉编译器 $GCC" >&2
    echo "       用 ARM_TOOLCHAIN_ROOT=<路径> ./build.sh 覆盖默认值" >&2
    exit 1
fi

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DARM_TOOLCHAIN_ROOT="$ARM_TOOLCHAIN_ROOT"
cmake --build "$BUILD_DIR" --parallel

echo "artifacts:"
ls -l "$BUILD_DIR/led_project.elf" "$BUILD_DIR/led_project.bin"
