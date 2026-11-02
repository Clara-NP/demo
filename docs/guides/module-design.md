# 模块设计说明（Module Design）

> 版本：v1.0（2026-11-02）
> 定位：讲**一个模块的代码为什么这么写** —— 依赖契约、错误码、同步清单、构建接入、常见错误。
> 不讲：流程步骤与设计要素原理（见 `docs/guides/module-development.md` §2~§4）。

---

## 0. 与原 `docs/templates/module/` 的关系

原目录**已删除**，本文件承接其中属于"文档"的部分。

删除原因（实测，非拍脑袋）：

| 原模板文件 | 现存同源位置 | 比对结果 |
|---|---|---|
| `include/tmpl_sensor.h` | `docs/templates/project_demo/libs/drivers/devices/tmpl_sensor/` | 完全相同 |
| `src/tmpl_sensor.c` | 同上 | 完全相同 |
| `cli/cli_tmpl_sensor.c` | 同上 | 完全相同 |
| `tests/test_tmpl_sensor.c` | `docs/templates/project_demo/test/` | 完全相同 |
| `sacp/*` | `docs/templates/project_demo/src/sacp/` | 仅 Doxygen 更详细 |
| `board/board_fragment.c` | `docs/templates/project_demo/src/system/board.c` | 已落地为真实代码 |

且它位于 `docs/` 下 —— 既不参与构建、也不被 `tools/check_layers.py` 扫描（`SKIP_DIRS` 含 `docs`），
缺陷会长期潜伏（`board_fragment.c` 里的 `#if sizeof(...)` 无法编译，躺到 2026-11-02 才被发现）。

结论：**代码只留一份（可编译的那份），文档只写代码表达不了的内容。**

## 1. 代码样板在哪（唯一来源）

| 角色 | 位置 | 说明 |
|---|---|---|
| 公开头 | `docs/templates/project_demo/libs/drivers/devices/tmpl_sensor/tmpl_sensor.h` | 描述符 + 枚举 + API + `#ifndef` 默认值 |
| 实现 | `.../tmpl_sensor.c` | 控制器单例 + 实例数组 + chip 分发 |
| 自检 CLI | `.../cli_tmpl_sensor.c` | `list / read / mode / dump` |
| 平台适配 | `docs/templates/project_demo/port/host/soc_tmpl_sensor_stub.c` | 唯一允许芯片头的落点 |
| 工程侧 | `docs/templates/project_demo/src/{config,system,sacp}` | 逻辑 ID、容量、板级表、协议接入 |
| 单测 | `docs/templates/project_demo/test/test_tmpl_sensor.c` | host 单测 + 假总线 |

新增模块（在目标工程内）：

```bash
cp -r docs/templates/project_demo/libs/drivers/devices/tmpl_sensor \
      <目标工程>/libs/drivers/devices/<name>
# 四组改名：tmpl_sensor→<name>、tmplSensor→<name>驼峰、TMPL_SENSOR→<NAME>、tmplsensor→<name>
```

> `tmpl_sensor` 是**样板**，不是产品驱动：产品代码不得依赖它，构建清单也不得默认包含它。

## 2. 依赖假设与错误码

模块按"抽象接口"编写，假设环境提供以下内容（名字不同则按实际调整，**不得因此引入芯片头**）。
这些头已在 `docs/templates/framework_demo/` 提供最小实现，并由 `docs/templates/CMakeLists.txt` 的 `smoke_app` / `smoke_test` 编译验证：

| 依赖 | 用途 | 期望形态 |
|---|---|---|
| `common/generic.h` | 固定宽度类型、`ARRAY_SIZE`、`handle_t`、错误码 `RET_*` | 平台公共头 |
| `common/trace.h` | `elog/wlog/ilog/dlog`，按 `TRACE_TAG` / `TRACE_LEVEL` 裁剪 | 需支持 host（打印到 stdout） |
| `i2c/i2c.h` | `i2cReadRegs` / `i2cWriteRegs`（总线抽象，无芯片符号） | 总线驱动公开头 |
| `osal` | `osTickMs()` | OSAL |
| `console/command.h` | `CMD_DEFINE(name, brief, usage)` + `argc/argv` + `rawPrint` | 组件 |
| `sacp/attribute/sacp_attribute.h` | `LOCAL_V32_TABLE` / `LOCAL_GROUP_DEFINE` / `attributeGroupRegister` 等 | 组件 |
| C 标准 | **C99**（`for (int i = ...)`、`bool`、`//` 注释） | 编译选项 |

错误码约定（建议固化到 `common/generic.h`）：

```c
#define RET_SUCCESS         0
#define RET_FAILED         -1
#define RET_NO_OBJECT      -2     /* 实例/对象不存在 */
#define RET_INVALID_ARGS   -3     /* 参数非法 */
#define RET_NOT_SUPPORT    -4     /* 当前芯片/型号不支持 */
#define RET_TIMEOUT        -5     /* 超时 */
#define RET_TYPE_MISMATCH  -6
```

## 3. 新增模块的 5 处同步清单

| # | 位置 | 写什么 | 漏了会怎样 |
|---|---|---|---|
| 1 | `src/config/export_ids.h` | 逻辑 ID 枚举 + `EXPORT_<NAME>_MAX_NUM` 哨兵 | 容量宏无从跟随 |
| 2 | `src/config/framework_config.h` | `#define CONFIG_<NAME>_MAX_NUM EXPORT_<NAME>_MAX_NUM` | 容量与资源脱钩 |
| 3 | `src/system/board.c` | `static const <name>Config_t s_xxxs[]`（引脚/地址/参数唯一来源） | 换板要改驱动 |
| 4 | `src/system/board.c` 初始化 | `<name>Init(s_xxxs, ARRAY_SIZE(s_xxxs))` + 检查返回值 | 静默少设备 |
| 5 | 构建 + 协议 + 文档 | CMake/Keil 源文件、`src/sacp/*`、接口对应表 | 链接期才炸 / 协议对不上 |

容量校验必须写成**编译期**断言，且**不能用 `#if`**（预处理阶段没有 `sizeof`）：

```c
#define TMPL_SENSOR_NUM     (sizeof(s_tmplSensors) / sizeof(s_tmplSensors[0]))

/* 正确：编译期断言 */
typedef char tmplSensorCapacityCheck_t[
    (TMPL_SENSOR_NUM <= CONFIG_TMPL_SENSOR_MAX_NUM) ? 1 : -1];

/* 错误：预处理阶段拿不到 sizeof，无法编译 */
/* #if (sizeof(s_tmplSensors) / sizeof(s_tmplSensors[0])) > CONFIG_TMPL_SENSOR_MAX_NUM */
/* #error "..." */
/* #endif */
```

## 4. 构建接入片段（仅供参考，以实际构建脚本为准）

```cmake
# 库目标：驱动 + CLI 同属一个目标 → 不会出现"忘了加 CLI"
add_library(tmpl_sensor STATIC
    tmpl_sensor.c
    cli_tmpl_sensor.c
)
target_include_directories(tmpl_sensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 只依赖抽象接口，禁止链接任何芯片库
target_link_libraries(tmpl_sensor PUBLIC fw_common fw_console fw_i2c fw_osal)

target_compile_options(tmpl_sensor PRIVATE
    -Wall -Wextra -Werror -Wconversion -Wshadow -Wundef -Wswitch-enum)

# host 单测：只编纯逻辑 + 假总线，不编 cli（cli 依赖 console）
if(BUILD_TESTING)
    add_executable(test_tmpl_sensor test_tmpl_sensor.c tmpl_sensor.c)
    target_link_libraries(test_tmpl_sensor PRIVATE fw_common fw_osal)
    add_test(NAME tmpl_sensor.unit COMMAND test_tmpl_sensor)
endif()
```

Keil 双轨并存时逐 target 勾选：`<name>.c`、`cli_<name>.c`、Include Paths、`CONFIG_<NAME>_MAX_NUM` 宏。

## 5. 与"参考实现"的差异（本方案主动补齐的部分）

| 参考实现的做法 | 本方案改法 | 原因 |
|---|---|---|
| 只有上板 CLI，无 host 单测 | 增加 `tests/` + 假总线 | AI 必须能自闭环验证 |
| 驱动内直接 `rawPrint` | 打印职责移到 `cli/`，驱动只提供 `GetIdByIndex` / `GetConfig` | 驱动不依赖 console 才能在 host 编译 |
| 无编译期容量校验 | 编译期 typedef 断言（见 §3） | 把"加资源忘改容量"变成编译错误 |
| 构建清单靠手工勾选 | CMake 单一来源 + Keil 勾选清单 | 消除"新增 .c 忘加" |
| `chipParameter` 字符串静默解析 | 解析失败 `wlog` + 保留默认值 | 不假装它是强类型 |
| 配置分散四处无记录 | §3 的 5 条清单 | 让固定动作可逐条勾掉 |

## 6. 常见错误

| 症状 | 原因 | 处理 |
|---|---|---|
| 上板后 CLI 命令不存在 | `cli_<name>.c` 未进构建清单 | 补 `target_sources` / Keil Source Group |
| 只有第一个设备能用 | 描述符 `id` 重复 | 检查 `export_ids.h` 与板级表 |
| 读到全 0 | 容量宏小于描述符条数（断言被绕过） | 保持编译期断言 |
| 换板后某实例失效 | 引脚/地址写在驱动里 | 移到板级描述符表 |
| 编译报 `RET_*` 未定义 | 环境 `generic.h` 未提供统一错误码 | 按 §2 补齐 |
| 编译报 `sizeof` 不能用在 `#if` 中 | 容量校验写成了 `#if` | 按 §3 改 typedef 断言 |

## 7. 遗留

1. ~~`libs/common`、`libs/drivers/bus/i2c`、`libs/components/{console,sacp}` 未提供~~（已完成：`docs/templates/framework_demo/` 最小实现，`smoke_app` 编译通过、`smoke_test` 9/9 用例通过）
2. ~~样板无冒烟编译目标~~（已完成：`docs/templates/CMakeLists.txt` 含 `smoke_app` / `smoke_test`）
3. 历史分析文档（`docs/reference/4x-mcu-framework-analysis.md`、`docs/design/platform-plan.md`）中"在 `docs/templates/module/` 放满分模块"属**当时的方案快照**，不再代表当前结构，未回改
