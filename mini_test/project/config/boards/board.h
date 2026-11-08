#ifndef __BOARD_H__
#define __BOARD_H__

#include "config/boards/platform_stm32.h"
#include <common/generic.h>
#include <gpio/gpio.h>
// #include <serial/serial.h>
// #include <i2c/i2c.h>
// #include <adc/adc.h>
// #include <eeprom/i2c_eeprom.h>
// #include <storage/fatfs/sd.h>
// #include <timer/timer_capture.h>
// #include <pwm/pwm.h>
// #include "system/tx_voltage_cfg.h"

#include "config/export_ids.h"

#ifndef __BOARD_DEFINE__
#error "!!!This header is only for board.c. please check"
#endif

// #define BOARD_NAME "f407zg"
// #define BOARD_HAS_TX_VOLTAGE 1

/**
 * @brief GPIO。LED / FAULT。Chirp / SDIO / PPS(TIM5) 不在此表。
 */
static const gpioConfig_t s_gpios[] =
{
    /// SYSLED。现行 PF9（Explorer）；产品网表 PC0
    { LED_SYS,          GPIO_CHIP_SOC, _PF9,  GPIO_DIR_OUTPUT, GPIO_FLAG_ACTIVE_LOW },
    // /// T_LED，发射灯。网表要求内部上拉；本平台 OUTPUT 模式已带上拉
    // { LED_TX,           GPIO_CHIP_SOC, _PC1,  GPIO_DIR_OUTPUT, 0 },
    // /// R_LED，接收灯。网表要求内部上拉
    // { LED_RX,           GPIO_CHIP_SOC, _PC2,  GPIO_DIR_OUTPUT, 0 },
    // /// A4955_FAULT_IN
    // { GPIO_A4955_FAULT, GPIO_CHIP_SOC, _PC14, GPIO_DIR_INPUT,  0 },
};

// /**
//  * @brief UM982 PPS。PA0 = TIM5_CH1，CC 中断由 gpsTimeInit 武装
//  */
// static const timerCaptureConfig_t s_timers[] =
// {
//     {
//         .id        = TIMER_UM982_PPS,
//         .chip      = TIMER_CAPTURE_CHIP_SOC,
//         .timer     = _TIMER5,
//         .channel   = 1,
//         .gpio      = _PA0,
//         .tickHz    = 1000000u,
//         .polarity  = TIMER_CAPTURE_POLARITY_RISING,
//         .filter    = 2,
//         .flags     = TIMER_CAPTURE_FLAG_NONE,
//     },
// };

// /**
//  * @brief 产品串口。GPS = USART2，不和 TRACE USART1 抢口。
//  *
//  * 默认脚 PA2 TX / PA3 RX。数字电台脚未给出，不登记。
//  */
// static const serialConfig_t s_serials[] =
// {
//     {
//         .id = SERIAL_UM982,
//         .chip = SERIAL_CHIP_SOC,
//         .port = _UART2,
//         .setting = _UART_SETTING_DEFAULT(115200),
//         .control = _UART_RX_DMA,
//         .chipParameter = NULL,      /* 默认 PA2/PA3 */
//     },
// };

// /**
//  * @brief I2C。I2C1 板载器件 + 深度总线。深度从设备在 depth_sensor.c 注册。
//  *
//  * I2C1 走 GPIO 模拟 PB8/PB9（硬件 I2C 扫总线无人 ACK）。
//  * 深度走 GPIO 模拟 PA8/PB11，不用 PC9。
//  */
// static const i2cConfig_t s_i2cBuses[] =
// {
//     {
//         .id = I2C_BOARD_SENSOR,
//         .chip = I2C_CHIP_GPIO_SIMULATE,
//         .port = _I2C1,
//         .rate = 100000,
//         .chipParameter = (void *)"scl:24,sda:25",   /* PB8 / PB9 */
//     },
//     {
//         .id = I2C_DEPTH_SENSOR,
//         .chip = I2C_CHIP_GPIO_SIMULATE,
//         .port = _I2C3,
//         .rate = 100000,
//         .chipParameter = (void *)"scl:8,sda:27",    /* PA8 / PB11，不是 PC9 */
//     },
// };

// /**
//  * @brief 板上 EEPROM，7-bit 地址 0x50。本期只探测 ACK，不读写内容。
//  */
// static const i2cDeviceConfig_t s_eepromBus =
// {
//     .id = EEPROM_MAIN,
//     .bus = I2C_BOARD_SENSOR,
//     .address = 0x50,
// };

// static const i2cEepromConfig_t s_eeprom =
// {
//     .pageSize = 32,
//     .pageDelay = 10,
//     .byteDelay = 1,
//     .chipSize = 4096,
// };

// /**
//  * @brief 输入电压。PC3 = ADC123_IN13，分压 G=1/11。
//  *
//  *   Vadc = ADC * 3.3 / 4096
//  *   Vin  = 11 * Vadc
//  */
// static const adcConfig_t s_adcs[] __attribute__((unused)) =
// {
//     {
//         .id = ADC_VIN_SENSE,
//         .chip = ADC_CHIP_SOC,
//         .adc = _ADC1,
//         .channel = _ADC_CH13,
//         .flags = ADC_FLAG_RANGE(12),
//         .calculation = ADC_LINEAR(0, 11.0f * 3.3f / 4096.0f),
//         .parameter = NULL,
//     },
// };

#endif /* __BOARD_H__ */