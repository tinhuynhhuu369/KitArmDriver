#ifndef __FSMC_H
#define __FSMC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported functions prototypes ---------------------------------------------*/
extern SRAM_HandleTypeDef hsram1;

void MX_FSMC_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __FSMC_H */