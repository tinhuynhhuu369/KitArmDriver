#include "LCD.h"

#include <stdint.h>

#define LCD_CMD_ADDR   (*(volatile uint16_t *)0x60000000)  /* D/C = 0 */
#define LCD_DATA_ADDR  (*(volatile uint16_t *)0x60100000)  /* D/C = 1 (A19) */

#define LCD_W 240
#define LCD_H 320

static inline void LCD_CMD(uint8_t c)   { LCD_CMD_ADDR  = c; }
static inline void LCD_DATA(uint16_t d) { LCD_DATA_ADDR = d; }

/* Bảng khởi tạo: lệnh, số byte tham số, các tham số...
   Byte "số tham số" có bit 0x80 nghĩa là cần delay 150 ms sau lệnh. */
static const uint8_t init_seq[] = {
    0xCF, 3, 0x00, 0xC1, 0x30,          /* Power control B */
    0xED, 4, 0x64, 0x03, 0x12, 0x81,    /* Power on sequence */
    0xE8, 3, 0x85, 0x00, 0x78,          /* Driver timing A */
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02, /* Power control A */
    0xF7, 1, 0x20,                      /* Pump ratio */
    0xEA, 2, 0x00, 0x00,                /* Driver timing B */
    0xC0, 1, 0x23,                      /* Power control 1 */
    0xC1, 1, 0x10,                      /* Power control 2 */
    0xC5, 2, 0x3E, 0x28,                /* VCOM control 1 */
    0xC7, 1, 0x86,                      /* VCOM control 2 */
    0x36, 1, 0x48,                      /* MADCTL: hướng + BGR */
    0x3A, 1, 0x55,                      /* COLMOD: 16-bit RGB565 */
    0xB1, 2, 0x00, 0x18,                /* Frame rate ~79 Hz */
    0xB6, 3, 0x08, 0x82, 0x27,          /* Display function control */
    0xF2, 1, 0x00,                      /* Tắt 3-gamma */
    0x26, 1, 0x01,                      /* Gamma curve 1 */
    0xE0, 15, 0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,
              0x37,0x07,0x10,0x03,0x0E,0x09,0x00, /* Gamma + */
    0xE1, 15, 0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,
              0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F, /* Gamma - */
    0x11, 0x80,                         /* Sleep out + delay */
    0x29, 0x80,                         /* Display on + delay */
    0x00                                /* Kết thúc */
};

void LCD_init(void)
{
    /* Reset cứng: kéo RESET xuống ≥10 µs rồi chờ ≥120 ms */
    HAL_GPIO_WritePin(FSMC_RES_GPIO_Port, FSMC_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(FSMC_RES_GPIO_Port, FSMC_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(120);

    HAL_GPIO_WritePin(FSMC_BLK_GPIO_Port, FSMC_BLK_Pin, GPIO_PIN_SET);

    LCD_CMD(0x01);          /* Software reset */
    HAL_Delay(150);

    const uint8_t *p = init_seq;
    uint8_t cmd;

    // Init Sequence
    while ((cmd = *p++) != 0) {
        uint8_t n = *p++;
        LCD_CMD(cmd);
        for (uint8_t i = 0; i < (n & 0x7F); i++) LCD_DATA(*p++);
        if (n & 0x80) HAL_Delay(150);
    }
}

static void LCD_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CMD(0x2A);                          /* Column address set */
    LCD_DATA(x0 >> 8); LCD_DATA(x0 & 0xFF);
    LCD_DATA(x1 >> 8); LCD_DATA(x1 & 0xFF);
    LCD_CMD(0x2B);                          /* Page address set */
    LCD_DATA(y0 >> 8); LCD_DATA(y0 & 0xFF);
    LCD_DATA(y1 >> 8); LCD_DATA(y1 & 0xFF);
    LCD_CMD(0x2C);                          /* Memory write */
}

void LCD_fill(uint16_t color)
{
    LCD_set_window(0, 0, LCD_W - 1, LCD_H - 1);
    for (uint32_t i = 0; i < (uint32_t)LCD_W * LCD_H; i++) LCD_DATA(color);
}

/* Đọc ID để kiểm tra dây nối: kết quả đúng là 0x9341 */
uint16_t LCD_read_id(void)
{
    LCD_CMD(0xD3);
    (void)LCD_DATA_ADDR;                 /* dummy */
    (void)LCD_DATA_ADDR;                 /* 0x00 */
    uint16_t id = (LCD_DATA_ADDR & 0xFF) << 8;   /* 0x93 */
    id |= (LCD_DATA_ADDR & 0xFF);                /* 0x41 */
    return id;
}