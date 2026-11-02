# 设计：project_demo 工程骨架与 demo 落位

- **status**: approved
- **date**: 2026-11-02
- **rev**: 2（2026-11-02）目录由仓库根 `999_test_temp/` 迁至 `docs/templates/project_demo/`，定位改为"复制用骨架"
- **rev**: 3（2026-11-02）`docs/templates/module/` 已删除（与本节内容 90% 重复且不被验证），说明并入 `docs/guides/module-design.md`
- **task**: 把满分模块 demo 按工程目录结构落位成"开新工程时的参考骨架"
- **scope**: 只落目录结构与文档；**不新建框架子集代码，不声称编译通过**

---

## 1. 结论

demo 拆成两层落位：

- **框架（所有工程共用一份，位于环境层）**：`framework/drivers/devices/tmpl_sensor/` + `framework/port/host/`
  （模板期地址：`docs/templates/framework_demo/drivers/devices/tmpl_sensor/`、`.../framework_demo/port/host/`）
- **工程层（本模板）**：`src/{config,system,sacp}` + `test/` + `tools/` + `data/` + `docs/`

> 定位：本目录**只被复制、不被构建**（`docs/` 在 `check_layers.py` 的 `SKIP_DIRS` 内）。

代码样板收敛为**一份**：`framework_demo/drivers/devices/tmpl_sensor/` + `framework_demo/port/host/`；其契约说明见 `docs/guides/module-design.md`。

## 2. 目录职责

| 路径 | 职责 | 内容来源 |
|---|---|---|
| `framework_demo/drivers/devices/tmpl_sensor/` | 设备驱动：描述符解析、实例数组、chip 分发、自检 CLI | 模板 `include/` + `src/` + `cli/` |
| `framework_demo/port/host/soc_tmpl_sensor_stub.c` | 平台适配（唯一允许芯片头的地方） | 模板 `port/` |
| `src/config/export_ids.h` | 逻辑 ID + 哨兵 | 模板 `board_fragment.c` 片段 1 |
| `src/config/framework_config.h` | 容量宏派生 | 片段 2 |
| `src/system/board.{h,c}` | 描述符表 + 编译期校验 + 初始化 | 片段 3、4 |
| `src/sacp/` | 属性 ID / 上报组 / 开关 + 协议接入 | 模板 `sacp/` 拆成 5 文件 |
| `test/test_tmpl_sensor.c` | host 单测 + 假总线 | 模板 `tests/` |
| `src/main.c` | 启动序列（board → driver → sacp → 任务） | 新增骨架 |
| `{tools,data,src/ld}/README.md` | 占位与边界说明 | 新增文档 |

## 3. 与模板的差异（有意为之）

| # | 模板做法 | 本工程做法 | 原因 |
|---|---|---|---|
| 1 | `board_fragment.c` 用 `#if (sizeof(x)/sizeof(x[0])) > CONFIG_...` | 改用编译期 typedef 断言 | `#if` 在预处理阶段不支持 `sizeof`，该写法无法编译 |
| 2 | `sacp_tmpl_sensor.c` 内联"片段 A"属性枚举 | 枚举移入 `src/sacp/local_attributes.h` | 避免同一号段在多文件重复定义 |
| 3 | 描述符表在 `projects/<id>/board/*.h`（头文件） | 放 `src/system/board.c`（编译单元内 `static const`） | 本工程只有一份板级实现；避免头文件里定义对象导致重复符号 |
| 4 | 构建靠 `CMakeLists.txt` 单一源 | 模板不提供工程 `CMakeLists.txt`，`build.sh` 缺文件时快速失败 | 模板自身由 `docs/templates/CMakeLists.txt` 冒烟编译；工程清单复制后手写，不靠 glob |

## 4. 验证状态（如实记录）

| 项 | 状态 |
|---|---|
| 目录结构与文档 | ✅ 已完成 |
| `cmake` 构建（smoke_app + smoke_test） | ✅ 已执行：`-Wall -Wextra -Werror -Wconversion -Wshadow -Wundef` 零警告 |
| host 单测 | ✅ 已执行：9/9 用例通过（`ctest`） |
| `python3 tools/check_layers.py` | ✅ 已执行：通过（7 个文件；本目录在 `docs/` 下不在扫描范围） |
| `clang-format` | ⛔ 未执行：本地无 `clang-format`，且 `.clang-format` 未建 |

## 5. 后续待办

1. ~~建 `framework/common`、`framework/drivers/bus/i2c`、`framework/components/{console,sacp}` 最小实现~~（已完成：见 `docs/templates/framework_demo/`，由 `docs/templates/CMakeLists.txt` 冒烟编译）
2. 复制到 `projects/<id>/` 时补 `CMakeLists.txt`，并把 `build.sh` 换成真实构建+单测
3. ~~修模板 `#if sizeof`~~（已完成：模板已删除，正确写法在 `src/system/board.c`，契约写入 `docs/guides/module-design.md` §3）
4. `tools/check_layers.py` 规则表适配 `framework/**` 与 `projects/*/src/**`
5. ~~加"模板冒烟编译"目标~~（已完成：`docs/templates/CMakeLists.txt` 的 `smoke_app` / `smoke_test`）
