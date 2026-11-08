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

#include "platform.h"


static void startTask(void *parameter)
{
    (void)parameter;

    // ilog("=============================");
    // ilog("=  SBL bring-up %-12s =", SBL_BOARD_NAME);
    // ilog("=============================");

    // consoleInit();

    // boardInit();
    // (void)sblConfigLoad();
    // gpsTimeInit();
    // pwmChirpInit();
    // hydroRxInit();
    // txVoltageInit();
    // sblEventLogInit();
    // sblEventLog(SBL_EVT_BOOT, NULL);
    // if (sblConfigState() == SBL_CFG_DEGRADED) {
    //     sblEventLog(SBL_EVT_CFG, "degraded");
    // } else {
    //     sblEventLog(SBL_EVT_CFG, "ready");
    // }
    // if (!sdMounted(STORAGE_TF)) {
    //     sblEventLog(SBL_EVT_SD, "timeout");
    // }
    // sblEventLogPoll();
    // addCommand(CMD(reset));
    // addCommand(CMD(i2c));
    // addCommand(CMD(sd));
    // addCommand(CMD(cfg));
    // addCommand(CMD(txv));
    // addCommand(CMD(chirp));
    // addCommand(CMD(rx));
    // oledSensorInit();
    // depthSensorInit();
    // ledFlash(LED_SYS, sblConfigGet()->ledPattern, 0xffu);
    // ilog("led flash started, id=%d pattern=0x%02x",
    //      LED_SYS, (unsigned)sblConfigGet()->ledPattern);

    // xTaskCreate(taskFramework, "fwk", 512, NULL, CONFIG_FRAMEWORK_TASK_PRIORITY, NULL);
    // vTaskDelete(NULL);
}


#define NULL 0

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

    /// 平台初始化
    platformInit();

    /// 板级初始化
    startTask(NULL);

    /// 主循环
    for(;;);
}
