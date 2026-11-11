#ifndef __DS3231_H
#define __DS3231_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"
#include "I2C.h"

/* Exported constants --------------------------------------------------------*/
/* 0x68 << 1 = 0xD0 */
#define DS3231_ADDR  (0x68U << 1)

/* Exported variables --------------------------------------------------------*/
typedef struct {
    uint8_t  seconds;    /* 0-59 */
    uint8_t  minutes;    /* 0-59 */
    uint8_t  hours;      /* 0-23 (che do 24h) */
    uint8_t  dayOfWeek;  /* 1-7, tu quy uoc (vd 1 = Chu nhat) */
    uint8_t  date;       /* 1-31 */
    uint8_t  month;      /* 1-12 */
    uint16_t year;       /* 2000-2099 */
} DS3231_Time;

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef DS3231_SetTime(I2C_HandleTypeDef *hi2c, const DS3231_Time *t);
HAL_StatusTypeDef DS3231_GetTime(I2C_HandleTypeDef *hi2c, DS3231_Time *t);

/* Nhiet do cam bien noi, do phan giai 0.25 do C */
HAL_StatusTypeDef DS3231_GetTemperature(I2C_HandleTypeDef *hi2c, float *tempC);

/* *lost = 1 neu dong ho da tung bi dung (het pin / chua cai gio) */
HAL_StatusTypeDef DS3231_LostPower(I2C_HandleTypeDef *hi2c, uint8_t *lost);

#ifdef __cplusplus
}
#endif

#endif /* __DS3231_H */