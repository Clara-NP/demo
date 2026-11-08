/**
 * @file main.c
 * @brief 复位后按固定周期翻转本板状态灯
 *
 * @par 执行上下文
 * 复位后主上下文；不进入 ISR
 *
 * @par 可重入性
 * 否，仅启动调用一次
 *
 * @par 资源所有权
 * 不直接占用外设。灯脚在 board.c，时基在端口
 */

// #include "board.h"
// #include "osal.h"

// #include <stdio.h>
// #define BLINK_PERIOD_MS 500u

int main(void) {
    // fw_err_t err;

    // err = board_init();
    // if (err != FW_OK) {
    //     return 1;
    // }

    // for (;;) {
    //     err = board_led_toggle(LED_SYS);
    //     if (err != FW_OK) {
    //         return 1;
    //     }
    //     err = osal_delay_ms(BLINK_PERIOD_MS);
    //     if (err != FW_OK) {
    //         return 1;
    //     }
    // }

    // printf("hello world!");
    for(;;);
}
