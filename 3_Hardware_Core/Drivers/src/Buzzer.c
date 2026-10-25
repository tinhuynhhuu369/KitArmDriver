#include "Buzzer.h"

#define TIM_CNT_FREQ 1000000UL

static void Buzzer_On(void)  { HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1); }
static void Buzzer_Off(void) { HAL_TIM_PWM_Stop(&htim13, TIM_CHANNEL_1); }

void Buzzer_Beep(uint32_t ms)
{
    Buzzer_On();
    HAL_Delay(ms);
    Buzzer_Off();
}

static void Buzzer_Tone(uint32_t freq)
{
    if (freq == 0) {             // nốt lặng
        __HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 0);
        return;
    }
    uint32_t arr = TIM_CNT_FREQ / freq - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim13, arr);
    __HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, (arr + 1) / 2);
}

static void Buzzer_Beep_Melody(uint32_t freq, uint32_t ms)
{
    Buzzer_Tone(freq);
    HAL_Delay(ms);
    Buzzer_Tone(0);
}

void Buzzer_Play_Melody(void)
{
    Buzzer_On();

    uint16_t melody[] = {262, 294, 330, 349, 392, 440, 494, 523};

    for (int i = 0; i < 8; i++) {
        Buzzer_Beep_Melody(melody[i], 300);
        HAL_Delay(50);              
    }

    Buzzer_Off();
}