/**
 * @file system_init.c
 * @brief 满足 CMSIS 启动文件对 SystemInit 与静态构造的引用
 *
 * @par 执行上下文
 * 复位后、main 之前，由 startup_stm32f446xx.s 调用
 *
 * @par 可重入性
 * 否，只在启动时调用一次
 *
 * @par 资源所有权
 * 不配置时钟，不占用外设。官方启动文件在 -nostdlib 下仍会调用这两个符号
 */

void SystemInit(void) {}

void __libc_init_array(void) {}
