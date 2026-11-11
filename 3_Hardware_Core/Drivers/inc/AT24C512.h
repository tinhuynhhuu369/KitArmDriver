#ifndef __AT24C512_H
#define __AT24C512_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"
#include "I2C.h"

/* Exported constants --------------------------------------------------------*/
#define AT24C512_ADDR       (0x50U << 1)
#define AT24C512_PAGE_SIZE  128U
#define AT24C512_SIZE       65536UL

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/* Ghi len byte bat dau tu o nho memAddr (tu dong chia theo trang 128 byte) */
HAL_StatusTypeDef AT24C512_Write(I2C_HandleTypeDef *hi2c, uint16_t memAddr,
                                 const uint8_t *data, uint16_t len);
 
/* Doc len byte bat dau tu o nho memAddr */
HAL_StatusTypeDef AT24C512_Read(I2C_HandleTypeDef *hi2c, uint16_t memAddr,
                                uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __AT24C512_H */