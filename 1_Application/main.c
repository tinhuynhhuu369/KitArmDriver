/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private defines -----------------------------------------------------------*/
/* EEPROM: vùng test bắt đầu trước ranh giới trang 0x1080 để kiểm tra việc chia trang */
#define EE_TEST_ADDR     0x1060U
#define EE_TEST_LEN      64U
/* 4 byte cuối IC dùng làm bộ đếm số lần khởi động (lưu qua mất điện) */
#define EE_BOOT_ADDR     ((uint16_t)(AT24C512_SIZE - 4U))

/* Toạ độ Y các dòng trên màn hình demo EEPROM + RTC */
#define DEMO_Y_EE_HDR    40
#define DEMO_Y_EE_BOOT   54
#define DEMO_Y_EE_TEST   66
#define DEMO_Y_RTC_HDR   88
#define DEMO_Y_RTC_STAT  102
#define DEMO_Y_RTC_TIME  122
#define DEMO_Y_RTC_DATE  156
#define DEMO_Y_RTC_TEMP  182

/* Toạ độ Y các dòng sensor ADC (nằm dưới phần RTC) */
#define DEMO_Y_ADC_HDR   208
#define DEMO_Y_ADC_TEMP  224
#define DEMO_Y_ADC_RES   244
#define DEMO_Y_ADC_LDR   264
#define DEMO_Y_ADC_RLDR  284

/* Message nhận qua RS485 để bật LED */
#define RS485_LED_MSG    "LED_ON"

/* Private variables ---------------------------------------------------------*/
static uint8_t rtc_ok = 0;          /* 1 nếu DS3231 phản hồi lúc khởi tạo */
static uint8_t rtc_last_sec = 0xFF; /* chỉ vẽ lại khi giây thay đổi */

static const char *const k_dow[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

/**
  * @brief  Vẽ một màn hình mẫu để test các hàm đồ hoạ LCD.
  * @retval None
  */
static void LCD_Demo(void)
{
  LCD_fill(LCD_BLACK);

  LCD_Write_String(10, 10, "KitArmDriver", LCD_WHITE, LCD_BLACK, 2);
  LCD_Write_String(10, 32, "LCD test: 0123456789", LCD_YELLOW, LCD_BLACK, 1);

  LCD_Draw_Line(0, 45, LCD_W - 1, 45, LCD_GRAY);
  LCD_Draw_Line(10, 55, 110, 115, LCD_CYAN);

  LCD_Draw_Rectangle(130, 55, 100, 60, LCD_GREEN);
  LCD_Fill_Rectangle(140, 65, 80, 40, LCD_BLUE);

  LCD_Draw_Circle(60, 175, 45, LCD_RED);
  LCD_Fill_Circle(60, 175, 25, LCD_ORANGE);

  LCD_Draw_Triangle(130, 220, 230, 220, 180, 130, LCD_MAGENTA);
  LCD_Fill_Triangle(150, 210, 210, 210, 180, 160, LCD_YELLOW);

  LCD_Write_String(10, 250, "Hello\nSTM32F407!", LCD_BLACK, LCD_WHITE, 3);
}

/**
  * @brief  Quét bus I2C1 (PB6 = SCL, PB7 = SDA) và hiển thị kết quả lên LCD.
  * @retval None
  */
static void I2C_Test(void)
{
  uint8_t found[I2C_ADDR_LAST - I2C_ADDR_FIRST + 1];
  char buf[32];
  int16_t count;
  int16_t i;
  int16_t y;

  LCD_fill(LCD_BLACK);
  LCD_Write_String(10, 10, "I2C1 Scan", LCD_WHITE, LCD_BLACK, 2);
  LCD_Write_String(10, 32, "SCL=PB6 SDA=PB7 100kHz", LCD_YELLOW, LCD_BLACK, 1);
  LCD_Draw_Line(0, 45, LCD_W - 1, 45, LCD_GRAY);

  count = I2C_Scan(&hi2c1, found, sizeof(found));

  if (count < 0)
  {
    LCD_Write_String(10, 55, "BUS BUSY!", LCD_RED, LCD_BLACK, 2);
    LCD_Write_String(10, 80, "Check pull-up / wiring", LCD_RED, LCD_BLACK, 1);
    return;
  }

  sprintf(buf, "Found: %d device(s)", count);
  LCD_Write_String(10, 55, buf, (count > 0) ? LCD_GREEN : LCD_RED, LCD_BLACK, 1);

  /* Mỗi địa chỉ một dòng: dạng 7-bit và dạng 8-bit (W/R) */
  y = 70;
  for (i = 0; i < count; i++)
  {
    sprintf(buf, "0x%02X  (W:0x%02X R:0x%02X)",
            found[i], (uint8_t)(found[i] << 1), (uint8_t)((found[i] << 1) | 1));
    LCD_Write_String(10, y, buf, LCD_CYAN, LCD_BLACK, 1);
    y += LCD_FONT_H + 2;
    if (y > LCD_H - LCD_FONT_H)
    {
      break;
    }
  }
}

/**
  * @brief  Demo EEPROM AT24C512:
  *         1. Bộ đếm số lần khởi động (reset board -> số tăng dần, chứng minh lưu được khi mất điện)
  *         2. Ghi 64 byte vắt qua ranh giới trang, đọc lại và so sánh từng byte
  * @retval None
  */
static void EEPROM_Demo(void)
{
  uint8_t tx[EE_TEST_LEN];
  uint8_t rx[EE_TEST_LEN];
  uint8_t cnt[4];
  uint32_t boot;
  char buf[32];
  HAL_StatusTypeDef st;
  uint16_t i;

  LCD_Write_String(10, DEMO_Y_EE_HDR, "AT24C512 EEPROM (0x50)", LCD_YELLOW, LCD_BLACK, 1);

  /* 1. Đọc - tăng - ghi lại bộ đếm khởi động (little-endian) */
  st = AT24C512_Read(&hi2c1, EE_BOOT_ADDR, cnt, sizeof(cnt));
  if (st != HAL_OK)
  {
    sprintf(buf, "Read error (st=%d)", st);
    LCD_Write_String(10, DEMO_Y_EE_BOOT, buf, LCD_RED, LCD_BLACK, 1);
    return;
  }
  boot = (uint32_t)cnt[0] | ((uint32_t)cnt[1] << 8) |
         ((uint32_t)cnt[2] << 16) | ((uint32_t)cnt[3] << 24);
  if (boot == 0xFFFFFFFFUL)
  {
    boot = 0;                       /* IC mới xuất xưởng: toàn bộ là 0xFF */
  }
  boot++;
  cnt[0] = (uint8_t)boot;
  cnt[1] = (uint8_t)(boot >> 8);
  cnt[2] = (uint8_t)(boot >> 16);
  cnt[3] = (uint8_t)(boot >> 24);
  st = AT24C512_Write(&hi2c1, EE_BOOT_ADDR, cnt, sizeof(cnt));

  if (st == HAL_OK)
  {
    sprintf(buf, "Boot count: %lu", (unsigned long)boot);
    LCD_Write_String(10, DEMO_Y_EE_BOOT, buf, LCD_WHITE, LCD_BLACK, 1);
  }
  else
  {
    sprintf(buf, "Boot count write err (st=%d)", st);
    LCD_Write_String(10, DEMO_Y_EE_BOOT, buf, LCD_RED, LCD_BLACK, 1);
  }

  /* 2. Mẫu dữ liệu đổi theo số lần boot -> chắc chắn đọc lại là dữ liệu vừa ghi */
  for (i = 0; i < EE_TEST_LEN; i++)
  {
    tx[i] = (uint8_t)(i * 7U + boot);
  }
  memset(rx, 0, sizeof(rx));

  st = AT24C512_Write(&hi2c1, EE_TEST_ADDR, tx, EE_TEST_LEN);
  if (st == HAL_OK)
  {
    st = AT24C512_Read(&hi2c1, EE_TEST_ADDR, rx, EE_TEST_LEN);
  }

  if (st != HAL_OK)
  {
    sprintf(buf, "R/W %uB @0x%04X: ERR st=%d", EE_TEST_LEN, EE_TEST_ADDR, st);
    LCD_Write_String(10, DEMO_Y_EE_TEST, buf, LCD_RED, LCD_BLACK, 1);
    return;
  }

  for (i = 0; (i < EE_TEST_LEN) && (tx[i] == rx[i]); i++)
  {
  }

  if (i == EE_TEST_LEN)
  {
    sprintf(buf, "R/W %uB @0x%04X: PASS", EE_TEST_LEN, EE_TEST_ADDR);
    LCD_Write_String(10, DEMO_Y_EE_TEST, buf, LCD_GREEN, LCD_BLACK, 1);
  }
  else
  {
    sprintf(buf, "FAIL @0x%04X: %02X != %02X", EE_TEST_ADDR + i, tx[i], rx[i]);
    LCD_Write_String(10, DEMO_Y_EE_TEST, buf, LCD_RED, LCD_BLACK, 1);
  }
}

/**
  * @brief  Tính thứ trong tuần (thuật toán Sakamoto).
  * @retval 0 = Chủ nhật ... 6 = Thứ bảy
  */
static uint8_t Day_Of_Week(uint16_t y, uint8_t m, uint8_t d)
{
  static const uint8_t t[12] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  if (m < 3U)
  {
    y--;
  }
  return (uint8_t)((y + y / 4U - y / 100U + y / 400U + t[m - 1U] + d) % 7U);
}

/**
  * @brief  Lấy ngày giờ lúc biên dịch (__DATE__ = "Nov 11 2026", __TIME__ = "20:07:45").
  * @retval None
  */
static void RTC_Get_Build_Time(DS3231_Time *t)
{
  static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  const char *d  = __DATE__;
  const char *tm = __TIME__;
  uint8_t m;

  for (m = 0; m < 12U; m++)
  {
    if (strncmp(&months[m * 3U], d, 3) == 0)
    {
      break;
    }
  }

  t->month   = (uint8_t)(m + 1U);
  t->date    = (uint8_t)(((d[4] == ' ') ? 0 : (d[4] - '0')) * 10 + (d[5] - '0'));
  t->year    = (uint16_t)((d[7] - '0') * 1000 + (d[8] - '0') * 100 +
                          (d[9] - '0') * 10 + (d[10] - '0'));
  t->hours   = (uint8_t)((tm[0] - '0') * 10 + (tm[1] - '0'));
  t->minutes = (uint8_t)((tm[3] - '0') * 10 + (tm[4] - '0'));
  t->seconds = (uint8_t)((tm[6] - '0') * 10 + (tm[7] - '0'));
  t->dayOfWeek = (uint8_t)(Day_Of_Week(t->year, t->month, t->date) + 1U); /* 1 = Chủ nhật */
}

/**
  * @brief  Khởi tạo demo RTC DS3231: nếu đồng hồ từng mất nguồn (cờ OSF)
  *         thì cài giờ theo thời điểm build firmware.
  * @retval None
  */
static void RTC_Demo_Init(void)
{
  DS3231_Time t;
  uint8_t lost;
  char buf[32];
  HAL_StatusTypeDef st;

  LCD_Write_String(10, DEMO_Y_RTC_HDR, "DS3231 RTC (0x68)", LCD_YELLOW, LCD_BLACK, 1);

  st = DS3231_LostPower(&hi2c1, &lost);
  if (st != HAL_OK)
  {
    sprintf(buf, "Not responding (st=%d)", st);
    LCD_Write_String(10, DEMO_Y_RTC_STAT, buf, LCD_RED, LCD_BLACK, 1);
    rtc_ok = 0;
    return;
  }

  if (lost)
  {
    RTC_Get_Build_Time(&t);
    st = DS3231_SetTime(&hi2c1, &t);
    LCD_Write_String(10, DEMO_Y_RTC_STAT,
                     (st == HAL_OK) ? "Lost power -> set build time" : "Set time error",
                     (st == HAL_OK) ? LCD_ORANGE : LCD_RED, LCD_BLACK, 1);
  }
  else
  {
    LCD_Write_String(10, DEMO_Y_RTC_STAT, "Clock running (battery OK)", LCD_GREEN, LCD_BLACK, 1);
  }

  rtc_ok = 1;
  rtc_last_sec = 0xFF;
}

/**
  * @brief  Đọc giờ + nhiệt độ từ DS3231 và vẽ lên LCD (chỉ vẽ khi giây thay đổi).
  *         Gọi định kỳ trong vòng lặp chính (ví dụ mỗi 200 ms).
  * @retval None
  */
static void RTC_Demo_Update(void)
{
  DS3231_Time t;
  float temp;
  int16_t t100;
  char buf[32];

  if (!rtc_ok)
  {
    return;
  }

  if (DS3231_GetTime(&hi2c1, &t) != HAL_OK)
  {
    LCD_Write_String(10, DEMO_Y_RTC_TIME, "Read error  ", LCD_RED, LCD_BLACK, 3);
    rtc_last_sec = 0xFF;
    return;
  }

  if (t.seconds == rtc_last_sec)
  {
    return;
  }
  rtc_last_sec = t.seconds;

  sprintf(buf, "%02u:%02u:%02u    ", t.hours, t.minutes, t.seconds);
  LCD_Write_String(10, DEMO_Y_RTC_TIME, buf, LCD_WHITE, LCD_BLACK, 3);

  sprintf(buf, "%s %02u/%02u/%04u",
          ((t.dayOfWeek >= 1U) && (t.dayOfWeek <= 7U)) ? k_dow[t.dayOfWeek - 1U] : "---",
          t.date, t.month, t.year);
  LCD_Write_String(10, DEMO_Y_RTC_DATE, buf, LCD_CYAN, LCD_BLACK, 2);

  /* In nhiệt độ bằng số nguyên x100: printf của IAR mặc định có thể không hỗ trợ %f */
  if (DS3231_GetTemperature(&hi2c1, &temp) == HAL_OK)
  {
    t100 = (int16_t)(temp * 100.0f);
    sprintf(buf, "Temp: %s%d.%02d C  ", (t100 < 0) ? "-" : "",
            ((t100 < 0) ? -t100 : t100) / 100, ((t100 < 0) ? -t100 : t100) % 100);
    LCD_Write_String(10, DEMO_Y_RTC_TEMP, buf, LCD_GREEN, LCD_BLACK, 2);
  }
}

/**
  * @brief  Vẽ phần tiêu đề của khối hiển thị sensor ADC.
  * @retval None
  */
static void Sensor_Demo_Init(void)
{
  LCD_Draw_Line(0, DEMO_Y_ADC_HDR - 6, LCD_W - 1, DEMO_Y_ADC_HDR - 6, LCD_GRAY);
  LCD_Write_String(10, DEMO_Y_ADC_HDR, "ADC sensors (DMA)", LCD_YELLOW, LCD_BLACK, 1);
}

/**
  * @brief  Đọc adc_buf (DMA) -> đổi ra đơn vị thực -> vẽ lên LCD.
  *         Kênh: [0]=LM35 nhiệt độ, [1]=biến trở, [2]=LDR.
  *         Gọi định kỳ trong vòng lặp chính (main gọi mỗi 1 s).
  * @retval None
  */
static void Sensor_Demo_Update(void)
{
  uint16_t raw[3];
  char buf[32];
  uint8_t i;

  /* Chụp nhanh buffer DMA để 3 kênh cùng một thời điểm */
  for (i = 0; i < 3U; i++)
  {
    raw[i] = adc_buf[i];
  }

  float v_temp  = raw[0] * VREF / ADC_MAX;
  float v_light = raw[2] * VREF / ADC_MAX;

  float temp_c  = v_temp * 100.0f;                 /* LM35: 10 mV/°C */
  float res_pct = raw[1] * 100.0f / ADC_MAX;       /* biến trở: 0-100 % */

  /* LDR: V = 3.3 * R_ldr / (R_ldr + 10k)  =>  R_ldr = 10k * V / (3.3 - V) */
  float r_ldr = (v_light < VREF - 0.05f)
              ? 10000.0f * v_light / (VREF - v_light)
              : 1e6f;                              /* rất tối */

  /* In số thực bằng số nguyên đã nhân sẵn (tránh phụ thuộc %f) */
  int32_t t10  = (int32_t)(temp_c * 10.0f + 0.5f);          /* 0.1 °C */
  int32_t p10  = (int32_t)(res_pct * 10.0f + 0.5f);         /* 0.1 %  */
  int32_t mv   = (int32_t)(v_light * 1000.0f + 0.5f);       /* mV     */
  int32_t rk10 = (int32_t)(r_ldr / 100.0f + 0.5f);          /* 0.1 kOhm */

  sprintf(buf, "Temp : %ld.%01ld C   ", (long)(t10 / 10), (long)(t10 % 10));
  LCD_Write_String(10, DEMO_Y_ADC_TEMP, buf, LCD_GREEN, LCD_BLACK, 2);

  sprintf(buf, "Res  : %ld.%01ld %%   ", (long)(p10 / 10), (long)(p10 % 10));
  LCD_Write_String(10, DEMO_Y_ADC_RES, buf, LCD_CYAN, LCD_BLACK, 2);

  sprintf(buf, "Light: %ld.%03ld V  ", (long)(mv / 1000), (long)(mv % 1000));
  LCD_Write_String(10, DEMO_Y_ADC_LDR, buf, LCD_ORANGE, LCD_BLACK, 2);

  if (r_ldr >= 1e6f)
  {
    sprintf(buf, "LDR  : >1000 k   ");
  }
  else
  {
    sprintf(buf, "LDR  : %ld.%01ld k   ", (long)(rk10 / 10), (long)(rk10 % 10));
  }
  LCD_Write_String(10, DEMO_Y_ADC_RLDR, buf, LCD_WHITE, LCD_BLACK, 2);
}

/**
  * @brief  Demo RS485 (USART3 + MAX485): nhận đúng message RS485_LED_MSG -> bật LED.
  *         Phía PC: USB-RS485, 115200 8N1.
  */
static void RS485_Demo(void)
{
  uint8_t rx[RS485_RX_BUF_SIZE];
  uint16_t len = RS485_Receive(rx, sizeof(rx));

  /* Bỏ CR/LF ở cuối nếu terminal tự thêm khi nhấn Enter */
  while ((len > 0U) && ((rx[len - 1U] == '\r') || (rx[len - 1U] == '\n')))
  {
    len--;
  }

  if ((len == sizeof(RS485_LED_MSG) - 1U) && (memcmp(rx, RS485_LED_MSG, len) == 0))
  {
    HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_SET);
    RS485_Send((const uint8_t *)"OK\r\n", 4);
  }
}

/**
  * @brief  Màn hình demo 2 thiết bị I2C: EEPROM AT24C512 + RTC DS3231.
  * @retval None
  */
static void I2C_Devices_Demo(void)
{
  LCD_fill(LCD_BLACK);
  LCD_Write_String(10, 10, "EEPROM + RTC", LCD_WHITE, LCD_BLACK, 2);
  LCD_Draw_Line(0, 30, LCD_W - 1, 30, LCD_GRAY);

  EEPROM_Demo();

  LCD_Draw_Line(0, 80, LCD_W - 1, 80, LCD_GRAY);
  RTC_Demo_Init();
  RTC_Demo_Update();
  Sensor_Demo_Init();
  Sensor_Demo_Update();
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  uint8_t btn;
  uint32_t rtc_tick;
  uint32_t sensor_tick;
  HardwareInit();
  LCD_Demo();
  HAL_Delay(2000);
  I2C_Test();
  HAL_Delay(2000);
  I2C_Devices_Demo();
  rtc_tick = HAL_GetTick();
  sensor_tick = rtc_tick;

  while (1)
  {
    /* Cập nhật đồng hồ mỗi 200 ms, chỉ vẽ lại khi sang giây mới */
    if ((HAL_GetTick() - rtc_tick) >= 200U)
    {
      rtc_tick = HAL_GetTick();
      RTC_Demo_Update();
    }

    /* Đọc lại sensor ADC mỗi 1 s */
    if ((HAL_GetTick() - sensor_tick) >= 1000U)
    {
      sensor_tick = HAL_GetTick();
      Sensor_Demo_Update();
    }

    RS485_Demo();

    /* USER CODE END WHILE */
    if (tim3_flag)
    {
      tim3_flag = 0;
      LED7_Scan();
    }
    if (tim3_1s_flag)
    {
      tim3_1s_flag = 0;
      LED7_ToggleExtLeds();   /* LED6, LED7, LED8 nhấp nháy mỗi 1 s */
    }
    btn = Button_Scan();
    if (btn != 0)
    {
      LED7_SetNumber(btn);    /* hiển thị số thứ tự nút vừa nhấn */
      Buzzer_Beep(10);
    }
  }
   
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
