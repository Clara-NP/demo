---
name: add-module
description: "Use when: 新增一个嵌入式 C 模块——传感器/外设驱动、总线驱动、组件、或需要接入协议的模块（关键词：加驱动、加传感器、新增模块、add driver、new module、接一个外设、加一条协议属性）。按 9 步流程生成四文件 + 配置四处 + CLI + 单测，并做分层与配置一致性校验。"
---

# 新增一个模块（add-module）

> 配套：`docs/guides/module-development.md`（流程与原理）、`docs/guides/module-design.md`（契约与差异）、`docs/templates/project_demo/libs/drivers/devices/tmpl_sensor/`（代码样板）
> 目标：**四文件 + 四处配置 + 一条 CLI + 一个 host 单测**，全部可验证。

---

## 步骤

### 1. 判定模块类型（先做，别跳过）

读到需求后先回答：**这个模块的 `#include` 列表里会不会出现芯片名？**

| 判定 | 类型 | 目标目录 |
|---|---|---|
| 直接读写寄存器 / 调 SDK | 总线/资源驱动 | `libs/drivers/bus/<name>/` |
| 通过抽象总线（i2c/spi/serial）访问器件 | 设备驱动 | `libs/drivers/devices/<name>/` |
| 纯逻辑（队列、状态机、编码、缓存） | 组件 | `libs/components/<name>/` |
| 含业务语义（设备模型、协议、控制策略） | 应用模块 | `projects/<id>/src/<mod>/` |

输出一句话结论，例如：**"这是设备驱动，走抽象 i2c，放 `libs/drivers/devices/<name>/`，不需要新增 port 实现。"**

### 2. 复制样板

```bash
cp -r docs/templates/project_demo/libs/drivers/devices/tmpl_sensor \
      <目标工程>/libs/drivers/devices/<name>
```

> 代码样板只有一份：`docs/templates/project_demo/libs/drivers/devices/tmpl_sensor/`（工程私有库位置）；
> `tmpl_` 前缀表示它不可被产品直接引用。
> 工程层落位参考工程骨架 `docs/templates/project_demo/`：工程只放 `src/config`（逻辑 ID / 容量）、
> `src/system/board.c`（描述符表）、`src/sacp`（协议）、`test`（单测）。

全局改名（四组）：`tmpl_sensor→<name>`、`tmplSensor→<name>`（驼峰）、`TMPL_SENSOR→<NAME>`（大写）、`tmplsensor→<name>`（CLI/日志 tag）。

按类型裁剪：

- 纯组件 → 不保留 `cli_<name>.c`
- 不接协议 → 删 `src/sacp/sacp_<name>.{c,h}`
- 总线/资源驱动 → 另加 `port/<soc>/soc_<name>.c`

### 3. 定义逻辑 ID

在 `config/export_ids.h` 增加枚举 + 哨兵：

```c
enum EXPORT_<NAME>
{
    <NAME>_0 = 0,          // 语义化命名优先，如 SENSOR_PRESSURE
    <NAME>_1,
    EXPORT_<NAME>_MAX_NUM  // 必须存在，供容量宏自动跟随
};
```

### 4. 写公开头 `include/<name>.h`

必须包含且**只包含**这些：

1. `#include <common/generic.h>`（+ 抽象总线/上游驱动头）
2. `#ifndef CONFIG_<NAME>_MAX_NUM` 默认值
3. `enum <NAME>_CHIP`（芯片/载体类型）+ 哨兵
4. `<name>Config_t` 描述符（`id` / `chip` / `bus|port` / 地址 / `flags` / `chipParameter`）
5. 对外 API —— **第一参数一律 `uint8_t id`**
6. 每个 API 的 Doxygen：`@brief @param @return` + **调用上下文（ISR/任务）+ 可重入性**

### 5. 写实现 `src/<name>.c`

固定结构，顺序不要变：

```c
/* ① 实例结构体：const config* + 运行期字段 */
/* ② 控制器单例：instances[CONFIG_<NAME>_MAX_NUM] + count + initialized */
/* ③ static <name>Instance_t *instanceFromId(uint8_t id)  —— 唯一的查找入口 */
/* ④ 参数解析 / 换算等纯函数（必须可在 host 上测） */
/* ⑤ 芯片分发：chipXxxProbe / chipXxxRead / chipSupportsXxx */
/* ⑥ 对外 API：全部做参数校验 → 查实例 → 分发 */
```

硬性规则：

- 禁止 `#include` 任何芯片头、`FreeRTOS.h`
- 禁止 `malloc/free`（实例在编译期确定）
- 禁止在驱动内 `rawPrint`（打印属于 CLI）
- 所有等待必须有超时；总线失败必须返回错误码，**不允许返回陈旧数据**
- 每个失败分支都要 `elog`，带 `id` 与 `ret`

### 6. 平台适配（仅总线/资源驱动需要）

- 在 `port/<soc>/` 下新建 `soc_<name>.c`，`#include` 芯片私有头
- 同时在 `common/soc/soc_<name>.h` 增加**纯声明**接口
- **每个** `port/<soc>/` 都要实现一遍，否则编译失败（这是刻意的）
- 设备驱动/组件：**不做这一步**，直接复用现有总线 API

### 7. 板级实例化（最容易漏，逐条做完）

按 `board/board_fragment.c` 的 5 处粘贴：

```text
① config/export_ids.h              逻辑 ID 枚举
② config/framework_config.h        #define CONFIG_<NAME>_MAX_NUM EXPORT_<NAME>_MAX_NUM
③ projects/<id>/board/board_*.h    static const <name>Config_t s_xxxs[] = {...}
                                   + #if ARRAY_SIZE(...) > CONFIG_...  #error
④ 板级初始化                        调用 <name>Init(s_xxxs, ARRAY_SIZE(s_xxxs))，并检查返回值
⑤ 其他同步点                        构建清单 / sacp / 文档接口对应表
```

禁止把引脚、地址、波特率写进 `src/<name>.c` 或 `include/<name>.h`。

### 8. 自检 CLI + 协议接入

- `cli/cli_<name>.c`：至少 `list` / `read` / `dump`，有写操作再加 `write|mode`
- 需要上报/控制 → `sacp/sacp_<name>.c`：属性表（ID 必须升序）+ 写回调 + 上报组 + `sacpXxxInit()` 注册
- 属性 ID 同步到 `src/sacp/local_attributes.h`，上报组同步到 `local_groups.h`

### 9. 单测 + 构建 + 验收

```bash
# host 单测（假总线，不接硬件）
cmake --preset host && ctest --preset host -R <name>

# 目标板编译，零警告
cmake --build --preset <board>

# 分层与配置守护
python tools/check_layers.py
python tools/check_config.py

# 风格
clang-format --dry-run --Werror <模块目录>
```

Keil 双轨并存时，按 `CMakeLists.txt` 末尾清单逐 target 勾选源文件。

---

## 禁止清单（出现即返工）

| # | 禁止 | 原因 |
|---|---|---|
| 1 | `<name>.c` / `<name>.h` 中出现芯片名、`HAL_`、寄存器名 | 破坏可移植性，等于放弃抽象层 |
| 2 | 引脚/地址/波特率写死在驱动里 | 换板要改驱动，板级配置失去意义 |
| 3 | 对外 API 不以 `uint8_t id` 为首参 | 无法多实例，后续必然重构 |
| 4 | 用 `static` 变量在函数内保存"唯一实例状态" | 隐式单例，多实例时静默串数据 |
| 5 | 容量宏写死数字、手写 `#define CONFIG_X_MAX_NUM 3` | 与导出枚举脱钩，加资源必忘 |
| 6 | 驱动内 `rawPrint` / 依赖 console | host 编译失败，单测无法进行 |
| 7 | 无超时的 `while` / 忙等 | 单点硬件异常即整机死锁 |
| 8 | 新增 `.c` 未加入构建清单就宣称"完成" | 链接期才炸，且每个 target 都要检查 |
| 9 | 只做 CLI 手测、不写 host 单测 | 无回归保护，AI 无法自验证 |
| 10 | 顺手重构无关模块 / 顺手加抽象层 | 改动面失控，评审无法聚焦 |

---

## 验收对话模板（完成时按此汇报）

```text
模块：<name>（<类型>）
位置：<目标目录>
改动文件：
  - 新增 <n> 个，修改 <m> 个（逐条列出）
配置同步：export_ids ✓ / framework_config ✓ / 板级表 ✓ / 构建清单 ✓ / sacp ✓
验证：
  - host 单测：<x>/<y> 通过
  - 目标板编译：0 warning
  - check_layers / check_config：通过
遗留：
  - <chipParameter 弱类型等已知项，如无则写"无">
```

---

## 相关

- 流程与原理：`docs/guides/module-development.md`
- 代码契约：`docs/guides/module-design.md`；工程骨架：`docs/templates/project_demo/`
- 逐步勾选：同目录 `checklist.md`
