//Delay for stm32f103c8 based on asm loop. Compiler ver.5 only. Yet
//Call SystemCoreClockUpdate function if the core clock is changing before using this delay
//Updated  23.12.18, us works better 
#include "stm32_delay_asm.h"

uint8_t sys_clk_var = 0;
uint32_t __ticks;

__asm void _delay_loop(uint32_t __count)		//asm function, executing in about 3 clocks (??)
{
	//unsigned long r0;
	//decrement:
loop
			 SUBS	r0, r0, #1			
			 BNE		loop							
			 BX			lr
}
//Delay initialization. Call once in the beginning of the program
void delay_init(void)
{
	SystemCoreClockUpdate();	//
	sys_clk_var = (SystemCoreClock)/6e6;
}

void _delay_us(uint16_t __us)
{
	//uint32_t __ticks;		//
	//double __tmp =  sys_clk_var * __us;
	__ticks = sys_clk_var * __us;	//(uint32_t)__tmp;
	_delay_loop(__ticks);
}

void _delay_ms(double __ms)
{	
	//uint32_t __ticks;
	double __tmp = sys_clk_var * 1000 * __ms;
	__ticks = (uint32_t)__tmp;
	_delay_loop(__ticks);
}
