---
status: approved
task: p0-agent-environment-convergence
date: 2026-11-02
scope: platform
---

# P0 Agent 开发环境收敛

## 1. 目标与边界

目标是把当前“模板可运行、规则多处漂移”的骨架收敛为 Agent 可确定执行的最小闭环：

1. `framework/` 是唯一共享源码。
2. 公开 API 使用固定宽度 `fw_err_t` 返回状态；数据和计数通过出参返回。
3. `check_layers.py` 必须扫描到真实源码，空扫描不得通过。
4. 当前可运行命令只有模板 smoke、分层检查和格式检查。

本任务不新增产品工程，不实现 CMake Presets、目标板工具链、Git hooks 或完整 CI。

## 2. 数据流与单一事实源

```text
framework/**                         共享源码唯一来源
    ↓ include / link
docs/templates/project_demo/**       工程复制源与 smoke 调用方
    ↓
docs/templates/CMakeLists.txt        host smoke 构建入口
    ↓
smoke_app + smoke_test               编译和行为验证
```

`docs/templates/framework_demo/` 不再保存可编译镜像，只保留脚手架说明，避免 `framework/`
与模板各自演进。

## 3. 状态与错误契约

- `fw_err_t` 固定为 `int32_t`。
- `FW_OK` 表示成功，`FW_ERR_*` 表示可恢复的失败原因。
- 公开 API 不用正数承载计数，不混合“数据”与“状态”。
- 计数使用 `uint8_t` / `uint16_t` 出参；失败时不得伪造有效数据。
- CLI handler 的退出码与标准 `main()` 返回值不是框架错误码，作为明确例外。

迁移期间不保留 `RET_*` 兼容别名，避免形成第二套契约。

## 4. 资源与分层边界

- `framework/port/<soc>/` 是芯片与 RTOS 私有头唯一落点。
- `framework/drivers/`、`framework/components/` 和 `projects/<id>/src/` 只能依赖公共接口。
- 所有等待必须有超时；禁止动态分配。
- `check_layers.py` 扫描文件数小于 1 时失败；模板目录仍不参与分层检查。

## 5. 任务入口

- 已有工程：读取 `projects/<id>/project.yaml`、局部 `AGENTS.md` 和工程方案。
- 新建工程：先复制 `docs/templates/project_demo/`，完成工程元数据后再按已有工程流程。
- 环境层任务：方案放在 `docs/design/<task>.md`。
- 模板层任务：读取模板 `AGENTS.md`，只验证复制源和 smoke，不把模板当产品工程。

所有方案统一使用 YAML frontmatter；只有 `status: approved` 才允许编码。

## 6. 实施顺序

1. 固化任务入口、方案路径和 frontmatter。
2. 展开唯一 `framework/`，迁移 `fw_err_t`、调用方和 host 单测。
3. 让分层检查拒绝空输入，并对齐 README、skills、guides 和 instructions。
4. 执行环境、构建、单测、分层和格式验证。

## 7. 验收标准

- `framework/` 包含实际共享源码，smoke 不再引用 `framework_demo` 源码。
- `framework_demo` 下不存在重复 `.c/.h`。
- 公开 API 不再使用 `RET_*`，也不通过返回值传递计数。
- `cmake -S docs/templates -B build`、构建和 `ctest` 全部通过。
- `python3 tools/check_layers.py --verbose` 扫描数大于 0 并通过。
- 新增的检查器单测覆盖空扫描、合法依赖、违规厂商头和 port 例外。
- 所有改动 C/H 通过 `clang-format --dry-run --Werror`。

## 8. 明确未完成

本任务完成后仍不代表目标板、Preset、Hook、配置生成器或 CI 已就绪；这些进入后续任务。
