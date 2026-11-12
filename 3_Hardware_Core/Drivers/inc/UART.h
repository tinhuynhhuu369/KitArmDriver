#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported constants --------------------------------------------------------*/
#define RS485_RX_BUF_SIZE   64U

/* Exported variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart3;

/* Exported functions prototypes ---------------------------------------------*/
void MX_USART3_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);

void RS485_Init(void);
HAL_StatusTypeDef RS485_Send(const uint8_t *data, uint16_t len);
uint16_t RS485_Receive(uint8_t *buf, uint16_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */