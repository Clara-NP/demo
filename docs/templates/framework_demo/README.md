# framework_demo —— 框架层骨架

> **用途是把"框架该有哪些目录、每个头提供什么"钉下来**，复制到真实仓库后展开为**环境层** `framework/`（含 `port/`）。
> 与 `project_demo/` 的分工：`project_demo` 管"一个工程怎么摆"，本目录管"一套框架由什么组成"；
> **框架只有一份，所有工程共用，工程目录内不出现 `framework/` 或 `port/` 副本。**

> ✅ **本目录被编译**：`docs/templates/CMakeLists.txt` 的 `smoke_app` / `smoke_test` 会直接编译这里与
> `project_demo/src/`、`project_demo/test/` 的代码 —— 这是它不会腐烂的唯一保证。

---

## 1. 目录结构

```text
docs/templates/framework_demo/
├── README.md                              本文件
├── common/
│   ├── generic.h                          固定宽度类型、ARRAY_SIZE、RET_* 错误码
│   └── trace.h                            elog/wlog/ilog/dlog + TRACE_TAG/TRACE_LEVEL 编译期裁剪
├── osal/
│   └── osal.h                             osTickMs / sysTimeStamp 声明
├── drivers/
│   ├── bus/i2c/i2c.h                      抽象总线：只有 bus/addr/reg，无芯片符号
│   └── devices/tmpl_sensor/               示例设备驱动（新增模块就照它复制）
│       ├── tmpl_sensor.h                  描述符 + 枚举 + API + #ifndef 默认值
│       ├── tmpl_sensor.c                  控制器单例 + 实例数组 + chip 分发
│       └── cli_tmpl_sensor.c              自检命令：list / read / mode / dump
├── components/
│   ├── console/{command.h, command.c}     CMD_DEFINE + 注册表 + rawPrint
│   └── sacp/attribute/
│       ├── sacp_attribute.h               属性表/属性组/上报组 + 表定义宏
│       └── sacp_attribute.c               注册、查找、写入、上报（最小实现）
└── port/
    └── host/                              host 平台后端（内存模拟，供构建与单测）
        ├── os_host.c                      osTickMs / sysTimeStamp
        ├── i2c_host.c / i2c_host.h        i2cReadRegs / i2cWriteRegs / i2cProbe
        └── soc_tmpl_sensor_stub.c         总线/资源驱动的平台实现写法（示例，不参与构建）
```

目标板后端按同样形状新增：`port/<soc>/{os_<soc>.c, i2c_<soc>.c, soc_<name>.c}`。

## 2. 三条硬约束

| 约束 | 理由 | 违反后果 |
|---|---|---|
| 框架代码不得 `#include` 芯片头 / RTOS 头 | 换芯片只改 `port/<soc>/` | 分层守护 `check_layers.py` 直接报错 |
| 头文件只放声明，实现放 `.c` 或 `static inline` | 避免重复符号 | 链接期炸 |
| 框架不依赖 console 之外的"上层"组件 | console 只给 CLI 用 | host 编译失败、单测无法跑 |

## 3. 当前实现范围（最小集，够跑通 demo 链路）

| 模块 | 已实现 | 未实现（后续替换为完整组件） |
|---|---|---|
| `generic.h` | 类型、`ARRAY_SIZE`、`COMPILE_TIME_ASSERT`、`RET_*` | 编译器差异宏（`__PACKED` / `__WEAK`） |
| `trace.h` | host stdout 后端、4 级裁剪 | 串口/环形缓冲后端、时间戳前缀 |
| `osal.h` | `osTickMs` / `sysTimeStamp` 声明 + host 实现 | 任务、信号量、队列（本骨架不含 RTOS） |
| `i2c.h` | 读写/探测接口 + host 内存模拟 | 真实时序、DMA、超时策略 |
| `console` | 命令注册、按行执行、`rawPrint` | 历史记录、参数类型解析、补全 |
| `sacp_attribute` | 32 位族属性表/组/写回调/上报校验 | 64 位与 OCTET 表（仅空壳）、传输层、属性持久化 |

## 4. 展开为环境层的 `framework/`

```bash
cp -r docs/templates/framework_demo/. framework/    # 覆盖占位 README 之外的内容
```

展开后：

1. 删除本 README 中"骨架/最小实现"的表述，改为真实版本的说明
2. 目标板后端按 `framework/port/<soc>/` 补齐；缺一个平台就编译失败（这是刻意的）
3. `check_layers.py` 规则表指向 `framework/`；注意：样板当前全在 `docs/` 下（被 `SKIP_DIRS` 跳过），脚本会扫到 **0 个文件**

## 5. 验证

```bash
cmake -S docs/templates -B build     # 在仓库根执行
cmake --build build              # smoke_app + smoke_test，零警告（-Werror）
ctest --test-dir build --output-on-failure
```

`smoke_app` 只编译链接、不运行（工程侧 `main.c` 是死循环骨架）；
`smoke_test` 运行 `project_demo/test/` 的 9 个用例。
