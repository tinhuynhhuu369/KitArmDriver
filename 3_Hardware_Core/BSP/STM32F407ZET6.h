#ifndef __STM32F407ZET6_H
#define __STM32F407ZET6_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "HardwareProfile.h"
#include "GPIO.h"
#include "spi.h"
#include "Timer.h"
#include "led7seg.h"
#include "Button.h"

/* Exported functions prototypes ---------------------------------------------*/
void HardwareInit(void);
void Error_Handler(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F407ZET6_H */