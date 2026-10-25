#include "Button.h"

static uint8_t Pressed_Buttons[BUTTON_LEN] = {0};
static uint16_t BUTTON_MASK[BUTTON_COUNT] = 
{
    BTN_01,
    BTN_02,
    BTN_03,
    BTN_04,
    BTN_05,
    BTN_06,
    BTN_07,
    BTN_08,
    BTN_09,
    BTN_10,
    BTN_11,
    BTN_12,
    BTN_13,
    BTN_14,
    BTN_15,
    BTN_16 
};
void (*Pressed_Key_fhandle[BUTTON_COUNT])(void) = {NULL};

static uint16_t Button_GetRaw(void)
{
    return (uint16_t)~(((uint16_t)Pressed_Buttons[0] << 8) | Pressed_Buttons[1]);
}

void Button_Init(void)
{
    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
}

uint8_t Button_Scan(void)
{
    static uint16_t prevRaw = 0;    /* trạng thái lần quét trước, để chỉ bắt sườn nhấn */
    HAL_StatusTypeDef status;
    uint16_t newPress;

    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_SET);
    status = HAL_SPI_Receive(&hspi1, Pressed_Buttons, BUTTON_LEN, 10);

    if (status != HAL_OK)
    {
        printf("[%lu] SPI error: status=%d, err=0x%lX\r\n",
            (unsigned long)HAL_GetTick(), (int)status, (unsigned long)hspi1.ErrorCode);
        return 0;
    }

    /* Giữ nút thì không delay lại -> không làm khựng quét LED 7 đoạn */
    newPress = Button_GetRaw() & (uint16_t)~prevRaw;
    if (newPress == 0)
    {
        prevRaw = Button_GetRaw();
        return 0;
    }

    HAL_Delay(TIMEOUT_DEBOUNCE);

    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_SET);
    status = HAL_SPI_Receive(&hspi1, Pressed_Buttons, BUTTON_LEN, 10);
    if (status != HAL_OK) return 0;

    prevRaw = Button_GetRaw();

    for(uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
        if((newPress & prevRaw & BUTTON_MASK[i]) != 0)
        {
            return (uint8_t)(i + 1);
        }
    }
    return 0;
}