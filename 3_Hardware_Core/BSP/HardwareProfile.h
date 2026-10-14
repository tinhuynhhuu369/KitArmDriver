#ifndef __HARDWAREPROFILE_H
#define __HARDWAREPROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define Y0_Pin GPIO_PIN_5
#define Y0_GPIO_Port GPIOE
#define Y1_Pin GPIO_PIN_6
#define Y1_GPIO_Port GPIOE
#define DEBUG_LED_Pin GPIO_PIN_3
#define DEBUG_LED_GPIO_Port GPIOA
#define INPUT_X0_Pin GPIO_PIN_6
#define INPUT_X0_GPIO_Port GPIOA
#define INPUT_X0_EXTI_IRQn EXTI9_5_IRQn
#define INPUT_X1_Pin GPIO_PIN_7
#define INPUT_X1_GPIO_Port GPIOA
#define INPUT_X1_EXTI_IRQn EXTI9_5_IRQn
#define INPUT_X2_Pin GPIO_PIN_4
#define INPUT_X2_GPIO_Port GPIOC
#define INPUT_X2_EXTI_IRQn EXTI4_IRQn
#define INPUT_X3_Pin GPIO_PIN_5
#define INPUT_X3_GPIO_Port GPIOC
#define INPUT_X3_EXTI_IRQn EXTI9_5_IRQn

#ifdef __cplusplus
}
#endif

#endif /* __HARDWAREPROFILE_H */