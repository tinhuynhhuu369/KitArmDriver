#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported variables --------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported functions prototypes ---------------------------------------------*/
void MX_I2C1_Init(void);
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H */