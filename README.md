# 900-test —— 嵌入式 C 多工程工作区

> 环境层（可复用骨架）+ 实例层（`projects/`，可增删），依赖方向单向：工程可引用环境层，环境层禁止引用具体工程。
> 平台总体方案见 `docs/design/platform-plan.md`；硬性约束见 `AGENTS.md`。

---

## 1. 目录职责

| 路径 | 是什么 | 谁维护 |
|---|---|---|
| `AGENTS.md` | 环境级硬性约束（常驻上下文） | 平台 |
| `.github/skills/` | 按需流程（`add-module` 等） | 平台 |
| `docs/standards/` `docs/guides/` | 规范细则与操作手册（规范类，受 §4 红线约束） | 平台 |
| `docs/adr/` | 平台级架构决策 | 平台 |
| `docs/templates/` | 复制源：`framework_demo/`、`project_demo/`；`CMakeLists.txt` 为模板冒烟构建 | 平台 |
| `docs/design/` `docs/reference/` | 平台总体方案、外部工程分析（**允许**出现工程名/芯片型号示例） | 平台 |
| `libs/` `port/<soc>/` | 环境层共享库与平台后端位置；**当前为空**（尚无真实共享代码，样板均在 `docs/templates/project_demo/` 内） | 平台 |
| `tools/` | 环境工具（`check_layers.py` 等） | 平台 |
| `projects/<id>/` | 具体工程，自包含，可整体删除 | 工程 |

## 2. 快速开始

```bash
# 模板冒烟构建 + 单测（不含产品构建）
cmake -S docs/templates -B build
cmake --build build              # smoke_app + smoke_test，零警告（-Werror）
ctest --test-dir build --output-on-failure

# 分层守护
python3 tools/check_layers.py
```

## 3. 常见入口

| 我要做什么 | 去哪 |
|---|---|
| 新增一个驱动 / 组件 / 协议属性 | `.github/skills/add-module/SKILL.md`（流程）+ `docs/guides/module-design.md`（契约） |
| 新建一个工程 | `cp -r docs/templates/project_demo projects/<id>`，见该目录 `README.md` §0 |
| 给框架加一类东西 | `docs/templates/framework_demo/README.md` |
| 了解平台为什么这样分层 | `docs/design/platform-plan.md`、`docs/adr/` |
| 看参考实现怎么做的 | `docs/reference/4x-mcu-framework-analysis.md` |

## 4. 当前状态（勿当成"已完成"）

| 项 | 状态 |
|---|---|
| 模板冒烟构建 / 单测 | ✅ `docs/templates` 下 2 个目标，9/9 用例通过 |
| `check_layers.py` | ✅ 通过，但当前扫描 **0 个文件**（样板全在 `docs/templates/`，被 `SKIP_DIRS` 跳过）；规则表待适配 |
| 共享库 / 平台后端 | ⛔ `libs/`、`port/` 尚未创建；框架骨架在 `docs/templates/framework_demo/`，模块样板在 `docs/templates/project_demo/libs/` |
| `env/presets/`、`.clang-format`、`.githooks/` | ⛔ 未建（`AGENTS.md` §7.1 已标注） |
