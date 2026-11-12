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
#include "Buzzer.h"
#include "FSMC.h"
#include "LCD.h"
#include "I2C.h"
#include "AT24C512.h"
#include "DS3231.h"
#include "DS3231.h"
#include "AT24C512.h"
#include "Sensor.h"

extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_adc1;

/* Exported functions prototypes ---------------------------------------------*/
void HardwareInit(void);
void Error_Handler(void);
void SystemClock_Config(void);
void MX_DMA_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F407ZET6_H */