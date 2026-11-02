#!/bin/sh
# @file bootstrap.sh
# @brief 新环境就绪：自检工具链 → 打印/执行安装命令 → 验证可用
#
# 用法：
#   ./tools/bootstrap.sh             # 只自检并打印安装命令（不修改系统）
#   ./tools/bootstrap.sh --install   # 按当前发行版执行安装命令（需 sudo，会先确认）
#   ./tools/bootstrap.sh --strict    # 可选项缺失也算失败
#
# 退出码：0 就绪；1 必需项缺失；2 用法/环境错误
#
# @note 本脚本只做"检查 + 可选安装"，不生成目录、不改仓库文件。

set -eu

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
ROOT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
INSTALL=0
STRICT=""

for arg in "$@"; do
    case "$arg" in
        --install) INSTALL=1 ;;
        --strict)  STRICT="--strict" ;;
        -h|--help)
            sed -n '2,12p' "$0" | sed 's/^# \{0,1\}//'
            exit 0
            ;;
        *)
            echo "未知参数: $arg（见 ./tools/bootstrap.sh --help）" >&2
            exit 2
            ;;
    esac
done

if ! command -v python3 >/dev/null 2>&1; then
    echo "✗ 未找到 python3 —— 先安装它，再重跑本脚本" >&2
    exit 2
fi

# shellcheck disable=SC2086
set +e
python3 "$ROOT_DIR/tools/check_env.py" $STRICT
CHECK_RC=$?
set -e

if [ "$CHECK_RC" -eq 0 ]; then
    exit 0
fi

if [ "$CHECK_RC" -ne 1 ]; then
    exit "$CHECK_RC"
fi

if [ "$INSTALL" -eq 0 ]; then
    echo ""
    echo "提示：确认无误后可用 ./tools/bootstrap.sh --install 执行上述安装命令。"
    exit 1
fi

# ---- 收集并执行安装命令 -----------------------------------------------------
CMDS=$(python3 "$ROOT_DIR/tools/check_env.py" | sed -n 's/^  - [a-zA-Z0-9._-]*: //p' | sort -u)

if [ -z "$CMDS" ]; then
    echo "没有可自动执行的安装命令，请手动处理。" >&2
    exit 1
fi

echo ""
echo "将执行以下命令（需要 sudo 权限）："
echo "$CMDS" | sed 's/^/  /'
printf "继续？[y/N] "
read -r answer
case "$answer" in
    y|Y) ;;
    *) echo "已取消。"; exit 1 ;;
esac

echo "$CMDS" | while IFS= read -r cmd; do
    [ -n "$cmd" ] || continue
    echo "==> $cmd"
    sh -c "$cmd"
done

echo ""
echo "安装完成，重新自检……"
# shellcheck disable=SC2086
exec python3 "$ROOT_DIR/tools/check_env.py" $STRICT
