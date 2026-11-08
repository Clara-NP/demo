/**
 * @file board.c
 * @brief 板级 bring-up。
 */

#define __BOARD_DEFINE__    /// 用于确保board.h前已经包含了board.c
#include "config/boards/board.h"

//  #include <pwm/pwm.h>
//  #include <serial/serial.h>
//  #include <timer/timer_capture.h>
//  #include <i2s/i2s.h>
//  #include <spi/spi.h>
#include <gpio/gpio.h>
 
//  #define TRACE_TAG "board"
//  #define TRACE_LEVEL T_INFO
//  #define TRACE_ENABLE
//  #include <common/trace.h>
 
 /**
  * @brief 板级 bring-up。gpio → pwm(钉 100%) → serial → timer → i2c → i2s(只登记) → spi。
  */
void boardInit(void)
{
    int ret = 0;

    /// 初始化GPIO
    ret = gpioInit(s_gpios, ARRAY_SIZE(s_gpios));
    // ASSERT(ret >= 0, "gpioInit failed");
    if (ret < 0) {
        // elog("gpioInit failed");
        return;
    }

    // /// 初始化SERIAL
    // ret = serialInit(s_serials, ARRAY_SIZE(s_serials));
    // // ASSERT(ret >= 0, "serialInit failed");

}
 