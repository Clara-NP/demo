#!/bin/sh
# @file build.sh
# @brief mini_test 交叉构建入口。芯片端口与默认链接脚本在 ../framework。
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

# 从 project.yaml 取出一个顶层或唯一的 key。注释行不参与匹配。
yaml_get() {
    awk -v key="$1" '
        /^[ \t]*#/ { next }
        $0 ~ "^[ \t]*" key ":[ \t]*" {
            sub(/^[ \t]*[^:]+:[ \t]*/, "")
            sub(/[ \t]*#.*/, "")
            gsub(/[ \t]+$/, "")
            print
            exit
        }
    ' "$2"
}

require_yaml() {
    if [ -z "$2" ]; then
        echo "error: project.yaml 缺少 $1" >&2
        exit 1
    fi
}

YAML="$ROOT_DIR/project.yaml"
PROJECT_ID=$(yaml_get id "$YAML")
SOC=$(yaml_get soc "$YAML")
TOOLCHAIN=$(yaml_get toolchain "$YAML")
LINKER_SCRIPT=$(yaml_get linker_script "$YAML")
FW_PATH=$(yaml_get path "$YAML")
require_yaml id "$PROJECT_ID"
require_yaml soc "$SOC"
require_yaml toolchain "$TOOLCHAIN"
require_yaml linker_script "$LINKER_SCRIPT"
require_yaml path "$FW_PATH"

MINI_ROOT=$(cd "$ROOT_DIR/.." && pwd)
REPO_ROOT=$(cd "$MINI_ROOT/.." && pwd)
FRAMEWORK_DIR="$REPO_ROOT/$FW_PATH"
SOC_LOWER=$(printf '%s' "$SOC" | tr '[:upper:]' '[:lower:]')
SOC_UPPER=$(printf '%s' "$SOC" | tr '[:lower:]' '[:upper:]')

case "$SOC_LOWER" in
    stm32f4*)
        PLATFORM_DIR="$FRAMEWORK_DIR/platform/stm32/stm32f4xx"
        # CMSIS 设备宏：stm32f446 → STM32F446xx。F1、GD 以后在这里加自己的目录。
        SOC_DEVICE="${SOC_UPPER}xx"
        ;;
    *)
        echo "error: 没有与 $SOC_LOWER 对应的平台目录" >&2
        exit 1
        ;;
esac

STARTUP_FILE="$FRAMEWORK_DIR/platform/startup/startup_${SOC_LOWER}xx.s"
if [ "$LINKER_SCRIPT" = "null" ]; then
    LD_SCRIPT="$FRAMEWORK_DIR/platform/ld/${SOC_UPPER}XX_FLASH.ld"
else
    LD_SCRIPT="$ROOT_DIR/$LINKER_SCRIPT"
fi

for required in \
    "$PLATFORM_DIR/platform.cmake" \
    "$STARTUP_FILE" \
    "$LD_SCRIPT"
do
    if [ ! -f "$required" ]; then
        echo "error: 找不到 $required" >&2
        exit 1
    fi
done

echo "soc: $SOC_LOWER"
echo "platform: $PLATFORM_DIR"
echo "linker script: $LD_SCRIPT"

if [ "$TOOLCHAIN" = "null" ]; then
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
          -DCMAKE_BUILD_TYPE=Debug \
          -DARM_TOOLCHAIN_ROOT="$ARM_TOOLCHAIN_ROOT" \
          -DPROJECT_ID="$PROJECT_ID" \
          -DPLATFORM_DIR="$PLATFORM_DIR" \
          -DSTARTUP_FILE="$STARTUP_FILE" \
          -DLD_SCRIPT="$LD_SCRIPT" \
          -DSOC_DEVICE="$SOC_DEVICE"
else
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
          -DCMAKE_BUILD_TYPE=Debug \
          -DARM_TOOLCHAIN_ROOT="$ARM_TOOLCHAIN_ROOT" \
          -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/$TOOLCHAIN" \
          -DPROJECT_ID="$PROJECT_ID" \
          -DPLATFORM_DIR="$PLATFORM_DIR" \
          -DSTARTUP_FILE="$STARTUP_FILE" \
          -DLD_SCRIPT="$LD_SCRIPT" \
          -DSOC_DEVICE="$SOC_DEVICE"
fi
cmake --build "$BUILD_DIR" --parallel

echo "artifacts:"
ls -l "$BUILD_DIR"/*.elf "$BUILD_DIR"/*.bin
