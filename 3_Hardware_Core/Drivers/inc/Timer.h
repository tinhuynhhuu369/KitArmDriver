#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported constants --------------------------------------------------------*/
#define TIM3_TICKS_PER_SEC  1000U   /* TIM3 tick = 1 ms */

/* Exported functions prototypes ---------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern volatile uint8_t tim3_flag;      /* set mỗi 1 ms */
extern volatile uint8_t tim3_1s_flag;   /* set mỗi 1 s  */

void MX_TIM3_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H */