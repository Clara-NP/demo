# project_demo —— 新建工程用的骨架

> **用途是复制，不是独立产品构建。** 开新工程时 `cp -r docs/templates/project_demo projects/<id>`，再按 §0 改三处。
> 本目录只放**产品差异**；驱动与平台后端在共享框架 `framework/`，**工程内不出现 `framework/` 或 `port/` 副本**。

> 本目录的源码由 `docs/templates/CMakeLists.txt` 参与 host smoke，但位于 `docs/` 下，
> 不被 `tools/check_layers.py` 扫描；复制到 `projects/<id>/` 后才进入分层守护范围。

---

## 0. 复制到新工程（三步）

| 步 | 动作 | 检查点 |
|---|---|---|
| 1 | `cp -r docs/templates/project_demo projects/<id>` | 目录名与 `project.yaml` 的 `id` 一致 |
| 2 | 改 `project.yaml`：`id` / `name` / `soc` / `toolchain` / `presets` | 字段含义见文件内注释；引脚/属性 ID 不写在这里 |
| 3 | 改 `src/config/export_ids.h`：删样例资源、按本工程重命名枚举 | 末尾 `EXPORT_<NAME>_MAX_NUM` 必须保留 |
| 4 | 补工程自身的 `CMakeLists.txt`（模板刻意不提供） | `./build.sh` 能编译并跑 `ctest` |

> 复制后**只留用得到的部分**：不接协议就整目录删 `src/sacp/`；`test/` 与模块同进退。

## 1. 一句话

**工程只放产品差异：逻辑 ID、容量、板级表、协议接入、单测。驱动与平台后端在共享框架里，所有工程共用一份。**

## 2. 目录结构

```text
project_demo/                  （复制后改名为 projects/<id>/）
├── AGENTS.md                 工程约束（依赖边界、目录职责、验证状态）
├── README.md                 本文件
├── project.yaml              机器可读元信息（id/soc/toolchain/presets/共享框架位置）
├── build.sh                  复制后工程的构建入口（缺 CMakeLists 时快速失败）
├── data/                     现场数据（标定 / 日志），不被源码 include
├── docs/design/              方案文档（一个任务一篇，approved 后才编码）
├── src/
│   ├── main.c                启动序列：boardInit → sacpInit → 任务
│   ├── config/
│   │   ├── export_ids.h      ① 逻辑 ID 枚举 + EXPORT_*_MAX_NUM 哨兵
│   │   └── framework_config.h ② 容量宏由枚举派生（禁止手写数字）
│   ├── system/
│   │   ├── board.h           板级初始化接口
│   │   └── board.c           ③ 描述符表（引脚/地址唯一来源）+ 编译期容量校验 + Init
│   ├── sacp/
│   │   ├── local_config.h    ④ 协议开关
│   │   ├── local_attributes.h ④ 属性 ID 号段（段内升序）
│   │   ├── local_groups.h    ④ 上报组基址
│   │   ├── local_common.h    ④ 协议公共声明占位（组件就绪后替换为 include）
│   │   ├── sacp_tmpl_sensor.h
│   │   └── sacp_tmpl_sensor.c ⑤ 属性表 + 写回调 + 上报 + 注册
│   └── ld/                   链接脚本（占位）
├── test/
│   └── test_tmpl_sensor.c    ⑥ host 单测（自带假总线，零依赖）
└── tools/                    工程级脚本（占位）
```

驱动（`tmpl_sensor`）与平台后端（`port/host`）不在本目录，见仓库根 `framework/`。

## 3. 新增一个模块时按这个顺序填

| 步 | 落在哪个文件 |
|---|---|
| ① 逻辑 ID | `src/config/export_ids.h` |
| ② 容量 | `src/config/framework_config.h` |
| ③ 板级表 | `src/system/board.c` |
| ④ 协议配置 | `src/sacp/local_{config,attributes,groups}.h` |
| ⑤ 协议接入 | `src/sacp/sacp_<name>.{h,c}` |
| ⑥ 单测 | `test/test_<name>.c` |
| ⑦ 驱动本体 | 共享框架 `framework/drivers/devices/<name>/`（不在工程内） |

## 4. 构建

模板自身由 `docs/templates/CMakeLists.txt` 的 smoke 目标编译（直接链接根 `framework/`），在**仓库根**执行：

```sh
cmake -S docs/templates -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

复制成 `projects/<id>/` 后，`./build.sh` 才是该工程的入口。它目前会快速失败：
工程自身的 `CMakeLists.txt` 必须按**真实源文件清单**手写（刻意不提供，避免 glob 掩盖漏加文件）。

## 5. 已知与模板的差异

| 差异 | 原因 |
|---|---|
| 容量校验用 typedef 断言，不用 `#if sizeof(...)` | `#if` 不支持 `sizeof`，模板写法无法编译 |
| 描述符表在 `board.c` 内而非板级头文件 | 头文件定义对象会产生重复符号 |
| 属性枚举下沉到 `local_attributes.h` | 号段集中管理，避免多模块抢号 |

## 6. 与代码样板的分工

| 位置 | 回答的问题 | 用在哪 |
|---|---|---|
| `framework/drivers/devices/tmpl_sensor/` | **一个模块**怎么写（驱动 + CLI） | `cp -r` 到 `framework/drivers/devices/<name>/` |
| `framework/port/host/` | 平台适配怎么写 | 目标板时改为 `framework/port/<soc>/` |
| 本目录整体 | **一个工程**怎么组织（ID、容量、板级表、协议接入、单测） | `cp -r` 到 `projects/<id>/` |
| `docs/guides/module-design.md` | 依赖假设、错误码、5 处同步清单、构建片段、常见错误 | 直接读，不复制 |

三者不一致时以 `docs/guides/module-development.md` 为准，并同步修正。

> 已知边界：模板源码会被 smoke 编译，但不会被分层脚本扫描；复制到产品工程后必须重新运行
> `check_layers.py`。
