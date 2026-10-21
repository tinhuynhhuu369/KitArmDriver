#include "Button.h"

static uint16_t Pressed_Buttons[BUTTON_LEN] = {0};
static uint16_t BUTTON_ARRAY[BUTTON_COUNT] = 
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
static void (*Pressed_Key_fhandle[16](void)) = {NULL};

void Button_Init(void)
{
    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
}

void Button_Scan(void)
{
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_SET);
    status = HAL_SPI_Receive(&hspi1, Pressed_Buttons, BUTTON_LEN, 10);

    if (status != HAL_OK)
    {
        printf("[%lu] SPI error: status=%d, err=0x%lX\r\n",
            (unsigned long)HAL_GetTick(), (int)status, (unsigned long)hspi1.ErrorCode);
        return;
    }

    for(uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
        
        if((Pressed_Buttons[i] & BUTTON_ARRAY[i]) != 0)
        {
            HAL_Delay(TIMEOUT_DEBOUNCE);

            HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, GPIO_PIN_SET);
            status = HAL_SPI_Receive(&hspi1, Pressed_Buttons, BUTTON_LEN, 10);

            if((Pressed_Buttons[i] & BUTTON_ARRAY[i]) != 0)
            {
                if (Pressed_Key_fhandle[i] != NULL) Pressed_Key_fhandle[i]();
            }
        }
    }
}