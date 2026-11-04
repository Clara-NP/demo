# 平台约定（环境级 · 所有工程适用）

> 环境层约束，作用于整个工作区。不含任何具体工程/芯片信息。
> 工程级差异写在 `projects/<id>/AGENTS.md`。

## 0. 冲突优先级

1. 更深的 `AGENTS.md` > 本文件；例外：「硬性禁止」不可被覆盖。
2. 本文件 > `docs/standards/` > 个人习惯。
3. 拿不准时停下来问，不要猜。

## 1. 工作区结构

- **环境层**：`AGENTS.md`、`.github/`、`docs/`、`env/`、`tools/`、`framework/` —— 可复用，与工程无关。
- `framework/` 内部划分：`common/`、`osal/`、`hal/`（纯接口）、`drivers/{bus,devices}/`、`components/`、`port/<soc>/`（平台实现，唯一允许芯片头的落点）；
  新代码用 `components/`（`services/` 若存在视为旧称）。
- **共享一份**：`framework/` 属环境层，所有工程共用同一份；**工程目录内不得出现 `framework/` 或 `port/` 副本**。
- **实例层**：`projects/<id>/` —— 只放产品差异（逻辑 ID、容量、板级表、协议接入、单测），可随时增删。
- 依赖方向单向：工程可引用环境层；**环境层禁止引用具体工程**。
- 工程入口：`projects/<id>/project.yaml`（芯片、板卡、工具链、preset）。

## 2. 开始任何任务前

先判定任务类型，不得猜工程 ID：

1. **已有工程**：读 `projects/<id>/project.yaml` 与最近的 `AGENTS.md`；方案在
   `projects/<id>/docs/design/<task>.md`。
2. **新建工程**：先按 `README.md` 从 `docs/templates/project_demo/` 建立工程并填写
   `project.yaml`，再进入“已有工程”流程。
3. **环境层**（`framework/`、`tools/`、`docs/` 等）：方案在 `docs/design/<task>.md`，
   不要求虚构 `projects/<id>`。
4. **模板层**：读模板内 `AGENTS.md`；模板只作复制源和 smoke，不视为产品工程。

所有任务只 include 公共接口，不 include 厂商 SDK 头文件；只改任务相关文件。

## 3. 工作流程（强制）

1. **方案先行**：先给方案，不写代码。
2. 用户确认后按 §2 的任务类型写入方案；统一使用 YAML frontmatter，只有
   `status: approved` 才允许编码。
3. **结论先行**：回复第一段给结论，再解释；原因控制在 2~4 行或一张表。
4. **图按需**：跨层 / 并发 / 时序 / 改动 ≥3 文件才画图，其余用文字。
5. 按方案章节逐段实现，一个 commit 对应一个章节。

## 4. 硬性禁止

- 业务层（`projects/<id>/src/**`）与组件/驱动层（`framework/**`）不得出现厂商 SDK / RTOS 头文件。
  验证：`python3 tools/check_layers.py`（规则表在脚本头部，可用 `--list-rules` 查看）
- 业务层不得用 `#ifdef` 区分芯片或板卡；差异只能位于 `framework/port/<soc>/`。
- 禁止修改 `framework/third_party/**`、`**/generated/**`、`**/*.ld`、`**/startup_*.s`。
- 禁止修改 `.deps/**`（由同步脚本重建）；`.deps/` 必须被 gitignore。
- 禁止 `malloc/free`；禁止在中断上下文阻塞、打印、动态分配。
- 禁止无超时的等待循环（`while` / 忙等 / 信号量）。
- 禁止用 `#include "../../..."` 穿越工程边界引用共享代码。
- 环境层（本文件、`docs/`、`env/`、`tools/`、`.github/`）不得写入具体工程名或芯片型号。
  - 例外：`docs/design/`（平台方案）与 `docs/reference/`（外部工程分析）**允许**出现示例性质的具体名称，它们不是规范。
  - 验证：`grep -rn "projects/00" docs/standards docs/guides docs/adr AGENTS.md env/ tools/ .github/ | grep -v 示例`

## 5. 语言与类型

- 只用 `stdint.h` 固定宽度类型；禁止用裸 `int/long` 做存储或协议字段。
- 编译选项：`-Wall -Wextra -Werror -Wconversion -Wshadow -Wundef`，警告未清零视为未完成。
- 所有返回值必须检查；框架公开 API 统一返回 `fw_err_t`，数据与计数通过固定宽度出参返回。
- CLI handler 与标准 `main()` 返回退出码，不属于 `fw_err_t` 契约。
- 内部符号一律 `static`；头文件只放声明。
- 每个 `.c` 顶部注释：职责 / 执行上下文（ISR | 线程）/ 可重入性 / 资源所有权。

## 6. 提交规范

- 标题：`<type>(<scope>): <做了什么> [<task> §<章节>]`
- 正文必填：`Task` / `Design` / `Part` / `Why` / `Verify`
- 一个 commit 只对应方案文档的一个章节；跨章节必须拆分。
- 禁止：`update`、`fix bug`、`修改`、`优化一下`。
- 校验：`.githooks/commit-msg` `[待建]`（就绪前人工自检标题与五个必填字段）

## 7. 完成标准

> ⏳ **状态说明**：标 `[待建]` 的条目，其依赖的基础设施（`env/presets/`、`.githooks/`）就绪前**不阻塞任务**，只提示；就绪后自动转为强制。
> 工程可用 `projects/<id>/AGENTS.md` 覆盖为该项目自己的验证命令（覆盖时须写明具体命令）。

- `cmake -S docs/templates -B build && cmake --build build && ctest --test-dir build --output-on-failure` 全绿
- `python3 -m unittest discover -s tools/tests` 与 `python3 tools/check_layers.py --verbose` 通过
- `[待建]` `cmake --preset host && ctest --preset host` 全绿（依赖 `env/presets/base.json`）
- `[待建]` `cmake --preset <board> && cmake --build --preset <board>` 无警告（依赖工程 preset）
- `clang-format --dry-run --Werror` 通过（`.clang-format` + `clang-format` 工具 ✅ 就绪，模板代码已格式化）
- 新模块：契约注释 + host 单测 + 在方案文档中可追溯
- 未实际验证的内容不得写成“已完成”（无论基础设施是否就绪，此条始终强制）

## 7.1 状态来源

| 条目 | 依赖 | 就绪后 |
|---|---|---|
| `check_layers.py` 验证 | ✅ `tools/check_layers.py` 已就绪 | **已强制**：改 C 代码后必跑 |
| `commit-msg` 校验 | `.githooks/commit-msg` | 转为强制 |
| host preset 测试 | `env/presets/base.json` | 转为强制 |
| 板上构建 | 工程 `CMakePresets.json` | 转为强制 |
| 格式检查 | ✅ `.clang-format` + `clang-format` 均就绪 | **已强制**：改 C 代码后必跑 |
| 环境自检 | ✅ `tools/check_env.py`、`tools/bootstrap.sh` | **已就绪**：新环境先跑它 |

**判断方法**：命令或文件不存在 → 视为“未就绪”，在任务结束时明确写出“该项因基础设施未建而跳过”，不得省略不提。

## 8. 文档索引

- **环境自检**：`python3 tools/check_env.py`（工具链/配置/目录三态报告），新环境先跑 `./tools/bootstrap.sh`
- **写 C 代码前先读**：`.github/instructions/c-embedded.instructions.md`（Doxygen 契约 / 花括号 / 第三方只读 / 分层）
- **新增/修改模块（先读这个）**：`docs/guides/module-development.md`（流程）+ `docs/guides/module-design.md`（契约）；流程化执行用 skill `add-module`；golden example 见 `framework/drivers/devices/tmpl_sensor/`
- **模板落地示例（工程层与框架层怎么分）**：`docs/templates/project_demo/`（工程骨架，复制用）+ 模块实体在 `framework/drivers/devices/`、平台实现在 `framework/port/`
- **共享框架**：`framework/` 是 common/osal/drivers/components/port 的唯一源码，由 `docs/templates/CMakeLists.txt` 冒烟编译
- **平台总体方案**：`docs/design/platform-plan.md`（分层、约束体系、依赖策略）
- **外部参考实现分析**：`docs/reference/4x-mcu-framework-analysis.md`（只读笔记，非本仓库规范）
- 编码细则：`docs/standards/coding-style.md`
- 错误处理：`docs/standards/error-handling.md`
- 测试：`docs/standards/testing.md`
- 移植：`docs/standards/porting-guide.md`
- 评审清单：`docs/standards/review-checklist.md`
- 操作手册：`docs/guides/`
- 平台决策：`docs/adr/`（规则与索引见 `docs/adr/README.md`）
- 工程文档：`projects/<id>/docs/{design,adr}/`

## 9. 防过度发挥

- 不新增未被要求的抽象层、接口或配置文件。
- 不批量重命名、不整体格式化无关文件。
- 不同时修改多个工程。
- 引入新依赖（库/工具）前先说明理由并等确认。
