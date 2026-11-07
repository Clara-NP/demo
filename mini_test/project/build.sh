#!/bin/sh
# @file build.sh
# @brief mini_test 交叉构建入口。源码、启动文件、链接脚本都在本目录。
#
# @note 用法：./build.sh [clean]
#       工具链可用 ARM_TOOLCHAIN_ROOT 覆盖；未设置时用 PATH 里的 arm-none-eabi-gcc。

set -eu

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="$ROOT_DIR/build"

if [ -z "${ARM_TOOLCHAIN_ROOT:-}" ]; then
    GCC_PATH=$(command -v arm-none-eabi-gcc || true)
    if [ -n "$GCC_PATH" ]; then
        ARM_TOOLCHAIN_ROOT=$(cd "$(dirname "$GCC_PATH")/.." && pwd)
    else
        ARM_TOOLCHAIN_ROOT=/home/clara/099-tools/arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi
    fi
fi

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
ls -l "$BUILD_DIR"/*.elf "$BUILD_DIR"/*.bin
