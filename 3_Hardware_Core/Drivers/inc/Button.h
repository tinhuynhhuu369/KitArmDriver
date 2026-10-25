#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Number of bytes to read input */
#define BUTTON_COUNT   16U
#define BUTTON_LEN   2U
#define TIMEOUT_DEBOUNCE 20U

#define BTN_01 (1 << 0)
#define BTN_02 (1 << 1)
#define BTN_03 (1 << 2)
#define BTN_04 (1 << 3)
#define BTN_05 (1 << 4)
#define BTN_06 (1 << 5)
#define BTN_07 (1 << 6)
#define BTN_08 (1 << 7)
#define BTN_09 (1 << 8)
#define BTN_10 (1 << 9)
#define BTN_11 (1 << 10)
#define BTN_12 (1 << 11)
#define BTN_13 (1 << 12)
#define BTN_14 (1 << 13)
#define BTN_15 (1 << 14)
#define BTN_16 (1 << 15)

/* Exported functions prototypes ---------------------------------------------*/
void Button_Init(void);
uint8_t Button_Scan(void);   /* trả về số thứ tự nút vừa nhấn (1..16), 0 nếu không có */

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */