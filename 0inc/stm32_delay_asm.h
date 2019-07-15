#ifndef __STM32_DELAY_ASM_H
#define __STM32_DELAY_ASM_H


#include "stm32f10x.h"

void _delay_us(uint16_t __us);
void _delay_ms(double __ms);
void delay_init(void);

#endif
