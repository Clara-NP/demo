#ifndef __CONSOLE_COMMAND_H__
#define __CONSOLE_COMMAND_H__

/**
 * @file command.h
 * @author <author>
 * @brief 命令行组件：命令注册 + 执行 + rawPrint（只给 CLI 用，驱动不得依赖）
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  主循环 / 控制台任务；不得在 ISR 中调用
 *
 * @par 可重入性
 *  否（注册表为单例）
 *
 * @note 本文件是 framework_demo 骨架的一部分：复制到真实仓库时展开为
 *       `framework/components/console/command.h`
 */

#include <common/generic.h>

/// 命令表容量（超出时注册失败并返回 RET_INVALID_ARGS）
#define CMD_MAX_NUM 16

/**
 * @brief 命令处理函数
 *
 * @param[in] argc 参数个数（不含命令名）
 * @param[in] argv 参数数组
 * @return int 0 成功；非 0 为命令自定义错误
 */
typedef int (*cmdHandler_t)(int argc, const char *const *argv);

/**
 * @brief 一条命令的注册项
 */
typedef struct {
    const char *name;
    const char *brief;
    const char *usage;
    cmdHandler_t handler;
} command_t;

/**
 * @brief 注册命令
 *
 * @return int RET_SUCCESS / RET_INVALID_ARGS（容量满、参数非法、重名）
 */
int commandRegister(const char *name, const char *brief, const char *usage, cmdHandler_t handler);

/**
 * @brief 按名字执行命令
 *
 * @param[in] name 命令名
 * @param[in] argc 参数个数
 * @param[in] argv 参数数组
 * @return int 处理函数返回值；RET_NO_OBJECT 表示命令不存在
 */
int commandExec(const char *name, int argc, const char *const *argv);

/**
 * @brief 解析并执行一行文本（空格分隔）
 *
 * @param[in] line 命令行，如 "tmplsensor list"
 * @return int 同上
 *
 * @note 最大支持 8 个参数，超出返回 RET_INVALID_ARGS
 */
int commandExecLine(const char *line);

/**
 * @brief 已注册命令数量
 */
int commandCount(void);

/**
 * @brief 按序号取命令
 *
 * @return const command_t* NULL 表示越界
 */
const command_t *commandAt(int index);

/**
 * @brief 格式化输出（不带 trace 前缀）
 */
void rawPrint(const char *fmt, ...);

#ifdef __GNUC__

/**
 * @brief 定义命令（自动注册）
 *
 * @param[in] _name  命令名
 * @param[in] _brief 一句话说明
 * @param[in] _usage 用法文本
 *
 * @code
 *   CMD_DEFINE(demo, "demo command", "demo list\r\ndemo read <id>\r\n")
 *   {
 *       return (argc < 1) ? 2 : 0;
 *   }
 * @endcode
 *
 * @note host 用 `__attribute__((constructor))` 自动注册；
 *       MCU 目标应改为 linker section 自动注册，接口保持不变
 */
#define CMD_DEFINE(_name, _brief, _usage)                                                          \
    static int cmd_##_name##_handler(int argc, const char *const *argv);                           \
    static void __attribute__((constructor)) cmd_##_name##_register(void) {                        \
        (void)commandRegister(#_name, (_brief), (_usage), cmd_##_name##_handler);                  \
    }                                                                                              \
    static int cmd_##_name##_handler(int argc, const char *const *argv)

#else
#error "CMD_DEFINE 需要自动注册机制：本骨架只提供 GCC constructor 实现"
#endif

#endif // __CONSOLE_COMMAND_H__
