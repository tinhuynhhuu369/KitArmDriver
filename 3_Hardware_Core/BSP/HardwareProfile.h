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
#define BTN_LOAD_Pin GPIO_PIN_3
#define BTN_LOAD_GPIO_Port GPIOD

#ifdef __cplusplus
}
#endif

#endif /* __HARDWAREPROFILE_H */