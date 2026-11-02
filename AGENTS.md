# 平台约定（环境级 · 所有工程适用）

> 环境层约束，作用于整个工作区。不含任何具体工程/芯片信息。
> 工程级差异写在 `projects/<id>/AGENTS.md`。

## 0. 冲突优先级

1. 更深的 `AGENTS.md` > 本文件；例外：「硬性禁止」不可被覆盖。
2. 本文件 > `docs/standards/` > 个人习惯。
3. 拿不准时停下来问，不要猜。

## 1. 工作区结构

- **环境层**：`AGENTS.md`、`.github/`、`docs/`、`env/`、`tools/`、`libs/` —— 可复用，与工程无关。
- `libs/` 内部划分：`hal/`（纯接口）、`drivers/{bus,devices}/`、`components/`、`osal/`；
  新代码用 `components/`（`services/` 若存在视为旧称）。
- **实例层**：`projects/<id>/` —— 具体工程，完全自包含，可随时增删。
- 依赖方向单向：工程可引用环境层；**环境层禁止引用具体工程**。
- 工程入口：`projects/<id>/project.yaml`（芯片、板卡、工具链、preset）。

## 2. 开始任何任务前

1. 读 `projects/<id>/project.yaml` 与 `projects/<id>/AGENTS.md`。
2. 确认是否有 `docs/design/<task>.md`；没有则先出方案（§3）。
3. 只 include 公共接口，不 include 厂商 SDK 头文件。
4. 只改与本任务相关的文件，不做未要求的重构。

## 3. 工作流程（强制）

1. **方案先行**：先给方案，不写代码。
2. 用户确认后写入 `projects/<id>/docs/design/<task>.md`；`status: approved` 才允许编码。
3. **结论先行**：回复第一段给结论，再解释；原因控制在 2~4 行或一张表。
4. **图按需**：跨层 / 并发 / 时序 / 改动 ≥3 文件才画图，其余用文字。
5. 按方案章节逐段实现，一个 commit 对应一个章节。

## 4. 硬性禁止

- 业务层（`src/app`、`libs/services`、`libs/drivers`）不得出现厂商 SDK / RTOS 头文件。
  验证：`python tools/check_layers.py`（规则表在脚本头部，可用 `--list-rules` 查看）
- 业务层不得用 `#ifdef` 区分芯片或板卡；差异只能位于 `port/<soc>/`。
- 禁止修改 `libs/third_party/**`、`**/generated/**`、`**/*.ld`、`**/startup_*.s`。
- 禁止修改 `.deps/**`（由同步脚本重建）；`.deps/` 必须被 gitignore。
- 禁止 `malloc/free`；禁止在中断上下文阻塞、打印、动态分配。
- 禁止无超时的等待循环（`while` / 忙等 / 信号量）。
- 禁止用 `#include "../../..."` 穿越工程边界引用共享代码。
- 环境层（本文件、`docs/`、`env/`、`tools/`、`.github/`）不得写入具体工程名或芯片型号。
  验证：`grep -rn "projects/00" docs/ AGENTS.md env/ tools/ | grep -v 示例`

## 5. 语言与类型

- 只用 `stdint.h` 固定宽度类型；禁止用裸 `int/long` 做存储或协议字段。
- 编译选项：`-Wall -Wextra -Werror -Wconversion -Wshadow -Wundef`，警告未清零视为未完成。
- 所有返回值必须检查；错误码统一 `xxx_err_t`。
- 内部符号一律 `static`；头文件只放声明。
- 每个 `.c` 顶部注释：职责 / 执行上下文（ISR | 线程）/ 可重入性 / 资源所有权。

## 6. 提交规范

- 标题：`<type>(<scope>): <做了什么> [<task> §<章节>]`
- 正文必填：`Task` / `Design` / `Part` / `Why` / `Verify`
- 一个 commit 只对应方案文档的一个章节；跨章节必须拆分。
- 禁止：`update`、`fix bug`、`修改`、`优化一下`。
- 校验：`.githooks/commit-msg` `[待建]`（就绪前人工自检标题与五个必填字段）

## 7. 完成标准

> ⏳ **状态说明**：标 `[待建]` 的条目，其依赖的基础设施（`env/presets/`、`.clang-format`、`tools/`、`.githooks/`）就绪前**不阻塞任务**，只提示；就绪后自动转为强制。
> 工程可用 `projects/<id>/AGENTS.md` 覆盖为该项目自己的验证命令（覆盖时须写明具体命令）。

- `[待建]` `cmake --preset host && ctest --preset host` 全绿（依赖 `env/presets/base.json`）
- `[待建]` `cmake --preset <board> && cmake --build --preset <board>` 无警告（依赖工程 preset）
- `[待建]` `clang-format --dry-run --Werror` 通过（依赖 `.clang-format`）
- 新模块：契约注释 + host 单测 + 在方案文档中可追溯
- 未实际验证的内容不得写成“已完成”（无论基础设施是否就绪，此条始终强制）

## 7.1 状态来源

| 条目 | 依赖 | 就绪后 |
|---|---|---|
| `check_layers.py` 验证 | ✅ `tools/check_layers.py` 已就绪 | **已强制**：改 C 代码后必跑 |
| `commit-msg` 校验 | `.githooks/commit-msg` | 转为强制 |
| host preset 测试 | `env/presets/base.json` | 转为强制 |
| 板上构建 | 工程 `CMakePresets.json` | 转为强制 |
| 格式检查 | `.clang-format` | 转为强制 |

**判断方法**：命令或文件不存在 → 视为“未就绪”，在任务结束时明确写出“该项因基础设施未建而跳过”，不得省略不提。

## 8. 文档索引

- **新增/修改模块（先读这个）**：`docs/guides/module-development.md`（流程）+ `docs/guides/module-design.md`（契约）；流程化执行用 skill `add-module`；代码样板见 `libs/drivers/devices/tmpl_sensor/`
- **模板落地示例（工程层与库层怎么分）**：`docs/templates/project_demo/`（工程骨架，复制用）+ 模块实体在 `libs/drivers/devices/`、平台实现在 `port/`
- **框架层骨架**：`docs/templates/framework_demo/`（common/osal/drivers/components 的最小实现，由根 `CMakeLists.txt` 冒烟编译）
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
