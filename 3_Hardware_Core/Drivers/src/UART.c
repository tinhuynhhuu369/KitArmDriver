#include "UART.h"
#include <string.h>

UART_HandleTypeDef huart3;

static uint8_t rs485_rx_buf[RS485_RX_BUF_SIZE];
static volatile uint16_t rs485_rx_len = 0;
static volatile uint8_t  rs485_rx_done = 0;

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART MSP Initialization
  * This function configures the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART3)
  {
    /* USER CODE BEGIN USART3_MspInit 0 */

    /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    /* USER CODE BEGIN USART3_MspInit 1 */

    /* USER CODE END USART3_MspInit 1 */

  }

}

/**
  * @brief UART MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART3)
  {
    /* USER CODE BEGIN USART3_MspDeInit 0 */

    /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
    /* USER CODE BEGIN USART3_MspDeInit 1 */

    /* USER CODE END USART3_MspDeInit 1 */
  }

}

/**
  * @brief  MAX485 về chế độ thu (RS485_EN = 0) và bắt đầu nhận bằng ngắt.
  */
void RS485_Init(void)
{
  HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);
  HAL_UARTEx_ReceiveToIdle_IT(&huart3, rs485_rx_buf, RS485_RX_BUF_SIZE);
}

/**
  * @brief  Gửi dữ liệu qua RS485: RS485_EN = 1 -> gửi -> RS485_EN = 0.
  */
HAL_StatusTypeDef RS485_Send(const uint8_t *data, uint16_t len)
{
  HAL_StatusTypeDef st;

  HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
  st = HAL_UART_Transmit(&huart3, (uint8_t *)data, len, 100);  /* trả về khi đã gửi xong byte cuối (TC) */
  HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);

  return st;
}

/**
  * @brief  Lấy message vừa nhận (kết thúc khi đường truyền rảnh).
  * @retval Số byte nhận được, 0 nếu chưa có message mới.
  */
uint16_t RS485_Receive(uint8_t *buf, uint16_t max_len)
{
  uint16_t len;

  if (!rs485_rx_done)
  {
    return 0;
  }

  len = (rs485_rx_len < max_len) ? rs485_rx_len : max_len;
  memcpy(buf, rs485_rx_buf, len);
  rs485_rx_done = 0;

  HAL_UARTEx_ReceiveToIdle_IT(&huart3, rs485_rx_buf, RS485_RX_BUF_SIZE);  /* nhận message tiếp */
  return len;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART3)
  {
    rs485_rx_len  = Size;
    rs485_rx_done = 1;
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, rs485_rx_buf, RS485_RX_BUF_SIZE);  /* lỗi (overrun...) -> nhận lại */
  }
}
