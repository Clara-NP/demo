#!/usr/bin/env python3
"""架构守护：检查分层 include 是否越界。

职责：扫描 .c/.h 的 #include，按"层规则表"判定违规，输出文件:行号。
执行上下文：离线工具（本地 / CI），不参与运行时。
可重入性：纯只读，无副作用，可并发运行。
资源所有权：不持有任何持久资源。

用法：
    python tools/check_layers.py             # 全量检查
    python tools/check_layers.py --verbose   # 额外打印扫描文件数
    python tools/check_layers.py --list-rules

退出码：
    0 = 通过；1 = 发现违规；2 = 用法/环境错误

豁免：在违规行尾加注释 `// layer-allow: <原因>` 即可放行（须说明原因，评审可见）。
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# 不参与检查的目录（第三方、生成物、模板、构建输出）
SKIP_DIRS = {
    ".git", ".vscode", ".deps", "build", "out", "released", "node_modules",
    "third_party", "generated", "docs",
}

# 芯片厂商 SDK / CMSIS 内核头：仅允许出现在 port/ 与 libs/third_party/
VENDOR_PATTERNS = [
    "stm32*.h", "esp_*.h", "nrf*.h", "gd32*.h", "at32*.h", "hc32*.h",
    "ch32*.h", "sam*.h", "fsl_*.h", "mimxrt*.h", "lpc*.h", "xmc_*.h",
    "cy_*.h", "core_cm*.h", "cmsis_*.h", "cmsis_os*.h", "hal.h",
]

# RTOS 头：仅允许出现在 libs/osal/ 与 port/
RTOS_PATTERNS = [
    "freertos.h", "task.h", "queue.h", "semphr.h", "timers.h",
    "event_groups.h", "portmacro.h", "stream_buffer.h",
]

# SOC 抽象头：仅允许出现在总线驱动、hal、port
SOC_PATTERNS = ["soc_*.h"]

# 规则表：(名称, 作用路径, 禁止的 include 模式)
# 路径使用 glob：* 不跨目录，** 跨目录
RULES = [
    (
        "业务层/组件不得引用芯片厂商 SDK 头",
        ["projects/*/src/**", "libs/components/**", "libs/services/**", "libs/drivers/**"],
        VENDOR_PATTERNS,
    ),
    (
        "业务层/设备驱动/组件不得引用 soc_ 抽象层",
        ["projects/*/src/**", "libs/components/**", "libs/services/**",
         "libs/drivers/devices/**", "libs/drivers/devices/**/*.c"],
        SOC_PATTERNS,
    ),
    (
        "非 osal/port 不得引用 RTOS 头",
        ["**"],
        RTOS_PATTERNS,
    ),
    (
        "禁止相对路径穿越 include（如 ../../libs/...）",
        ["**"],
        ["../**"],
    ),
]

# 上述规则的例外路径（相对仓库根）
ALLOW_PATHS = {
    "非 osal/port 不得引用 RTOS 头": ["libs/osal/**", "port/**"],
    "禁止相对路径穿越 include（如 ../../libs/...）": [],
}
INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]')
ALLOW_RE = re.compile(r"layer-allow\s*:\s*\S+")


def glob_to_regex(pattern: str) -> re.Pattern[str]:
    """把 glob 转成正则：* 不跨目录，** 跨目录。"""
    out = ["^"]
    i = 0
    while i < len(pattern):
        if pattern.startswith("**", i):
            i += 2
            if i < len(pattern) and pattern[i] == "/":
                i += 1
            out.append(".*")
            continue
        ch = pattern[i]
        if ch == "*":
            out.append("[^/]*")
        elif ch == "?":
            out.append("[^/]")
        else:
            out.append(re.escape(ch))
        i += 1
    out.append("$")
    return re.compile("".join(out))


COMPILED_RULES = [
    (name, [glob_to_regex(p) for p in paths],
     [p.lower() for p in forbids])
    for name, paths, forbids in RULES
]

ALLOW_COMPILED = {
    name: [glob_to_regex(p) for p in paths] for name, paths in ALLOW_PATHS.items()
}


def matches_any(rel: str, patterns: list[re.Pattern[str]]) -> bool:
    return any(p.match(rel) for p in patterns)


def include_matches(inc: str, patterns: list[str]) -> bool:
    """include 名按小写整体匹配（如 soc_gpio.h / ../foo.h）。"""
    low = inc.lower().replace("\\", "/")
    for pat in patterns:
        if glob_to_regex(pat).match(low):
            return True
        # 子目录形式：driver/gpio.h 也匹配 gpio.h
        if "/" not in pat and glob_to_regex(pat).match(low.rsplit("/", 1)[-1]):
            return True
    return False


def iter_sources() -> list[Path]:
    files: list[Path] = []
    for path in ROOT.rglob("*"):
        if not path.is_file() or path.suffix not in {".c", ".h"}:
            continue
        if any(part in SKIP_DIRS for part in path.relative_to(ROOT).parts):
            continue
        files.append(path)
    return sorted(files)


def check_file(path: Path) -> list[tuple[str, int, str, str]]:
    rel = path.relative_to(ROOT).as_posix()
    violations: list[tuple[str, int, str, str]] = []
    try:
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    except OSError as exc:
        print(f"! 无法读取 {rel}: {exc}", file=sys.stderr)
        return violations

    for lineno, line in enumerate(lines, start=1):
        m = INCLUDE_RE.match(line)
        if not m:
            continue
        inc = m.group(1)
        for name, paths, forbids in COMPILED_RULES:
            if not matches_any(rel, paths):
                continue
            if matches_any(rel, ALLOW_COMPILED.get(name, [])):
                continue
            if include_matches(inc, forbids):
                if ALLOW_RE.search(line):
                    continue
                violations.append((name, lineno, inc, rel))
    return violations


def main() -> int:
    parser = argparse.ArgumentParser(description="分层 include 架构守护")
    parser.add_argument("--verbose", action="store_true", help="打印扫描文件数")
    parser.add_argument("--list-rules", action="store_true", help="列出规则后退出")
    args = parser.parse_args()

    if args.list_rules:
        for name, paths, forbids in RULES:
            print(f"- {name}\n    作用路径: {', '.join(paths)}\n    禁止: {', '.join(forbids)}")
        return 0

    if not ROOT.is_dir():
        print(f"! 仓库根不存在: {ROOT}", file=sys.stderr)
        return 2

    sources = iter_sources()
    if args.verbose:
        print(f"扫描 {len(sources)} 个 .c/.h 文件（跳过第三方/生成物/模板）")

    by_rule: dict[str, list[tuple[str, int, str]]] = {}
    for path in sources:
        for name, lineno, inc, rel in check_file(path):
            by_rule.setdefault(name, []).append((rel, lineno, inc))

    if not by_rule:
        print(f"✓ 分层检查通过（{len(sources)} 个文件）")
        return 0

    total = 0
    for name, items in by_rule.items():
        print(f"\n✗ {name}（{len(items)} 处）")
        for rel, lineno, inc in items:
            print(f"    {rel}:{lineno}  #include \"{inc}\"")
            total += 1

    print(f"\n共 {total} 处违规。修复方式：改为依赖抽象接口，或移入 port/<soc>/。")
    print("确需保留时，在该行末加 `// layer-allow: <原因>` 并在评审中说明。")
    return 1


if __name__ == "__main__":
    sys.exit(main())
