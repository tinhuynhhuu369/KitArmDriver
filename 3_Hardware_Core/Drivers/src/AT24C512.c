#include "AT24C512.h"

#define I2C_TIMEOUT_MS 100U

HAL_StatusTypeDef AT24C512_Write(I2C_HandleTypeDef *hi2c, uint16_t memAddr,
                                 const uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef st;

    if ((uint32_t)memAddr + len > AT24C512_SIZE) {
        return HAL_ERROR; /* vuot qua dung luong IC */
    }

    while (len > 0U) {
        /* So byte con trong trong trang hien tai.
           Khong duoc ghi vuot ranh gioi trang, neu khong dia chi se
           quay vong ve dau trang va ghi de du lieu cu. */
        uint16_t room  = AT24C512_PAGE_SIZE - (memAddr % AT24C512_PAGE_SIZE);
        uint16_t chunk = (len < room) ? len : room;

        /* START + 0xA0 + 2 byte dia chi o nho + du lieu + STOP */
        st = HAL_I2C_Mem_Write(hi2c, AT24C512_ADDR, memAddr,
                               I2C_MEMADD_SIZE_16BIT,
                               (uint8_t *)data, chunk, I2C_TIMEOUT_MS);
        if (st != HAL_OK) {
            return st;
        }

        /* Sau STOP, IC mat ~5 ms de ghi va khong tra ACK trong luc do.
           Goi lien tuc den khi IC tra ACK tro lai (ACK polling). */
        st = HAL_I2C_IsDeviceReady(hi2c, AT24C512_ADDR, 50U, 10U);
        if (st != HAL_OK) {
            return st;
        }

        memAddr += chunk;
        data    += chunk;
        len     -= chunk;
    }

    return HAL_OK;
}

HAL_StatusTypeDef AT24C512_Read(I2C_HandleTypeDef *hi2c, uint16_t memAddr,
                                uint8_t *data, uint16_t len)
{
    if ((uint32_t)memAddr + len > AT24C512_SIZE) {
        return HAL_ERROR;
    }

    /* Doc khong bi gioi han theo trang, doc lien tuc duoc */
    return HAL_I2C_Mem_Read(hi2c, AT24C512_ADDR, memAddr,
                            I2C_MEMADD_SIZE_16BIT,
                            data, len, I2C_TIMEOUT_MS + len);
}