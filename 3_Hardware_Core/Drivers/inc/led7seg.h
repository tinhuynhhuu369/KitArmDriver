#ifndef __LED_7SEG_H
#define __LED_7SEG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported functions prototypes ---------------------------------------------*/
#define TEST_DIGIT 0xFFFF
void test_led7Seg(void);
void LED7_Init(void);
void LED7_Scan(void);

#ifdef __cplusplus
}
#endif

#endif /* __LED_7SEG_H */