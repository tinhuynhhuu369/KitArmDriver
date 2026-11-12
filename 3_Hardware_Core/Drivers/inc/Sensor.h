#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;

/* Exported functions prototypes ---------------------------------------------*/
void MX_ADC1_Init(void);
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H */