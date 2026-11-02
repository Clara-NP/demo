# 工程约定：project_demo（工程骨架模板）

> 本文件只能收紧环境层 `AGENTS.md`，不得放宽。
> 环境层「硬性禁止」在本工程同样有效。

## 0. 本目录的性质

- 这是 `docs/templates/project_demo/`，**复制源**：`cp -r` 到 `projects/<id>/` 后改名为该工程
- 复制后把标题里的 `project_demo` 改成 `<id>`，并删掉本节（它只对模板自身有意义）
- 本目录**不参与构建**、**不被 `check_layers.py` 扫描**（`docs` 在脚本 `SKIP_DIRS` 内）

## 1. 定位

- **工程骨架**：演示"模块四文件 + 配置同步 + 自检 CLI + host 单测"在工程内如何落位
- 首选 **host 构建**（无需硬件），目标板构建后续再补

## 2. 依赖边界（框架一份，所有工程共用）

| 依赖 | 位置（从本目录出发） | 说明 |
|---|---|---|
| 框架库 | `../../../framework/`（模板期：`../../../docs/templates/framework_demo/`） | `common/`、`osal/`、`drivers/`、`components/` |
| 平台后端 | `../../../framework/port/<soc>/`（模板期：`.../framework_demo/port/<soc>/`） | 唯一允许出现芯片头的目录 |
| 环境工具 | `../../../tools/`、`../../../docs/` | `check_layers.py`、标准与指南 |

**工程内不得出现 `framework/` 或 `port/` 副本**：驱动与平台后端只改框架侧，并评估对所有工程的影响。

## 3. 本工程目录职责

| 路径 | 放什么 | 不放什么 |
|---|---|---|
| `src/config/export_ids.h` | 逻辑 ID 枚举 + 哨兵 | 引脚、地址、波特率 |
| `src/config/framework_config.h` | 开关与容量宏（由枚举派生） | 手写数字容量 |
| `src/system/board.{h,c}` | 描述符表（硬件事实唯一来源）、初始化调用 | 协议属性、业务状态机 |
| `src/sacp/` | 属性 ID、上报组、协议接入 | 驱动内部实现、芯片符号 |
| `src/ld/` | 链接脚本（生成后锁定） | 手改断点、调试残留 |
| `test/` | host 单测 + 假总线 | 依赖芯片的测试 |
| `tools/` | 仅本工程使用的脚本 | 环境层工具的拷贝 |
| `data/` | 现场数据（标定、日志） | 被 `#include` 的数据 |

## 4. 验证命令

| 项 | 命令 | 状态 |
|---|---|---|
| host 冒烟构建 + 单测 | 在仓库根：`cmake -S docs/templates -B build && cmake --build build && ctest --test-dir build` | ✅ 已执行：0 警告，9/9 用例通过 |
| 复制后工程构建 | `./build.sh` | ⛔ 需先补工程自身 `CMakeLists.txt` |
| 分层守护 | `python3 ../../../tools/check_layers.py` | ⛔ 本目录在 `docs/` 下，脚本跳过；复制到 `projects/<id>/` 后才生效 |
| 格式 | `clang-format --dry-run --Werror` | ⛔ 本地无 `clang-format`、无 `.clang-format` |

> 未实际执行的项，汇报时必须写"未验证"，不得写"已完成"。

## 5. 本工程额外禁止

- 在 `src/**` 出现任何芯片厂商头文件、`HAL_`、寄存器名（芯片差异只能进 `framework/port/<soc>/`）
- 在 `src/system/board.c` 之外的任何文件写引脚/地址/波特率
- 为通过编译而临时 `#define` 掉缺失的框架符号
