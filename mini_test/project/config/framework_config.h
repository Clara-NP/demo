// #ifndef __FRAMEWORK_CONFIG_H__
// #define __FRAMEWORK_CONFIG_H__

// #include "export_ids.h"

// #ifndef CONFIG_FREERTOS
// #define CONFIG_FREERTOS 1
// #endif

// #define CONFIG_GPIO_MAX_NUM      EXPORT_GPIO_MAX_NUM
// #define CONFIG_GPIO_CACHE_SIZE   CONFIG_GPIO_MAX_NUM
// #define CONFIG_BUTTON_MAX_NUM    0
// #define CONFIG_DIGIT_INPUT_MAX_NUM 0

// #define CONFIG_TRACE_UART_WAIT_MAX_TIME 50

// /*
//  * 与 N1 相同：不定义则 framework 默认 0，ilog 走 socUartSend。
//  * 开 FIFO 必须同时有 UART TX DMA，否则 flush 把 -1 当成不可用，A9/A10 全黑。
//  */
// //#define CONFIG_TRACE_FIFO_SIZE  4096

// #define CONFIG_DEBUG_CONSOLE  1
// #define CONFIG_CONSOLE_PROMPT "SBL> "
// #define CONFIG_CONSOLE_ENTER_EXECUTE_LAST_COMMAND  1
// #define CONFIG_SERIAL_MAX_NUM 1   /* 仅 SERIAL_UM982；USART1 不是这个数 */
// #define CONFIG_TIMER_MAX_NUM     EXPORT_TIMER_MAX_NUM
// #define CONFIG_TIMER_CACHE_SIZE  CONFIG_TIMER_MAX_NUM
// #define CONFIG_PWM_MAX_NUM       EXPORT_PWM_MAX_NUM
// #define CONFIG_PWM_CACHE_SIZE    CONFIG_PWM_MAX_NUM
// #define CONFIG_GPS_TIME_TASK_PRIORITY  (CONFIG_FRAMEWORK_TASK_PRIORITY + 2)
// #define CONFIG_GPS_TIME_TASK_PERIOD_MS 5

// #define CONFIG_I2C_MAX_NUM          EXPORT_I2C_MAX_NUM
// #define CONFIG_I2S_MAX_NUM          EXPORT_I2S_MAX_NUM
// #define CONFIG_SPI_MAX_NUM          EXPORT_SPI_MAX_NUM
// #define CONFIG_I2C_EEPROM_MAX_NUM   1
// #define CONFIG_KELLER_XLD_MAX_NUM   1
// #define CONFIG_CLI_I2C              1
// #define CONFIG_DRIVER_I2C_GPIO_SIMULATE 1
// #define CONFIG_GPIO_I2C_BUS_MAX_NUM     2
// #define CONFIG_DEPTH_SENSOR_SAMPLE_PERIOD  200
// #define CONFIG_FONT_ACORN_8X8       1
// #define CONFIG_SD_MAX_NUM           1
// #define CONFIG_CLI_SD               1

// #ifndef CONFIG_BOARD_PLATFORM_HEADER
// #define CONFIG_BOARD_PLATFORM_HEADER "boards/f407zg/platform_stm32.h"
// #endif
// #include CONFIG_BOARD_PLATFORM_HEADER

// #endif
