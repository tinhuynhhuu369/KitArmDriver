#include "led7seg.h"
#include <stdio.h>

/* ---- Bit trong U14 ---- */
#define U14_LED6    (1u << 0)
#define U14_LED7    (1u << 1)
#define U14_LED8    (1u << 2)
#define U14_COLON   (1u << 3)       /* active low */
#define U14_DIG3    (1u << 4)       /* LD_LED3, active low */
#define U14_DIG2    (1u << 5)       /* LD_LED2, active low */
#define U14_DIG1    (1u << 6)       /* LD_LED1, active low */
#define U14_DIG4    (1u << 7)       /* LD_LED4, active low */
#define U14_DIG_ALL (U14_DIG1 | U14_DIG2 | U14_DIG3 | U14_DIG4)

/* MSB - 0x[a][b]...[P] */
static const uint8_t segFont[LED7_FONT_COUNT] = {
    0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6,     /* 0..9 */
    0xEE, 0x3E, 0x9C, 0x7A, 0x9E, 0x8E,                             /* A, B, C, D, E, F */
    0x00,                                                           /* BLANK */
    0x02                                                            /* MINUS */
};

/* Vị trí 0..3 tính từ trái sang phải -> chân chọn digit trong U14 */
static const uint8_t digitSel[LED7_DIGIT_COUNT] = {
    U14_DIG1, U14_DIG2, U14_DIG3, U14_DIG4
};

static uint8_t buf[2] = {0xFF, 0xFF};
static volatile uint8_t segBuf[LED7_DIGIT_COUNT];
static volatile uint8_t extLeds;     /* LED6..8 */
static volatile bool    colonOn;
static uint8_t curDigit;

static void LED7_Write(uint8_t u14, uint8_t u15)
{
    uint8_t tx[2] = { u14, u15 };    /* byte đầu sẽ bị đẩy sang U14 */

    HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx, 2, 2);
    HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, GPIO_PIN_SET);   /* sườn lên ST_CP -> chốt */
}

void LED7_Clear(void)
{
    for (int i = 0; i < LED7_DIGIT_COUNT; i++) segBuf[i] = segFont[LED7_BLANK];
}

void LED7_Init(void)
{
    HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, GPIO_PIN_SET);

    curDigit = 0;
    extLeds = 0;
    colonOn = false;
    LED7_Clear();
    LED7_Write(U14_DIG_ALL | U14_COLON, 0xFF);   /* tắt hết */

    LED7_SetTime(0, 0);   /* 00:00 */
    LED7_SetColon(true);
}

void LED7_SetDigit(uint8_t pos, uint8_t value)
{
    if (pos >= LED7_DIGIT_COUNT || value >= LED7_FONT_COUNT) return;
    segBuf[pos] = segFont[value];
}

void LED7_SetColon(bool on)
{
    colonOn = on;
}

void LED7_SetTime(uint8_t mm, uint8_t ss)
{
    LED7_SetDigit(0, (uint8_t)((mm / 10) % 10));
    LED7_SetDigit(1, (uint8_t)(mm % 10));
    LED7_SetDigit(2, (uint8_t)((ss / 10) % 10));
    LED7_SetDigit(3, (uint8_t)(ss % 10));
}

/**
  * @brief  Quét 1 digit mỗi lần gọi (multiplex). Gọi đều đặn mỗi 1 ms
  *         -> 4 digit làm tươi 250 Hz, không nháy.
  */
void LED7_Scan(void)
{
    if (hspi1.Instance == NULL) return;     /* SPI chưa init */

    uint8_t u14 = U14_DIG_ALL | U14_COLON | extLeds;  /* mặc định: tắt digit, tắt colon */
    u14 &= (uint8_t)~digitSel[curDigit];              /* bật digit hiện tại */
    if (colonOn) u14 &= (uint8_t)~U14_COLON;

    uint8_t u15 = (uint8_t)~segBuf[curDigit];         /* anode chung -> đảo bit */

    LED7_Write(u14, u15);

    if (++curDigit >= LED7_DIGIT_COUNT) curDigit = 0;
}

void test_led7Seg(void)
{
    HAL_SPI_Transmit(&hspi1, buf, 2U, 100);
    printf("Transmitted 0xFFFF\n");
}