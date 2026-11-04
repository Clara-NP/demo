# `framework/` —— 共享框架（唯一源码，所有工程共用）


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
