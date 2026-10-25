#ifndef __LED_7SEG_H
#define __LED_7SEG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"
#include <stdbool.h>

/* Exported constants --------------------------------------------------------*/
#define LED7_DIGIT_COUNT    4U
#define LED7_FONT_COUNT     18U
#define LED7_BLANK          16U     /* index trong segFont */
#define LED7_MINUS          17U

/* Exported functions prototypes ---------------------------------------------*/
#define TEST_DIGIT 0xFFFF
void test_led7Seg(void);
void LED7_Init(void);
void LED7_Clear(void);
void LED7_SetDigit(uint8_t pos, uint8_t value);
void LED7_SetColon(bool on);
void LED7_SetTime(uint8_t mm, uint8_t ss);
void LED7_Scan(void);

#ifdef __cplusplus
}
#endif

#endif /* __LED_7SEG_H */