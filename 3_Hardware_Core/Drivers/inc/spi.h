#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

/* Exported functions prototypes ---------------------------------------------*/
void MX_DMA_Init(void);
void MX_SPI1_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H */