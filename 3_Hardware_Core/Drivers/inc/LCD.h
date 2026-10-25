#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported functions prototypes ---------------------------------------------*/
void LCD_init(void);
void LCD_fill(uint16_t color);
uint16_t LCD_read_id(void);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */