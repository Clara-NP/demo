# `framework/` —— 共享框架（唯一源码，所有工程共用）

> **当前状态：已展开并由 host smoke 构建验证。**
> `docs/templates/framework_demo/` 只保留脚手架说明，不保存本目录的代码副本。

## 当前结构

```text
framework/
├── common/            generic.h / trace.h（类型、错误码、trace）
├── osal/              osal.h（osTickMs / sysTimeStamp）
├── drivers/
│   ├── bus/i2c/i2c.h  抽象总线接口
│   └── devices/<name>/ <name>.h  <name>.c  cli_<name>.c
├── components/
│   ├── console/       command.h / command.c
│   └── sacp/attribute/ sacp_attribute.h / .c
└── port/<soc>/        平台后端：os_<soc>.c / i2c_<soc>.c / soc_<name>.c
```

## 约定

| 规则 | 原因 |
|---|---|
| 所有工程共用这一份，**工程内不得出现 `framework/` 或 `port/` 副本** | 各存一份必然分叉，改一处崩一片 |
| 平台差异只允许出现在 `port/<soc>/` | 换芯片只改这一层 |
| 除 `port/<soc>/` 外，任何文件不得 `#include` 芯片厂商头 / RTOS 头 | 分层守护 `tools/check_layers.py` |
| 本目录是代码唯一来源；禁止向 `framework_demo` 复制镜像 | 防止双真源漂移 |
| 公开 API 返回 `fw_err_t`；数据和计数走固定宽度出参 | 状态与数据不混用 |

## 验证

```bash
cmake -S docs/templates -B build
cmake --build build
ctest --test-dir build --output-on-failure
python3 tools/check_layers.py --verbose
```

`tmpl_sensor` 是 golden example，不是产品器件名；产品不得直接依赖它。新增模块时按
`.github/skills/add-module/SKILL.md` 复制并完成重命名。
