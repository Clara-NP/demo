# `framework/` —— 共享框架（唯一一份，所有工程共用）

> **当前状态：占位目录，尚未落代码。**
> 框架的具体写法还没定稿；骨架参考 `docs/templates/framework_demo/`，定稿后再展开到这里。

## 计划结构

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
| 本目录不是模板：**不要**从 `docs/templates/` 再复制一份进来 | 模板只用于展开与参考 |

## 展开方式（框架写法定稿后）

```bash
cp -r docs/templates/framework_demo/. framework/     # 覆盖本 README 之外的骨架内容
python3 tools/check_layers.py                        # 展开后才真正扫到框架代码
```

> 展开前不构建是正常状态：模板的冒烟构建（`cmake -S docs/templates -B build`）已经验证代码可编译。
