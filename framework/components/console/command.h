#ifndef __CONSOLE_COMMAND_H__
#define __CONSOLE_COMMAND_H__

/**
 * @file command.h
 * @brief 命令注册、执行与 CLI 输出
 *
 * @par 执行上下文
 * 控制台任务；不得在 ISR 中调用
 *
 * @par 可重入性
 * 否，注册表为单例
 *
 * @par 资源所有权
 * 组件持有静态注册表；字符串由调用方持有
 */

#include <common/generic.h>

#define CMD_MAX_NUM 16

typedef int32_t (*cmdHandler_t)(int32_t argc, const char *const *argv);

typedef struct {
    const char *name;
    const char *brief;
    const char *usage;
    cmdHandler_t handler;
} command_t;

fw_err_t commandRegister(const char *name, const char *brief, const char *usage,
                         cmdHandler_t handler);

/**
 * @brief 按名字执行命令
 * @param[in] name 命令名
 * @param[in] argc 参数个数
 * @param[in] argv 参数数组
 * @param[out] exitCode 命令自定义退出码
 * @return fw_err_t
 */
fw_err_t commandExec(const char *name, int32_t argc, const char *const *argv, int32_t *exitCode);

/**
 * @brief 解析并执行一行文本
 * @param[in] line 命令行
 * @param[out] exitCode 命令自定义退出码
 * @return fw_err_t
 */
fw_err_t commandExecLine(const char *line, int32_t *exitCode);

fw_err_t commandGetCount(uint8_t *count);
fw_err_t commandGetAt(uint8_t index, const command_t **command);
void rawPrint(const char *fmt, ...);

#ifdef __GNUC__
#define CMD_DEFINE(_name, _brief, _usage)                                                          \
    static int32_t cmd_##_name##_handler(int32_t argc, const char *const *argv);                   \
    static void __attribute__((constructor)) cmd_##_name##_register(void) {                        \
        (void)commandRegister(#_name, (_brief), (_usage), cmd_##_name##_handler);                  \
    }                                                                                              \
    static int32_t cmd_##_name##_handler(int32_t argc, const char *const *argv)
#else
#error "CMD_DEFINE 需要自动注册机制：本骨架只提供 GCC constructor 实现"
#endif

#endif // __CONSOLE_COMMAND_H__
