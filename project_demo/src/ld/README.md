# `ld/` —— 链接脚本目录（占位）

本目录预留给工程链接脚本（`*.ld` / `*.lds`）。

- **禁止**仓库内改写：链接脚本属于工程基建，按平台模板生成后锁定
- host 构建不需要本目录
- 命名建议：`<board>_<backend>.ld`，与 `src/config/framework_config.h` 中的后端选择保持一致
