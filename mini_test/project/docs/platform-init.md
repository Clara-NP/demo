# platformInit 做了什么

复位后 CPU 从 `Reset_Handler` 进入。它设置栈指针，调用 `SystemInit`，把初值从 Flash 拷到 RAM，清零 BSS，再调用 `main`。`main` 调用 `platformInit`。`platformInit` 返回后，`main` 停在空循环里。

`SystemInit` 不在 `platformInit` 里面。它只在使用浮点指令时打开 FPU。当前没有把向量表改到别的地址。

```text
复位
  -> Reset_Handler
       -> SystemInit
       -> 拷贝 .data，清零 .bss
       -> main
            -> platformInit
                 1. 可选：改向量表
                 2. HAL_Init
                 3. SystemClock_Config
                 4. SystemCoreClockUpdate
                 5. sysTickInit
            -> for (;;)
```

## 1. 改向量表

仅当定义了 `APP_ENTRY` 时执行：`SCB->VTOR = APP_ENTRY`。

程序放在 Flash 起始地址 `0x08000000` 时不需要这一步。启动文件里的向量表已经在这个地址，复位后 CPU 会从这里取中断入口。

程序被放在 Flash 后半段（例如前面留给引导程序）时必须做。否则中断仍跳到 `0x08000000` 的旧向量表。`APP_ENTRY` 填本程序向量表的实际地址，并且该地址要按芯片要求对齐。

## 2. HAL_Init

定义了 `CONFIG_STM32_HAL_ENABLE` 才调用。

它做四件事：

- 打开指令缓存、数据缓存和预取。取指和读常量更快。
- 把中断优先级分组设为 4 位抢占、0 位亚优先级。后面登记的中断都按这个分组解释。
- 按**当时**的 `SystemCoreClock` 配置 SysTick，打算得到 1 ms。此时时钟还是复位后的内部 16 MHz，所以这一拍是按 16 MHz 算的。
- 调用 `HAL_MspInit`。现在是空的弱函数，不初始化引脚。

这一步的作用是让 HAL 的中断优先级和时基有一个确定的起点。它不配置 PLL，也不知道板上有没有晶振。

`SystemClock_Config` 会在下一步复位各条总线上的外设。HAL 在这里做的外设状态会被清掉。SysTick 属于内核，不会被这次外设复位清掉，所以 16 MHz 下配好的重载值会一直留到第 5 步被改掉。

## 3. SystemClock_Config

先复位 APB1、APB2、AHB1、AHB2、AHB3 上的全部外设，再打开 SYSCFG 和 PWR 时钟。目的是丢掉上电或调试器可能留下的外设配置，从已知状态配时钟。

然后按编译宏选择时钟源。

| 板子 | 要定义的宏 | 实际走的函数 |
|---|---|---|
| 有 8 MHz 外部晶振 | `CONFIG_STM32_HSE_8M` | `setSystemClock_HSE_8Mhz` |
| 没有外部晶振，用芯片内部 RC | `CONFIG_STM32_HSI_16M` | `setSystemClock_HSI_16Mhz` |

两个都定义时只走 HSE。两个都不定义时编译会报警，运行时仍走 HSI。

`HSE_VALUE` 必须和晶振频率一致，当前按 8 000 000 Hz 检查。它是计算用的常数，不会去测量晶振。写错时，PLL 和后面算出的 `SystemCoreClock` 都会错，函数本身不一定报错。

### 8 MHz 晶振时的时钟

`setSystemClock_HSE_8Mhz` 把调压器设为 Scale 1，然后：

| 项目 | 取值 | 结果 |
|---|---|---|
| HSE | 8 MHz | 外部晶振 |
| PLLM | 4 | PLL 输入 2 MHz |
| PLLN | 180 | VCO 360 MHz |
| PLLP | 2 | SYSCLK 180 MHz |
| AHB 分频 | 1 | HCLK 180 MHz |
| APB1 分频 | 4 | PCLK1 45 MHz |
| APB2 分频 | 2 | PCLK2 90 MHz |
| Flash 等待 | 5 | 匹配 180 MHz |

晶振从 OSC_IN 直接送时钟、而不是接晶体时，需要旁路。当前调用使用 `LL_UTILS_HSEBYPASS_OFF`，只适用于晶体。

等 HSE 就绪的循环没有超时。晶振没焊、没起振或频率不对时，程序停在这里，`platformInit` 不会返回。

HSE 就绪后还要等 PLL 锁定，再把系统时钟切到 PLL。这两处也没有超时。当前这组分频在晶振正常时可以锁定。

F446 在 180 MHz 下还要求打开 over-drive。当前这段代码没有打开它。时钟切换可以完成，但 180 MHz 不在该电压模式的规格内。目标频率若降到规格允许的范围，或者补上 over-drive，才和手册一致。

### 没有晶振时

`setSystemClock_HSI_16Mhz` 用内部 16 MHz RC 做 PLL 源，目标同样是 180 MHz（PLLM 16，PLLN 360，PLLP 2）。内部 RC 有频率误差，波特率、USB 和需要准确时间的外设会受影响。不依赖板上的晶体，所以没有“等 HSE”这一步。

这块函数后面还有一段 PLLSAI 配置，和 USB 48 MHz 有关，不属于把系统时钟跑起来所必需的步骤。

## 4. SystemCoreClockUpdate

读 `RCC->CFGR` 和 `RCC->PLLCFGR`，把算出来的 HCLK 写入全局变量 `SystemCoreClock`。

复位后这个变量固定是 16 000 000。改完 PLL 若不更新，所有用它算波特率、定时器和延时的代码仍按 16 MHz 计算。

它使用 `HSE_VALUE` 或 `HSI_VALUE`，不测量真实频率。晶振是 8 MHz 但常数写成 25 MHz 时，变量是错的，硬件时钟也是错的。

## 5. sysTickInit

没有定义 `CONFIG_FREERTOS` 时，`sys_tick.c` 自己打开 SysTick，节拍默认 `CONFIG_SYSTICK_HZ = 1000`。

`SysTick_Config(SystemCoreClock / 1000)` 用**更新后**的内核时钟重装 SysTick，得到 1 ms 中断。这一步覆盖第 2 步按 16 MHz 留下的重载值。时钟切到 180 MHz 之后如果不重装，原来的 1 ms 会变成大约 0.09 ms。

中断里 `SysTick_Handler` 把 `s_sysTick` 加一。它没有调用 `HAL_IncTick()`。因此 HAL 自己的 `uwTick` 不增加，`HAL_Delay()` 会一直等。需要 HAL 毫秒延时时，应在这个中断里调用 `HAL_IncTick()`，或者改用这里的计数。

定义了 `CONFIG_FREERTOS` 时，这个文件不再配置 SysTick，节拍改由 RTOS 的移植层负责，避免两个地方抢同一个 SysTick。

## 返回时的状态

8 MHz 晶振起振且 PLL 锁定时，`platformInit` 返回。此时：

- SYSCLK / HCLK 为 180 MHz，APB1 为 45 MHz，APB2 为 90 MHz。
- `SystemCoreClock` 与 HCLK 一致。
- SysTick 每 1 ms 进一次中断，`s_sysTick` 递增。
- 各总线外设处于刚被复位后的关闭状态。引脚、串口、具体外设都还没初始化。
- `main` 进入空循环。

## 换一块板时要核对的项

- 晶振频率和有无晶振：决定 `CONFIG_STM32_HSE_8M` 或 `CONFIG_STM32_HSI_16M`，以及 `HSE_VALUE`。当前 HSE 路径只认 8 MHz。25 MHz 等其它频率不能只改宏，PLL 的 M、N、P 和 Flash 等待都要重算。
- 时钟是晶体还是外部方波：方波要开 HSE 旁路。
- 目标主频是否超出该芯片在当前电压模式下的上限。180 MHz 的 F446 需要 over-drive，当前未打开。
- 程序是否从 `0x08000000` 启动。不是的话要定义 `APP_ENTRY`。
- 是否使用 HAL 延时。使用的话要定义 `CONFIG_STM32_HAL_ENABLE`，并让 SysTick 中断更新 HAL 的 tick。
- 是否使用 FreeRTOS。使用的话不要再由 `sysTickInit` 占用 SysTick。
- 节拍是否仍要 1 ms。不是的话改 `CONFIG_SYSTICK_HZ`。
