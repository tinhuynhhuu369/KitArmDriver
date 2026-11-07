#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported constants --------------------------------------------------------*/
#define LCD_W 240
#define LCD_H 320

/* Màu RGB565 */
#define LCD_BLACK    0x0000
#define LCD_WHITE    0xFFFF
#define LCD_RED      0xF800
#define LCD_GREEN    0x07E0
#define LCD_BLUE     0x001F
#define LCD_YELLOW   0xFFE0
#define LCD_CYAN     0x07FF
#define LCD_MAGENTA  0xF81F
#define LCD_ORANGE   0xFD20
#define LCD_GRAY     0x8410

/* Ô ký tự font 5x7 (5 cột chữ + 1 cột cách, 7 hàng chữ + 1 hàng cách) */
#define LCD_FONT_W   6
#define LCD_FONT_H   8

/* Exported functions prototypes ---------------------------------------------*/
void LCD_init(void);
void LCD_fill(uint16_t color);
uint16_t LCD_read_id(void);

void LCD_Draw_Pixel(int16_t x, int16_t y, uint16_t color);
void LCD_Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void LCD_Draw_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void LCD_Fill_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void LCD_Draw_Circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void LCD_Fill_Circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void LCD_Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color);
void LCD_Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color);

/* size = 1 -> ô 6x8 pixel, size = 2 -> 12x16, ... Ký tự tràn màn hình sẽ bị bỏ qua. */
void LCD_Write_Char(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg, uint8_t size);
void LCD_Write_String(int16_t x, int16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */