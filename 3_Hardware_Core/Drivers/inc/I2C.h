#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported constants --------------------------------------------------------*/
/* Dải địa chỉ 7-bit hợp lệ (0x00-0x07 và 0x78-0x7F là địa chỉ dành riêng) */
#define I2C_ADDR_FIRST   0x08
#define I2C_ADDR_LAST    0x77

/* Exported variables --------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported functions prototypes ---------------------------------------------*/
void MX_I2C1_Init(void);
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c);

/* Quét bus, ghi các địa chỉ 7-bit có ACK vào found[] (tối đa max_found phần tử).
   Trả về số thiết bị tìm thấy, hoặc -1 nếu bus bị treo (SDA/SCL bị kéo thấp). */
int16_t I2C_Scan(I2C_HandleTypeDef *hi2c, uint8_t *found, uint8_t max_found);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H */