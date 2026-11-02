---
applyTo: "**/*.{c,h}"
---

# 嵌入式 C 约束（仅 `.c` / `.h` 生效）

> 本文件是"写 C 时永远成立"的细则；红线与验证命令见根 `AGENTS.md`。
> 格式问题由 `.clang-format` 机械保证，这里只写**格式工具管不了**的部分。

## 1. 第三方与生成代码：只读

| 目录 | 规则 |
|---|---|
| `framework/third_party/**`、任何 `**/generated/**` | **禁止修改**，也禁止格式化；升级只能整目录替换 |
| `**/*.ld`、**/`startup_*.s` | 禁止修改（链接脚本/启动文件） |
| HAL / FreeRTOS / CMSIS 等厂商代码 | 一律视为第三方；需要"改行为"就在 `framework/port/<soc>/` 包一层 |

违反即返工；`.githooks/pre-commit` 会拦截。

## 2. 注释：Doxygen 契约

- **公开头（对外 API）每个函数声明前必须有 Doxygen 块**，且包含：
  `@brief`、`@param`（每个参数一条）、`@return`（或 `@retval`）、
  **调用上下文**（ISR / 任务）、**可重入性**。
- 每个 `.c` 文件顶部必须有文件块：`@file`、`@brief`（职责）、
  `@par 执行上下文`、`@par 可重入性`、`@par 资源所有权`。
- 结构体/枚举成员用 `///<` 行尾注释，或成员上一行 `///`。
- **不写"改了什么的注释"**（那是 git 的职责）；注释解释"为什么"与"约束"。

```c
/**
 * @brief 读取一次采样结果
 *
 * @param[in]  id  逻辑 ID
 * @param[out] out 结果输出，不允许为 NULL
 * @return int
 *  - RET_SUCCESS   成功
 *  - RET_NO_OBJECT 实例不存在
 *
 * @note 调用上下文：任务上下文；可重入：否
 */
int tmplSensorRead(uint8_t id, tmplSensorData_t *out);
```

## 3. 花括号与排版（由 `.clang-format` 保证，此处说明意图）

- `if` / `for` / `while` / `switch` / `struct` / `enum` / 函数：**左花括号不换行**。
- 缩进 4 空格，不用 Tab；行宽 ≤ 100。
- 指针靠变量名：`int32_t *p`；强制转换不留空格：`(uint8_t)value`。
- 单行 `if` 不带花括号的写法**禁止**（`AllowShortIfStatementsOnASingleLine: false`）。

## 4. 类型与命名

- 只用 `stdint.h` 固定宽度类型做存储/协议字段；禁止裸 `int`/`long`。
- 内部符号一律 `static`；头文件只放声明。
- 每个模块统一前缀（`tmplSensor*`、`i2c*`、`sacp*`），文件名与模块名一致。

## 5. 错误与等待

- 所有返回值必须检查；失败分支 `elog` 带 `id` 与 `ret`。
- 禁止无超时的 `while` / 忙等 / 信号量等待。
- 总线失败不得返回陈旧数据（要么错误码，要么本次新数据）。

## 6. 分层（违反即架构问题）

- `drivers/`、`components/` 不得 `#include` 芯片头、`soc_*.h`、`FreeRTOS.h`。
- 芯片差异只能出现在 `framework/port/<soc>/`。
- 业务层（`projects/<id>/src/**`）不得用 `#ifdef` 区分芯片/板卡。

验证：`python3 tools/check_layers.py`。

## 7. 自检命令

```bash
python3 tools/check_layers.py
clang-format --dry-run --Werror <改动过的 .c/.h>
```
