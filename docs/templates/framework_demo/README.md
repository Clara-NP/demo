# framework_demo —— 框架脚手架说明

> 本目录不保存可编译源码。共享框架的唯一源码位于仓库根 `framework/`，并由
> `docs/templates/CMakeLists.txt` 直接编译。

## 1. 为什么不保留代码镜像

`framework/` 与 `docs/templates/framework_demo/` 各放一份 `.c/.h` 会形成双真源：
一份通过构建，另一份可能长期腐烂。P0 收敛后采用以下边界：

- `framework/`：common、osal、drivers、components、port 的唯一源码。
- `docs/templates/project_demo/`：新产品工程的复制源。
- 本目录：解释框架结构和指向 golden example，不复制框架实现。

## 2. Golden example

- 设备驱动：`framework/drivers/devices/tmpl_sensor/`
- 抽象总线：`framework/drivers/bus/i2c/`
- host port：`framework/port/host/`
- 工程接入：`docs/templates/project_demo/`
- 开发流程：`.github/skills/add-module/SKILL.md`

新增产品模块时复制 `tmpl_sensor` 到 `framework/drivers/devices/<name>/` 并完整重命名；
产品代码不得直接依赖 `tmpl_` 示例。

## 3. 验证

```bash
cmake -S docs/templates -B build
cmake --build build
ctest --test-dir build --output-on-failure
python3 tools/check_layers.py --verbose
```
