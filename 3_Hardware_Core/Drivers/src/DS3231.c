#include "DS3231.h"

#define I2C_TIMEOUT_MS   100U

#define DS3231_REG_TIME    0x00U /* 0x00-0x06: giay, phut, gio, thu, ngay, thang, nam */
#define DS3231_REG_STATUS  0x0FU
#define DS3231_REG_TEMP    0x11U
#define DS3231_STATUS_OSF  0x80U /* Oscillator Stop Flag */

/* DS3231 luu so o dang BCD: vd 45 giay -> 0x45 */
static uint8_t dec2bcd(uint8_t v) { return (uint8_t)(((v / 10U) << 4) | (v % 10U)); }
static uint8_t bcd2dec(uint8_t v) { return (uint8_t)(((v >> 4) * 10U) + (v & 0x0FU)); }

HAL_StatusTypeDef DS3231_SetTime(I2C_HandleTypeDef *hi2c, const DS3231_Time *t)
{
    HAL_StatusTypeDef st;
    uint8_t buf[7];
    uint8_t status;

    buf[0] = dec2bcd(t->seconds);
    buf[1] = dec2bcd(t->minutes);
    buf[2] = dec2bcd(t->hours) & 0x3FU;          /* bit 6 = 0: che do 24h */
    buf[3] = dec2bcd(t->dayOfWeek);
    buf[4] = dec2bcd(t->date);
    buf[5] = dec2bcd(t->month);
    buf[6] = dec2bcd((uint8_t)(t->year - 2000U));

    /* Ghi 7 thanh ghi lien tiep, con tro thanh ghi tu tang */
    st = HAL_I2C_Mem_Write(hi2c, DS3231_ADDR, DS3231_REG_TIME,
                           I2C_MEMADD_SIZE_8BIT, buf, sizeof(buf), I2C_TIMEOUT_MS);
    if (st != HAL_OK) {
        return st;
    }

    /* Xoa co OSF de danh dau gio da hop le */
    st = HAL_I2C_Mem_Read(hi2c, DS3231_ADDR, DS3231_REG_STATUS,
                          I2C_MEMADD_SIZE_8BIT, &status, 1U, I2C_TIMEOUT_MS);
    if (st != HAL_OK) {
        return st;
    }
    status &= (uint8_t)~DS3231_STATUS_OSF;
    return HAL_I2C_Mem_Write(hi2c, DS3231_ADDR, DS3231_REG_STATUS,
                             I2C_MEMADD_SIZE_8BIT, &status, 1U, I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef DS3231_GetTime(I2C_HandleTypeDef *hi2c, DS3231_Time *t)
{
    uint8_t buf[7];
    HAL_StatusTypeDef st;

    st = HAL_I2C_Mem_Read(hi2c, DS3231_ADDR, DS3231_REG_TIME,
                          I2C_MEMADD_SIZE_8BIT, buf, sizeof(buf), I2C_TIMEOUT_MS);
    if (st != HAL_OK) {
        return st;
    }

    t->seconds   = bcd2dec(buf[0] & 0x7FU);
    t->minutes   = bcd2dec(buf[1] & 0x7FU);
    t->hours     = bcd2dec(buf[2] & 0x3FU);      /* gia dinh che do 24h */
    t->dayOfWeek = bcd2dec(buf[3] & 0x07U);
    t->date      = bcd2dec(buf[4] & 0x3FU);
    t->month     = bcd2dec(buf[5] & 0x1FU);      /* bo bit the ky (bit 7) */
    t->year      = 2000U + bcd2dec(buf[6]);

    return HAL_OK;
}

HAL_StatusTypeDef DS3231_GetTemperature(I2C_HandleTypeDef *hi2c, float *tempC)
{
    uint8_t buf[2];
    HAL_StatusTypeDef st;

    st = HAL_I2C_Mem_Read(hi2c, DS3231_ADDR, DS3231_REG_TEMP,
                          I2C_MEMADD_SIZE_8BIT, buf, sizeof(buf), I2C_TIMEOUT_MS);
    if (st != HAL_OK) {
        return st;
    }

    /* buf[0]: phan nguyen co dau, 2 bit cao cua buf[1]: phan le 0.25 */
    int16_t raw = (int16_t)(((uint16_t)buf[0] << 8) | buf[1]);
    *tempC = (float)raw / 256.0f;

    return HAL_OK;
}

HAL_StatusTypeDef DS3231_LostPower(I2C_HandleTypeDef *hi2c, uint8_t *lost)
{
    uint8_t status;
    HAL_StatusTypeDef st;

    st = HAL_I2C_Mem_Read(hi2c, DS3231_ADDR, DS3231_REG_STATUS,
                          I2C_MEMADD_SIZE_8BIT, &status, 1U, I2C_TIMEOUT_MS);
    if (st != HAL_OK) {
        return st;
    }

    *lost = (status & DS3231_STATUS_OSF) ? 1U : 0U;
    return HAL_OK;
}