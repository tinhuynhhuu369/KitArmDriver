#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported functions prototypes ---------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern volatile uint8_t tim3_flag;

void MX_TIM3_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H */