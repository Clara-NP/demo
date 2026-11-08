/**
 * @file gpio_pins.h
 * @brief 定义一个抽象的，与平台无关的GPIO引脚定义
 **/

#ifndef __GPIO_PINS_H__
#define __GPIO_PINS_H__


/**
 * @addtogroup gpiopins GPIO引脚定义
 * @ingroup soc_gpio
 * @{ 
 * 
 * @brief GPIO引脚定义
 * 
 * @details 
 * 
 * GPIO的逻辑索引定义，目前大部分的MCU引脚是使用8位或16的，可通过以下宏配置16位的端口模式
 * 
 *    CONFIG_GPIO_PORT_WIDTH_16  使用16位的端口模式 <br>
 *    CONFIG_GPIO_PORTEFG        启用 PORTE － PORTG，（默认定义了PORTA－PORTD）<br>
 *   
 */

/**
 * @brief 定义GPIO索引使用的类型，一般不会超过256
 * 
 */
typedef unsigned char  gpio_t;

/// 定义一个无效的GPIO
#define  GPIO_NONE  255

// /**
//  * @brief 定义SOC GPIO 模式
//  * 
//  */
// typedef enum
// {
//     _GPIO_MODE_INPUT = 0,
//     _GPIO_MODE_INPUT_PULLUP,
//     _GPIO_MODE_INPUT_PULLDOWN,
//     _GPIO_MODE_OUTPUT,

//     // 扩展模式，非GPIO模块使用
//     _GPIO_MODE_INPUT_LOWSPEED,
//     _GPIO_MODE_OUTPUT_LOWSPEED,
// } gpioMode_t;


// /**
//  * @brief 定义SOC GPIO的中断模式
//  * 
//  */
// typedef enum
// {
//     _GPIO_INTERRUPT_DISABLED = 0,
//     _GPIO_INTERRUPT_RISINGEDGE = 1,
//     _GPIO_INTERRUPT_FALLINGEDGE = 2,
//     _GPIO_INTERRUPT_BOTHEDGE = 3,
// }gpioInterruptMode_t;

// /**
//  * @brief GPIO中断的函数指针
//  * 
//  */
// typedef void (*gpioInterruptHandle_t)(int gpio); 

// /**
//  * @brief GPIO 端口定义
//  * 
//  */
// enum GPIO_PORT
// {
//     _PORTA = 0,
//     _PORTB,
//     _PORTC,
//     _PORTD,
//     _PORTE,
//     _PORTF,
//     _PORTG,
//     _PORTH,
//     _PORTI,
//     _PORTJ,
//     _PORTK,
// };


#ifdef CONFIG_GPIO_PORT_WIDTH_16

/**
 * @brief GPIO引脚定义
 * 
 */
enum GPIO_PIN
{
  _PA0 = 0,
  _PA1,
  _PA2,
  _PA3,
  _PA4,
  _PA5,
  _PA6,
  _PA7,
  _PA8,
  _PA9,
  _PA10,
  _PA11,
  _PA12,
  _PA13,
  _PA14,
  _PA15,  
  _PB0,
  _PB1,
  _PB2,
  _PB3,
  _PB4,
  _PB5,
  _PB6,
  _PB7,
  _PB8,
  _PB9,
  _PB10,
  _PB11,
  _PB12,
  _PB13,
  _PB14,
  _PB15,
  _PC0,
  _PC1,
  _PC2,
  _PC3,
  _PC4,
  _PC5,
  _PC6,
  _PC7,
  _PC8,
  _PC9,
  _PC10,
  _PC11,
  _PC12,
  _PC13,
  _PC14,
  _PC15,
  _PD0,
  _PD1,
  _PD2,
  _PD3,
  _PD4,
  _PD5,
  _PD6,
  _PD7,
  _PD8,
  _PD9,
  _PD10,
  _PD11,
  _PD12,
  _PD13,
  _PD14,
  _PD15, 
  #ifdef CONFIG_GPIO_PORTEFG
  _PE0,
  _PE1,
  _PE2,
  _PE3,
  _PE4,
  _PE5,
  _PE6,
  _PE7,
  _PE8,
  _PE9,
  _PE10,
  _PE11,
  _PE12,
  _PE13,
  _PE14,
  _PE15,
  _PF0,
  _PF1,
  _PF2,
  _PF3,
  _PF4,
  _PF5,
  _PF6,
  _PF7,
  _PF8,
  _PF9,
  _PF10,
  _PF11,
  _PF12,
  _PF13,
  _PF14,
  _PF15,
  _PG0,
  _PG1,
  _PG2,
  _PG3,
  _PG4,
  _PG5,
  _PG6,
  _PG7,
  _PG8,
  _PG9,
  _PG10,
  _PG11,
  _PG12,
  _PG13,
  _PG14,
  _PG15,
  #endif  /* CONFIG_GPIO_PORTEFG */ 
  #ifdef CONFIG_GPIO_PORTHIJK
  _PH0,
  _PH1,
  _PH2,
  _PH3,
  _PH4,
  _PH5,
  _PH6,
  _PH7,
  _PH8,
  _PH9,
  _PH10,
  _PH11,
  _PH12,
  _PH13,
  _PH14,
  _PH15,
  _PI0,
  _PI1,
  _PI2,
  _PI3,
  _PI4,
  _PI5,
  _PI6,
  _PI7,
  _PI8,
  _PI9,
  _PI10,
  _PI11,
  _PI12,
  _PI13,
  _PI14,
  _PI15,
  _PJ0,
  _PJ1,
  _PJ2,
  _PJ3,
  _PJ4,
  _PJ5,
  _PJ6,
  _PJ7,
  _PJ8,
  _PJ9,
  _PJ10,
  _PJ11,
  _PJ12,
  _PJ13,
  _PJ14,
  _PJ15,
  _PK0,
  _PK1,
  _PK2,
  _PK3,
  _PK4,
  _PK5,
  _PK6,
  _PK7,
  _PK8,
  _PK9,
  _PK10,
  _PK11,
  _PK12,
  _PK13,
  _PK14,
  _PK15,  
  #endif  /* CONFIG_GPIO_PORTHIJK */ 
  GPIO_END
};

#else // CONFIG_GPIO_PORT_WIDTH_8

/**
 * @brief GPIO引脚定义
 * 
 */
enum GPIO_PIN
{
  _PA0 = 0,
  _PA1,
  _PA2,
  _PA3,
  _PA4,
  _PA5,
  _PA6,
  _PA7,
  _PB0,
  _PB1,
  _PB2,
  _PB3,
  _PB4,
  _PB5,
  _PB6,
  _PB7,
  _PC0,
  _PC1,
  _PC2,
  _PC3,
  _PC4,
  _PC5,
  _PC6,
  _PC7,
  _PD0,
  _PD1,
  _PD2,
  _PD3,
  _PD4,
  _PD5,
  _PD6,
  _PD7,
  #ifdef CONFIG_GPIO_PORTEFG
  _PE0,
  _PE1,
  _PE2,
  _PE3,
  _PE4,
  _PE5,
  _PE6,
  _PE7,
  _PF0,
  _PF1,
  _PF2,
  _PF3,
  _PF4,
  _PF5,
  _PF6,
  _PF7,
  _PG0,
  _PG1,
  _PG2,
  _PG3,
  _PG4,
  _PG5,
  _PG6,
  _PG7,
  #endif  /* CONFIG_GPIO_PORTEFG */ 
  #ifdef CONFIG_GPHO_PORTHIJK
  _PH0,
  _PH1,
  _PH2,
  _PH3,
  _PH4,
  _PH5,
  _PH6,
  _PH7,
  _PI0,
  _PI1,
  _PI2,
  _PI3,
  _PI4,
  _PI5,
  _PI6,
  _PI7,
  _PJ0,
  _PJ1,
  _PJ2,
  _PJ3,
  _PJ4,
  _PJ5,
  _PJ6,
  _PJ7,
  _PK0,
  _PK1,
  _PK2,
  _PK3,
  _PK4,
  _PK5,
  _PK6,
  _PK7,
  #endif  /* CONFIG_GPHO_PORTHIJK */   
  GPIO_END
};

#endif /* not CONFIG_GPIO_PORT_WIDTH_16 */

/**
 * @} End of Group
 * 
 */

#endif /* __GPIO_PINS_H__ */

