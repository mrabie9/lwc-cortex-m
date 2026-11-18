/* main.h for NUCLEO-N657X0-Q (Cortex-M55, STM32N657X0H3Q).
 * Used instead of Software/common/Inc/main.h when building with BOARD=m55. */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32n6xx_hal.h"

void Error_Handler(void);

/* Board init, implemented in common_m55/Src/board_m55.c */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART3_UART_Init(void); /* name kept from the M7 port; uses USART1 (ST-LINK VCP) */
void Board_CacheInit(void);

/* Set to 0 to run the M55 with I/D-caches off (see board_m55.c). */
#ifndef M55_ENABLE_CACHES
#define M55_ENABLE_CACHES 1
#endif

/* User LEDs on NUCLEO-N657X0-Q */
#define LD1_Pin GPIO_PIN_8   /* active low */
#define LD1_GPIO_Port GPIOG
#define LD2_Pin GPIO_PIN_10  /* active low */
#define LD2_GPIO_Port GPIOG

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
