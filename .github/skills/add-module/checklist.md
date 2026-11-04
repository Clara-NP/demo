# add-module 勾选清单

> 复制到任务里逐条勾掉。任一条未勾 = 未完成。

## A. 判定与准备

- [ ] 已判定模块类型：☐ 总线/资源驱动 ☐ 设备驱动 ☐ 组件 ☐ 应用模块
- [ ] 已确定目标目录，并在回复中说明理由（一句话）
- [ ] 已 `cp -r framework/drivers/devices/tmpl_sensor framework/drivers/devices/<name>`
- [ ] 已完成四组改名（`tmpl_sensor` / `tmplSensor` / `TMPL_SENSOR` / `tmplsensor`）
- [ ] 已按类型裁剪（组件去 `cli_`；不接协议删 `src/sacp/sacp_<name>.*`）
- [ ] 已对照工程骨架 `docs/templates/project_demo/` 确认工程层落位：`src/config` / `src/system/board.c` / `src/sacp` / `test`

## B. 代码（四文件）

- [ ] `projects/<id>/src/config/export_ids.h` 增加逻辑 ID 枚举 + `EXPORT_<NAME>_MAX_NUM`
- [ ] `<name>.h`（公开头，与驱动同目录）
  - [ ] `#ifndef CONFIG_<NAME>_MAX_NUM` 默认值
  - [ ] `enum <NAME>_CHIP` + 哨兵
  - [ ] `<name>Config_t` 描述符
  - [ ] 所有对外 API 首参为 `uint8_t id`
  - [ ] API 返回 `fw_err_t`，数据与计数使用固定宽度出参
  - [ ] 每个 API 有 Doxygen：参数 / 返回值 / 调用上下文 / 可重入性
- [ ] `src/<name>.c`
  - [ ] 实例结构体 + 控制器单例 + `instanceFromId()`
  - [ ] 芯片分发（`switch (chip)` 或 ops 表）
  - [ ] 无芯片头 / 无 malloc / 无 `rawPrint` / 无 `FreeRTOS.h`
  - [ ] 每个失败分支 `elog` 带 `id` 与 `ret`；无无界等待
- [ ] `cli_<name>.c`（与驱动同目录）：`list` / `read` / `dump`（+ `write|mode`）
- [ ] （总线驱动才做）`framework/port/<soc>/soc_<name>.c` + `framework/common/soc/soc_<name>.h`
- [ ] （需要协议时）`sacp/sacp_<name>.c`：属性表升序 + 写回调 + 上报组 + 注册

## C. 配置同步（最容易漏）

- [ ] `projects/<id>/src/config/framework_config.h`：`#define CONFIG_<NAME>_MAX_NUM EXPORT_<NAME>_MAX_NUM`
- [ ] `projects/<id>/src/system/board.c`：`static const <name>Config_t s_xxxs[]`（引脚/地址/参数唯一来源）
- [ ] `projects/<id>/src/system/board.c`：编译期 typedef 断言（描述符条数 ≤ `CONFIG_<NAME>_MAX_NUM`）
- [ ] `projects/<id>/src/system/board.c`：调用 `<name>Init()` 并检查返回值（部分失败要 `wlog`）
- [ ] `src/sacp/local_attributes.h` / `local_groups.h` / `local_config.h`（如接协议）
- [ ] 文档：板级接口对应表 / 协议说明（如有）

## D. 构建

- [ ] CMake：`target_sources` 已加模块目录下全部 `.c`（驱动 + `cli_<name>.c`，模板为扁平目录）
- [ ] CMake：`target_include_directories` 已加模块目录
- [ ] CMake：`target_compile_options` 含 `-Werror -Wconversion -Wshadow -Wundef`
- [ ] Keil（若双轨）：**每个** target 的 Source Group 与 Include Path 已同步

## E. 测试

- [ ] `projects/<id>/test/test_<name>.c` 覆盖：
  - [ ] 初始化绑定数量正确
  - [ ] 容量溢出返回 `FW_ERR_INVALID_ARGS`
  - [ ] 未注册 ID 返回 `FW_ERR_NO_OBJECT`
  - [ ] 主要换算/解析逻辑（含边界值）
  - [ ] 总线失败 → 错误码（不返回陈旧数据）
  - [ ] 不支持的模式/芯片返回 `FW_ERR_NOT_SUPPORTED`
- [ ] 假总线（mock）不依赖任何芯片

## F. 验收（命令必须实际执行过）

- [ ] `cmake -S docs/templates -B build && cmake --build build && ctest --test-dir build --output-on-failure` 全绿
- [ ] `python3 -m unittest discover -s tools/tests` 全绿
- [ ] `python3 tools/check_layers.py --verbose` 通过且扫描数大于 0
- [ ] 目标板 preset：基础设施未建，明确记录“未验证”
- [ ] `tools/check_config.py`：基础设施未建，明确记录“未执行”
- [ ] `clang-format --dry-run --Werror <模块目录>` 通过
- [ ] 上板：`<name> list` 能列出全部实例；`<name> read <id>` 返回合理值

## G. 收尾

- [ ] `CHANGELOG.md` 已补记（涉及发版时）
- [ ] 按"验收对话模板"汇报（模块/位置/改动/配置同步/验证/遗留）

---

## 自检 5 问（口述一遍）

1. 我的 `.c` 里出现过芯片名 / `HAL_` / 寄存器名吗？
2. 引脚 / 地址 / 波特率是否只在板级配置？
3. 所有对外 API 第一参数是 `uint8_t id` 吗？
4. 容量常量是 `#ifndef` 默认值 + 产品覆盖吗？
5. 有 CLI 或协议属性能在板上验证，并有 host 单测吗？
