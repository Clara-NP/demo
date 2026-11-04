#!/bin/sh
# @file build.sh
# @brief project_demo 的 host 构建与单测入口（复制到 projects/<id>/ 后才可用）
#
# @note 模板自身由根目录 CMakeLists 的 smoke 目标编译，不靠本脚本。
#   本脚本供**复制后的工程**使用；缺 CMakeLists 时快速失败，不给虚假的成功信号。
#   前置条件：
#     1) ../../../framework/ 是共享框架唯一源码
#     2) 本工程 CMakeLists.txt（复制后补，列出真实源文件，不靠 glob）
#
# 用法：./build.sh [clean]

set -eu

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="$ROOT_DIR/build"

if [ "${1:-}" = "clean" ]; then
    rm -rf "$BUILD_DIR"
    echo "cleaned: $BUILD_DIR"
    exit 0
fi

echo "=============================================================="
echo " project_demo: 复制后的工程构建入口（本目录是骨架，自身由根 smoke 目标编译）"
echo "=============================================================="
echo " 缺少的前置条件："
echo "   - ./CMakeLists.txt（复制后补，列出真实源文件）"
echo ""
echo " 就绪后本脚本将执行："
echo "   cmake -S \"$ROOT_DIR\" -B \"$BUILD_DIR\" -DCMAKE_BUILD_TYPE=Debug"
echo "   cmake --build \"$BUILD_DIR\""
echo "   ctest --test-dir \"$BUILD_DIR\" --output-on-failure"
echo "=============================================================="
exit 1
