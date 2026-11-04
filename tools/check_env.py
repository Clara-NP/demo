#!/usr/bin/env python3
"""环境自检：工具链 / 配置文件 / 目录结构 的就绪状态。

职责：只读检查，不安装、不修改任何文件；输出三态（OK / MISSING / PENDING）。
执行上下文：离线工具（本地或 CI），任意目录执行。
可重入性：纯只读，可并发运行。
资源所有权：不持有任何持久资源。

用法：
    python3 tools/check_env.py            # 必需项缺失时退出码 1
    python3 tools/check_env.py --strict   # 可选项缺失也算失败
    python3 tools/check_env.py --quiet    # 只输出问题项

退出码：
    0 = 必需项齐全（--strict 时含可选项）
    1 = 有必需项缺失
    2 = 用法/环境错误
"""

from __future__ import annotations

import argparse
import platform
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# (命令, 最低版本或 None, 说明, 是否必需)
TOOLS: list[tuple[str, tuple[int, ...] | None, str, bool]] = [
    ("gcc", None, "host 编译（冒烟构建 / 单测）", True),
    ("cmake", (3, 16), "构建（docs/templates/CMakeLists.txt）", True),
    ("python3", (3, 8), "工具脚本（check_layers / check_env）", True),
    ("git", None, "版本管理、pre-commit 钩子", True),
    ("clang-format", None, "代码格式检查（缺省时该检查项跳过）", False),
    ("ninja", None, "快速构建后端（缺省时回退 make）", False),
    ("doxygen", None, "文档生成与未注释告警（可选）", False),
    ("arm-none-eabi-gcc", None, "目标板交叉编译（缺省时只能 host 构建）", False),
]

# (相对路径, 状态, 说明)  status: required | pending
PATHS: list[tuple[str, str, str]] = [
    (".clang-format", "required", "格式约定"),
    (".github/instructions/c-embedded.instructions.md", "required", "C 语言细则（AI 自动加载）"),
    ("AGENTS.md", "required", "环境级硬性约束"),
    ("tools/bootstrap.sh", "required", "环境就绪入口（自检 + 可选安装）"),
    ("tools/check_env.py", "required", "环境自检（本脚本）"),
    ("tools/check_layers.py", "required", "分层守护"),
    ("docs/templates/CMakeLists.txt", "required", "唯一构建入口"),
    ("docs/templates/framework_demo/README.md", "required", "框架脚手架说明"),
    ("docs/templates/project_demo/project.yaml", "required", "工程骨架"),
    ("docs/templates/design.md", "required", "方案 frontmatter 模板"),
    ("framework/common/generic.h", "required", "共享框架唯一源码与 fw_err_t 契约"),
    ("framework/README.md", "required", "共享框架说明"),
    ("env/presets/base.json", "pending", "host preset（未建，不阻塞）"),
    ("env/toolchains", "pending", "交叉编译工具链文件（未建，不阻塞）"),
    (".githooks/commit-msg", "pending", "提交信息校验（未建，不阻塞）"),
    (".githooks/pre-commit", "pending", "禁改区拦截（未建，不阻塞）"),
    ("docs/standards", "pending", "规范细则（未建，不阻塞）"),
]

INSTALL_HINTS: dict[str, dict[str, str]] = {
    "debian": {
        "clang-format": "sudo apt install -y clang-format",
        "ninja": "sudo apt install -y ninja-build",
        "doxygen": "sudo apt install -y doxygen",
        "arm-none-eabi-gcc": "sudo apt install -y gcc-arm-none-eabi",
        "gcc": "sudo apt install -y build-essential",
        "cmake": "sudo apt install -y cmake",
        "python3": "sudo apt install -y python3",
        "git": "sudo apt install -y git",
    },
    "fedora": {
        "clang-format": "sudo dnf install -y clang-tools-extra",
        "ninja": "sudo dnf install -y ninja-build",
        "doxygen": "sudo dnf install -y doxygen",
        "arm-none-eabi-gcc": "sudo dnf install -y arm-none-eabi-gcc-cs",
        "gcc": "sudo dnf install -y gcc",
        "cmake": "sudo dnf install -y cmake",
        "python3": "sudo dnf install -y python3",
        "git": "sudo dnf install -y git",
    },
    "arch": {
        "clang-format": "sudo pacman -S --needed clang",
        "ninja": "sudo pacman -S --needed ninja",
        "doxygen": "sudo pacman -S --needed doxygen",
        "arm-none-eabi-gcc": "sudo pacman -S --needed arm-none-eabi-gcc",
        "gcc": "sudo pacman -S --needed base-devel",
        "cmake": "sudo pacman -S --needed cmake",
        "python3": "sudo pacman -S --needed python",
        "git": "sudo pacman -S --needed git",
    },
    "darwin": {
        "clang-format": "brew install llvm   # 或用 xcrun clang-format",
        "ninja": "brew install ninja",
        "doxygen": "brew install doxygen",
        "arm-none-eabi-gcc": "brew install --cask gcc-arm-embedded",
        "gcc": "xcode-select --install",
        "cmake": "brew install cmake",
        "python3": "brew install python",
        "git": "brew install git",
    },
}


def distro_key() -> str:
    """返回安装提示使用的平台键。"""
    if platform.system() == "Darwin":
        return "darwin"

    try:
        text = Path("/etc/os-release").read_text(encoding="utf-8", errors="replace")
    except OSError:
        return "debian"

    m = re.search(r"^ID=(\w+)", text, re.MULTILINE)
    if m is None:
        return "debian"

    oid = m.group(1)
    if oid in ("ubuntu", "debian", "linuxmint", "pop"):
        return "debian"
    if oid in ("fedora", "rhel", "centos", "rocky", "almalinux"):
        return "fedora"
    if oid in ("arch", "manjaro", "endeavouros"):
        return "arch"
    return "debian"


def install_hint(tool: str) -> str:
    table = INSTALL_HINTS.get(distro_key(), INSTALL_HINTS["debian"])
    return table.get(tool, "")


def version_of(tool: str) -> str:
    """取版本号；失败返回空串。"""
    try:
        out = subprocess.run(
            [tool, "--version"],
            capture_output=True, text=True, timeout=10, check=False,
        ).stdout
    except (OSError, subprocess.SubprocessError):
        return ""

    first = out.splitlines()[0] if out else ""
    m = re.search(r"(\d+\.\d+(?:\.\d+)?)", first)
    return m.group(1) if m else ""


def version_tuple(text: str) -> tuple[int, ...]:
    parts: list[int] = []
    for item in text.split("."):
        if not item.isdigit():
            break
        parts.append(int(item))
    return tuple(parts)


def check_tools() -> tuple[list[tuple[str, str, str]], int]:
    """返回 (行, 必需项缺失数)。行 = (状态, 名称, 说明)。"""
    rows: list[tuple[str, str, str]] = []
    missing_required = 0

    for tool, min_ver, note, required in TOOLS:
        path = shutil.which(tool)
        if path is None:
            rows.append(("MISSING" if required else "OPTIONAL", tool, note))
            if required:
                missing_required += 1
            continue

        ver = version_of(tool)
        if (min_ver is not None) and ver:
            if version_tuple(ver) < min_ver:
                rows.append(("MISSING", tool, f"{ver} < 需要 {'.'.join(map(str, min_ver))}"))
                if required:
                    missing_required += 1
                continue

        rows.append(("OK", tool, ver))

    return rows, missing_required


def check_paths() -> tuple[list[tuple[str, str, str]], int]:
    rows: list[tuple[str, str, str]] = []
    missing_required = 0

    for rel, status, note in PATHS:
        exists = (ROOT / rel).exists()
        if exists:
            rows.append(("OK", rel, note))
        elif status == "required":
            rows.append(("MISSING", rel, note))
            missing_required += 1
        else:
            rows.append(("PENDING", rel, note))

    return rows, missing_required


MARKERS = {"OK": "✓", "MISSING": "✗", "OPTIONAL": "⚠", "PENDING": "⏳"}


def print_rows(title: str, rows: list[tuple[str, str, str]], quiet: bool) -> None:
    shown = [r for r in rows if (not quiet) or (r[0] in ("MISSING", "OPTIONAL"))]
    if not shown:
        return

    print(f"\n=== {title} ===")
    for status, name, note in shown:
        print(f"{MARKERS[status]} {name:<46} {note}")


def main() -> int:
    parser = argparse.ArgumentParser(description="环境自检（只读）")
    parser.add_argument("--strict", action="store_true", help="可选项缺失也算失败")
    parser.add_argument("--quiet", action="store_true", help="只输出问题项")
    args = parser.parse_args()

    if not (ROOT / "AGENTS.md").is_file():
        print(f"! 仓库根不正确（未找到 AGENTS.md）: {ROOT}", file=sys.stderr)
        return 2

    tool_rows, tool_missing = check_tools()
    path_rows, path_missing = check_paths()

    print(f"仓库根: {ROOT}")
    print_rows("工具链", tool_rows, args.quiet)
    print_rows("配置与结构", path_rows, args.quiet)

    optional_missing = sum(1 for status, _, _ in tool_rows if status == "OPTIONAL")
    pending = sum(1 for status, _, _ in path_rows if status == "PENDING")

    print("\n=== 汇总 ===")
    print(f"必需工具缺失: {tool_missing}    必需文件缺失: {path_missing}")
    print(f"可选工具缺失: {optional_missing}    待建（不阻塞）: {pending}")

    if tool_missing or path_missing:
        print("\n必需项缺失，安装/补齐后可重跑本脚本：")
        for status, name, _ in tool_rows:
            if status == "MISSING":
                hint = install_hint(name)
                print(f"  - {name}: {hint or '需手动安装'}")
        return 1

    if args.strict and optional_missing:
        print("\n--strict：可选项缺失视为失败：")
        for status, name, _ in tool_rows:
            if status == "OPTIONAL":
                hint = install_hint(name)
                print(f"  - {name}: {hint or '需手动安装'}")
        return 1

    print("\n✓ 环境自检通过")
    return 0


if __name__ == "__main__":
    sys.exit(main())
