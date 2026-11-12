#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern volatile uint16_t adc_buf[3];   /* [0]=TEMP, [1]=RES, [2]=LIGHT (DMA) */

#define VREF     3.3f
#define ADC_MAX  4095.0f

/* Exported functions prototypes ---------------------------------------------*/
void MX_ADC1_Init(void);
void MX_Sensor_Init(void);
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H */