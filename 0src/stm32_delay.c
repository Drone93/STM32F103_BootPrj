
//Uses SysTick timer. Call DelayInit() function before use

#include "stm32_top_header.h"	

static __IO uint32_t usTicks;	//

//SysTick handler will be called every 1 us
void SysTick_Handler()
{
	if(usTicks !=0) usTicks--;
}

void DelayInit()
{
	//Update system core clock value
	SystemCoreClockUpdate();
	//Configure the SysTick timer to overflow every 1 us
	SysTick_Config(SystemCoreClock/1000000);
}

void _delay_us(uint32_t us)
{
	DelayInit();		//Initialize delay functions (stm32_delay.c)
	//Reload us value
	usTicks = us;
	//Wait until usTick reach zero
	while(usTicks);
}

void _delay_ms(uint32_t ms)
{
	//Wait until ms reach zero
	while(ms--) _delay_us(1000);
}
