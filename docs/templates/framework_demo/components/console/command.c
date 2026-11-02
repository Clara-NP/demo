/**
 * @file command.c
 * @author <author>
 * @brief 命令行组件实现：静态注册表 + 极简分词
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  主循环 / 控制台任务
 *
 * @par 可重入性
 *  否。注册表为单例，commandExec / commandExecLine 需由同一上下文串行调用
 *
 * @par 资源所有权
 *  拥有 s_commands[]（静态）；命令名字符串由调用方（宏）提供字面量，本模块不释放
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <common/generic.h>
#include <console/command.h>

static command_t s_commands[CMD_MAX_NUM];
static int s_count = 0;

int commandRegister(const char *name, const char *brief, const char *usage, cmdHandler_t handler)
{
    if ((name == NULL) || (handler == NULL))
    {
        return RET_INVALID_ARGS;
    }

    for (int i = 0; i < s_count; i ++)
    {
        if (strcmp(s_commands[i].name, name) == 0)
        {
            return RET_INVALID_ARGS;    // 重名
        }
    }

    if (s_count >= CMD_MAX_NUM)
    {
        return RET_INVALID_ARGS;        // 容量满
    }

    s_commands[s_count].name    = name;
    s_commands[s_count].brief   = brief;
    s_commands[s_count].usage   = usage;
    s_commands[s_count].handler = handler;
    s_count ++;

    return RET_SUCCESS;
}

int commandExec(const char *name, int argc, const char *const *argv)
{
    if (name == NULL)
    {
        return RET_INVALID_ARGS;
    }

    for (int i = 0; i < s_count; i ++)
    {
        if (strcmp(s_commands[i].name, name) == 0)
        {
            return s_commands[i].handler(argc, argv);
        }
    }

    return RET_NO_OBJECT;
}

int commandExecLine(const char *line)
{
    const char *args[8];
    char buf[128];
    int argc = 0;
    size_t len;
    char *p;

    if (line == NULL)
    {
        return RET_INVALID_ARGS;
    }

    len = strlen(line);
    if ((len == 0u) || (len >= sizeof(buf)))
    {
        return RET_INVALID_ARGS;
    }

    (void)memcpy(buf, line, len + 1u);

    p = buf;
    while ((*p != '\0') && (argc < (int)ARRAY_SIZE(args)))
    {
        while ((*p == ' ') || (*p == '\t'))
        {
            p ++;
        }
        if (*p == '\0')
        {
            break;
        }

        args[argc] = p;
        argc ++;

        while ((*p != '\0') && (*p != ' ') && (*p != '\t'))
        {
            p ++;
        }
        if (*p != '\0')
        {
            *p = '\0';
            p ++;
        }
    }

    if (argc == 0)
    {
        return RET_INVALID_ARGS;
    }

    return commandExec(args[0], argc - 1, &args[1]);
}

int commandCount(void)
{
    return s_count;
}

const command_t *commandAt(int index)
{
    if ((index < 0) || (index >= s_count))
    {
        return NULL;
    }

    return &s_commands[index];
}

void rawPrint(const char *fmt, ...)
{
    va_list ap;

    if (fmt == NULL)
    {
        return;
    }

    va_start(ap, fmt);
    (void)vprintf(fmt, ap);
    va_end(ap);
}
