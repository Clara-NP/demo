#ifndef __EXPORT_IDS_H__
#define __EXPORT_IDS_H__


// /**
//  * @brief 硬件ID，在V2.1 及以后的主控硬件中，引入了这个标识 
//  * 
//  */
// enum BOARD_ID
// {
//     BOARD_ID_V2_1 = 0,
//     BOARD_ID_V3_0 = 3,
//     BOARD_ID_V3_1 = 5,
//     // 未知
//     BOARD_ID_NONE = 0xff,
// };


/**
 * @brief GPIO定义
 * 
 */
enum EXPORT_GPIO
{
    LED_SYS = 0,

    EXPORT_GPIO_MAX_NUM
};

/// GPIO名称定义
#define GPIO_NAMES \
"SysLed\0" \
"TestOut\0" \

#define GPIO_NAME(v) stringAt(GPIO_NAMES, v, 0)


/**
 * @brief 串口逻辑ID定义
 * 
 */
enum EXPORT_SERIAL
{
    SERIAL_CORE = 0,
    SERIAL_1,

    EXPORT_SERIAL_MAX_NUM,
};

/// SERIAL名称定义
#define SERIAL_NAMES \
"UART-Core\0" \
"UART-1\0"

#define SERIAL_NAME(v) stringAt(SERIAL_NAMES, v, 0)


/**
 * @brief 板上的EEPROM
 * 
 */
enum EXPORT_EEPROM
{
    EEPROM_MAIN = 0,

    EXPORT_EEPROM_MAX_NUM,
};

enum EXPORT_STORAGE
{
    STORAGE_TF = 0,

    EXPORT_STORAGE_MAX_NUM,
};


// #ifdef PROJECT_MEM_TRACE_ENABLE
// #include <mem_trace/mem_trace.h>

// #define MEM_ID_LIFTER  CONFIG_MEM_TRACE_USER_ID_BASE + 1
// #define MEM_ID_NEPTDEV  CONFIG_MEM_TRACE_USER_ID_BASE + 2
// #define MEM_ID_SACP_SERVER  CONFIG_MEM_TRACE_USER_ID_BASE + 3
// #endif


#endif // __EXPORT_IDS_H__
