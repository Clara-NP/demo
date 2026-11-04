/**
 * @file command.c
 * @brief 静态命令注册表与命令行分词
 *
 * @par 执行上下文
 * 控制台任务
 *
 * @par 可重入性
 * 否
 *
 * @par 资源所有权
 * 持有静态注册表；不复制字符串
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <common/generic.h>
#include <console/command.h>

static command_t s_commands[CMD_MAX_NUM];
static uint8_t s_count;

fw_err_t commandRegister(const char *name, const char *brief, const char *usage,
                         cmdHandler_t handler) {
    if ((name == NULL) || (handler == NULL)) {
        return FW_ERR_INVALID_ARGS;
    }

    for (uint8_t i = 0u; i < s_count; i++) {
        if (strcmp(s_commands[i].name, name) == 0) {
            return FW_ERR_INVALID_ARGS;
        }
    }
    if (s_count >= CMD_MAX_NUM) {
        return FW_ERR_FAILED;
    }

    s_commands[s_count].name = name;
    s_commands[s_count].brief = brief;
    s_commands[s_count].usage = usage;
    s_commands[s_count].handler = handler;
    s_count++;
    return FW_OK;
}

fw_err_t commandExec(const char *name, int32_t argc, const char *const *argv, int32_t *exitCode) {
    if ((name == NULL) || (exitCode == NULL)) {
        return FW_ERR_INVALID_ARGS;
    }
    *exitCode = 0;

    for (uint8_t i = 0u; i < s_count; i++) {
        if (strcmp(s_commands[i].name, name) == 0) {
            *exitCode = s_commands[i].handler(argc, argv);
            return FW_OK;
        }
    }
    return FW_ERR_NO_OBJECT;
}

fw_err_t commandExecLine(const char *line, int32_t *exitCode) {
    const char *args[8];
    char buf[128];
    uint8_t argc = 0u;
    size_t len;
    char *p;

    if ((line == NULL) || (exitCode == NULL)) {
        return FW_ERR_INVALID_ARGS;
    }
    *exitCode = 0;

    len = strlen(line);
    if ((len == 0u) || (len >= sizeof(buf))) {
        return FW_ERR_INVALID_ARGS;
    }
    (void)memcpy(buf, line, len + 1u);

    p = buf;
    while ((*p != '\0') && (argc < (uint8_t)ARRAY_SIZE(args))) {
        while ((*p == ' ') || (*p == '\t')) {
            p++;
        }
        if (*p == '\0') {
            break;
        }

        args[argc] = p;
        argc++;
        while ((*p != '\0') && (*p != ' ') && (*p != '\t')) {
            p++;
        }
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
    }

    if (argc == 0u) {
        return FW_ERR_INVALID_ARGS;
    }
    return commandExec(args[0], (int32_t)(argc - 1u), &args[1], exitCode);
}

fw_err_t commandGetCount(uint8_t *count) {
    if (count == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *count = s_count;
    return FW_OK;
}

fw_err_t commandGetAt(uint8_t index, const command_t **command) {
    if (command == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *command = NULL;

    if (index >= s_count) {
        return FW_ERR_NO_OBJECT;
    }
    *command = &s_commands[index];
    return FW_OK;
}

void rawPrint(const char *fmt, ...) {
    va_list ap;

    if (fmt == NULL) {
        return;
    }
    va_start(ap, fmt);
    (void)vprintf(fmt, ap);
    va_end(ap);
}
