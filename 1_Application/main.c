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

/* Number of bytes clocked in per SPI debug read */
#define SPI_DEBUG_LEN   2U

/**
  * @brief  Clock SPI_DEBUG_LEN bytes on SPI1 and print what was received.
  *         printf goes to IAR Terminal I/O via semihosting.
  * @retval None
  */
static void SPI_DebugPrint(void)
{
  uint8_t rxBuf[SPI_DEBUG_LEN] = {0};
  HAL_StatusTypeDef status;

  HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_SET);
  status = HAL_SPI_Receive(&hspi1, rxBuf, SPI_DEBUG_LEN, 100);

  if (status != HAL_OK)
  {
    printf("[%lu] SPI error: status=%d, err=0x%lX\r\n",
           (unsigned long)HAL_GetTick(), (int)status, (unsigned long)hspi1.ErrorCode);
    return;
  }

  printf("[%lu] SPI RX:", (unsigned long)HAL_GetTick());
  for (uint32_t i = 0; i < SPI_DEBUG_LEN; i++)
  {
    printf(" 0x%02X", rxBuf[i]);
  }
  printf("\r\n");
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  HardwareInit();

  while (1)
  {
    /* USER CODE END WHILE */
    // HAL_GPIO_TogglePin(Y0_GPIO_Port, Y0_Pin);
    // HAL_GPIO_TogglePin(Y1_GPIO_Port, Y1_Pin);
    // HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);

    Button_Scan();
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
