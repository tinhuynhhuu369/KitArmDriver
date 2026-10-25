#include "Buzzer.h"

#define TIM_CNT_FREQ 1000000UL   // PSC = 71 với clock 72 MHz

void Buzzer_Tone(uint32_t freq)
{
    if (freq == 0) {             // nốt lặng
        __HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 0);
        return;
    }
    uint32_t arr = TIM_CNT_FREQ / freq - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim13, arr);
    __HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, (arr + 1) / 2);
}

void Buzzer_Beep(uint32_t freq, uint32_t ms)
{
    Buzzer_Tone(freq);
    HAL_Delay(ms);
    Buzzer_Tone(0);
}
