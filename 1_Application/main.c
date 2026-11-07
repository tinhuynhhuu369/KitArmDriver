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
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  uint8_t btn;
  HardwareInit();
  LCD_Demo();

  while (1)
  {
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
