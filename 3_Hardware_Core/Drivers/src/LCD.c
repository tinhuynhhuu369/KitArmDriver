#include "LCD.h"

#include <stdint.h>

#define LCD_CMD_ADDR   (*(volatile uint16_t *)0x60000000)  /* D/C = 0 */
#define LCD_DATA_ADDR  (*(volatile uint16_t *)0x60100000)  /* D/C = 1 (A19) */

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

    LCD_fill(0x0000);
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

/* ------------------------------ Hình học --------------------------------- */

#define SWAP16(a, b) do { int16_t t_ = (a); (a) = (b); (b) = t_; } while (0)

void LCD_Draw_Pixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || y < 0 || x >= LCD_W || y >= LCD_H) return;
    LCD_set_window(x, y, x, y);
    LCD_DATA(color);
}

/* Tô hình chữ nhật, tự cắt phần nằm ngoài màn hình */
void LCD_Fill_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (w <= 0 || h <= 0) return;

    int32_t x0 = x, y0 = y;
    int32_t x1 = x0 + w - 1, y1 = y0 + h - 1;

    if (x0 >= LCD_W || y0 >= LCD_H || x1 < 0 || y1 < 0) return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= LCD_W) x1 = LCD_W - 1;
    if (y1 >= LCD_H) y1 = LCD_H - 1;

    LCD_set_window(x0, y0, x1, y1);
    for (uint32_t n = (uint32_t)(x1 - x0 + 1) * (y1 - y0 + 1); n; n--) LCD_DATA(color);
}

static void LCD_hline(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    LCD_Fill_Rectangle(x, y, w, 1, color);
}

static void LCD_vline(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    LCD_Fill_Rectangle(x, y, 1, h, color);
}

/* Bresenham; đường ngang/dọc dùng đường tô nhanh */
void LCD_Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    if (y0 == y1) {
        if (x0 > x1) SWAP16(x0, x1);
        LCD_hline(x0, y0, x1 - x0 + 1, color);
        return;
    }
    if (x0 == x1) {
        if (y0 > y1) SWAP16(y0, y1);
        LCD_vline(x0, y0, y1 - y0 + 1, color);
        return;
    }

    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y1 > y0) ? (y0 - y1) : (y1 - y0);   /* dy <= 0 */
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = dx + dy;

    for (;;) {
        LCD_Draw_Pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int32_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void LCD_Draw_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (w <= 0 || h <= 0) return;
    LCD_hline(x, y, w, color);
    LCD_hline(x, y + h - 1, w, color);
    LCD_vline(x, y, h, color);
    LCD_vline(x + w - 1, y, h, color);
}

/* Thuật toán midpoint circle */
void LCD_Draw_Circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    if (r < 0) return;

    int16_t f = 1 - r, ddx = 1, ddy = -2 * r;
    int16_t x = 0, y = r;

    LCD_Draw_Pixel(x0, y0 + r, color);
    LCD_Draw_Pixel(x0, y0 - r, color);
    LCD_Draw_Pixel(x0 + r, y0, color);
    LCD_Draw_Pixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) { y--; ddy += 2; f += ddy; }
        x++; ddx += 2; f += ddx;

        LCD_Draw_Pixel(x0 + x, y0 + y, color);
        LCD_Draw_Pixel(x0 - x, y0 + y, color);
        LCD_Draw_Pixel(x0 + x, y0 - y, color);
        LCD_Draw_Pixel(x0 - x, y0 - y, color);
        LCD_Draw_Pixel(x0 + y, y0 + x, color);
        LCD_Draw_Pixel(x0 - y, y0 + x, color);
        LCD_Draw_Pixel(x0 + y, y0 - x, color);
        LCD_Draw_Pixel(x0 - y, y0 - x, color);
    }
}

void LCD_Fill_Circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    if (r < 0) return;

    int16_t f = 1 - r, ddx = 1, ddy = -2 * r;
    int16_t x = 0, y = r;

    LCD_hline(x0 - r, y0, 2 * r + 1, color);

    while (x < y) {
        if (f >= 0) { y--; ddy += 2; f += ddy; }
        x++; ddx += 2; f += ddx;

        LCD_hline(x0 - x, y0 + y, 2 * x + 1, color);
        LCD_hline(x0 - x, y0 - y, 2 * x + 1, color);
        LCD_hline(x0 - y, y0 + x, 2 * y + 1, color);
        LCD_hline(x0 - y, y0 - x, 2 * y + 1, color);
    }
}

void LCD_Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color)
{
    LCD_Draw_Line(x0, y0, x1, y1, color);
    LCD_Draw_Line(x1, y1, x2, y2, color);
    LCD_Draw_Line(x2, y2, x0, y0, color);
}

/* Quét từng hàng: sắp đỉnh theo y, nội suy x trên cạnh dài (0->2) và hai cạnh ngắn */
void LCD_Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color)
{
    if (y0 > y1) { SWAP16(y0, y1); SWAP16(x0, x1); }
    if (y1 > y2) { SWAP16(y1, y2); SWAP16(x1, x2); }
    if (y0 > y1) { SWAP16(y0, y1); SWAP16(x0, x1); }

    if (y0 == y2) {                     /* cả 3 đỉnh cùng hàng */
        int16_t a = x0, b = x0;
        if (x1 < a) a = x1; else if (x1 > b) b = x1;
        if (x2 < a) a = x2; else if (x2 > b) b = x2;
        LCD_hline(a, y0, b - a + 1, color);
        return;
    }

    int32_t dx01 = x1 - x0, dy01 = y1 - y0;
    int32_t dx02 = x2 - x0, dy02 = y2 - y0;
    int32_t dx12 = x2 - x1, dy12 = y2 - y1;

    for (int16_t y = y0; y <= y2; y++) {
        int16_t a, b;

        b = x0 + dx02 * (y - y0) / dy02;
        if (y < y1)        a = x0 + dx01 * (y - y0) / dy01;
        else if (dy12 > 0) a = x1 + dx12 * (y - y1) / dy12;
        else               a = x1;      /* đáy nằm ngang: y == y1 == y2 */

        if (a > b) SWAP16(a, b);
        LCD_hline(a, y, b - a + 1, color);
    }
}

/* ------------------------------- Chữ ------------------------------------- */

/* Font 5x7 ASCII 0x20..0x7E, mỗi ký tự 5 cột, bit 0 là hàng trên cùng */
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* ' ' */
    {0x00,0x00,0x5F,0x00,0x00}, /* !   */
    {0x00,0x07,0x00,0x07,0x00}, /* "   */
    {0x14,0x7F,0x14,0x7F,0x14}, /* #   */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* $   */
    {0x23,0x13,0x08,0x64,0x62}, /* %   */
    {0x36,0x49,0x55,0x22,0x50}, /* &   */
    {0x00,0x05,0x03,0x00,0x00}, /* '   */
    {0x00,0x1C,0x22,0x41,0x00}, /* (   */
    {0x00,0x41,0x22,0x1C,0x00}, /* )   */
    {0x08,0x2A,0x1C,0x2A,0x08}, /* *   */
    {0x08,0x08,0x3E,0x08,0x08}, /* +   */
    {0x00,0x50,0x30,0x00,0x00}, /* ,   */
    {0x08,0x08,0x08,0x08,0x08}, /* -   */
    {0x00,0x60,0x60,0x00,0x00}, /* .   */
    {0x20,0x10,0x08,0x04,0x02}, /* /   */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0   */
    {0x00,0x42,0x7F,0x40,0x00}, /* 1   */
    {0x42,0x61,0x51,0x49,0x46}, /* 2   */
    {0x21,0x41,0x45,0x4B,0x31}, /* 3   */
    {0x18,0x14,0x12,0x7F,0x10}, /* 4   */
    {0x27,0x45,0x45,0x45,0x39}, /* 5   */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 6   */
    {0x01,0x71,0x09,0x05,0x03}, /* 7   */
    {0x36,0x49,0x49,0x49,0x36}, /* 8   */
    {0x06,0x49,0x49,0x29,0x1E}, /* 9   */
    {0x00,0x36,0x36,0x00,0x00}, /* :   */
    {0x00,0x56,0x36,0x00,0x00}, /* ;   */
    {0x08,0x14,0x22,0x41,0x00}, /* <   */
    {0x14,0x14,0x14,0x14,0x14}, /* =   */
    {0x00,0x41,0x22,0x14,0x08}, /* >   */
    {0x02,0x01,0x51,0x09,0x06}, /* ?   */
    {0x32,0x49,0x79,0x41,0x3E}, /* @   */
    {0x7E,0x11,0x11,0x11,0x7E}, /* A   */
    {0x7F,0x49,0x49,0x49,0x36}, /* B   */
    {0x3E,0x41,0x41,0x41,0x22}, /* C   */
    {0x7F,0x41,0x41,0x22,0x1C}, /* D   */
    {0x7F,0x49,0x49,0x49,0x41}, /* E   */
    {0x7F,0x09,0x09,0x09,0x01}, /* F   */
    {0x3E,0x41,0x49,0x49,0x7A}, /* G   */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H   */
    {0x00,0x41,0x7F,0x41,0x00}, /* I   */
    {0x20,0x40,0x41,0x3F,0x01}, /* J   */
    {0x7F,0x08,0x14,0x22,0x41}, /* K   */
    {0x7F,0x40,0x40,0x40,0x40}, /* L   */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* M   */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N   */
    {0x3E,0x41,0x41,0x41,0x3E}, /* O   */
    {0x7F,0x09,0x09,0x09,0x06}, /* P   */
    {0x3E,0x41,0x51,0x21,0x5E}, /* Q   */
    {0x7F,0x09,0x19,0x29,0x46}, /* R   */
    {0x46,0x49,0x49,0x49,0x31}, /* S   */
    {0x01,0x01,0x7F,0x01,0x01}, /* T   */
    {0x3F,0x40,0x40,0x40,0x3F}, /* U   */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V   */
    {0x3F,0x40,0x38,0x40,0x3F}, /* W   */
    {0x63,0x14,0x08,0x14,0x63}, /* X   */
    {0x07,0x08,0x70,0x08,0x07}, /* Y   */
    {0x61,0x51,0x49,0x45,0x43}, /* Z   */
    {0x00,0x7F,0x41,0x41,0x00}, /* [   */
    {0x02,0x04,0x08,0x10,0x20}, /* \   */
    {0x00,0x41,0x41,0x7F,0x00}, /* ]   */
    {0x04,0x02,0x01,0x02,0x04}, /* ^   */
    {0x40,0x40,0x40,0x40,0x40}, /* _   */
    {0x00,0x01,0x02,0x04,0x00}, /* `   */
    {0x20,0x54,0x54,0x54,0x78}, /* a   */
    {0x7F,0x48,0x44,0x44,0x38}, /* b   */
    {0x38,0x44,0x44,0x44,0x20}, /* c   */
    {0x38,0x44,0x44,0x48,0x7F}, /* d   */
    {0x38,0x54,0x54,0x54,0x18}, /* e   */
    {0x08,0x7E,0x09,0x01,0x02}, /* f   */
    {0x0C,0x52,0x52,0x52,0x3E}, /* g   */
    {0x7F,0x08,0x04,0x04,0x78}, /* h   */
    {0x00,0x44,0x7D,0x40,0x00}, /* i   */
    {0x20,0x40,0x44,0x3D,0x00}, /* j   */
    {0x7F,0x10,0x28,0x44,0x00}, /* k   */
    {0x00,0x41,0x7F,0x40,0x00}, /* l   */
    {0x7C,0x04,0x18,0x04,0x78}, /* m   */
    {0x7C,0x08,0x04,0x04,0x78}, /* n   */
    {0x38,0x44,0x44,0x44,0x38}, /* o   */
    {0x7C,0x14,0x14,0x14,0x08}, /* p   */
    {0x08,0x14,0x14,0x18,0x7C}, /* q   */
    {0x7C,0x08,0x04,0x04,0x08}, /* r   */
    {0x48,0x54,0x54,0x54,0x20}, /* s   */
    {0x04,0x3F,0x44,0x40,0x20}, /* t   */
    {0x3C,0x40,0x40,0x20,0x7C}, /* u   */
    {0x1C,0x20,0x40,0x20,0x1C}, /* v   */
    {0x3C,0x40,0x30,0x40,0x3C}, /* w   */
    {0x44,0x28,0x10,0x28,0x44}, /* x   */
    {0x0C,0x50,0x50,0x50,0x3C}, /* y   */
    {0x44,0x64,0x54,0x4C,0x44}, /* z   */
    {0x00,0x08,0x36,0x41,0x00}, /* {   */
    {0x00,0x00,0x7F,0x00,0x00}, /* |   */
    {0x00,0x41,0x36,0x08,0x00}, /* }   */
    {0x08,0x04,0x08,0x10,0x08}, /* ~   */
};

/* Vẽ cả ô ký tự (kể cả nền) trong một cửa sổ để ghi liên tục cho nhanh */
void LCD_Write_Char(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg, uint8_t size)
{
    if (size == 0) size = 1;
    if (c < 0x20 || c > 0x7E) c = '?';

    int32_t w = (int32_t)LCD_FONT_W * size;
    int32_t h = (int32_t)LCD_FONT_H * size;
    if (x < 0 || y < 0 || x + w > LCD_W || y + h > LCD_H) return;

    const uint8_t *glyph = font5x7[c - 0x20];

    LCD_set_window(x, y, x + w - 1, y + h - 1);
    for (uint8_t row = 0; row < LCD_FONT_H; row++) {
        for (uint8_t sy = 0; sy < size; sy++) {
            for (uint8_t col = 0; col < LCD_FONT_W; col++) {
                uint8_t bits = (col < 5) ? glyph[col] : 0;
                uint16_t color = ((bits >> row) & 1) ? fg : bg;
                for (uint8_t sx = 0; sx < size; sx++) LCD_DATA(color);
            }
        }
    }
}

/* Hỗ trợ '\n' và tự xuống dòng khi chạm mép phải */
void LCD_Write_String(int16_t x, int16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t size)
{
    if (size == 0) size = 1;
    int16_t cx = x;

    while (*str) {
        if (*str == '\n' || cx + LCD_FONT_W * size > LCD_W) {
            cx = x;
            y += LCD_FONT_H * size;
            if (*str == '\n') { str++; continue; }
        }
        LCD_Write_Char(cx, y, *str++, fg, bg, size);
        cx += LCD_FONT_W * size;
    }
}