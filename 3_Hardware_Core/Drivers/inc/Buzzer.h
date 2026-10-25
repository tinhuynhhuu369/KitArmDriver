#ifndef __BUZZER_H
#define __BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM32F407ZET6.h"

/* Exported variables --------------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/
void Buzzer_Tone(uint32_t freq);
void Buzzer_Beep(uint32_t freq, uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H */