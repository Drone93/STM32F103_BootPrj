
//Bootloader program for stm32f103c8
/*
Settings:
Opt. for Target-> Target-> IROM1 = 0x8018000, size=0x8000 (size 32Kbyte for bootloader)
Opt. for Target-> Linker-> Use memory layout from target dialog -> check
Opt. for Target-> C/C++ include paths->0inc , STM32 USB-CDC libs, 0inc->inc

Setup:
CMSIS->core
Device->Startup
StdPeriph->framework,SPI,GPIO,RCC

*/
/*
SPI 1 pins:
PA4 - NSS
PA5 - SCK
PA6 - MISO
PA7 - MOSI

*/

//Defines
#define SYSCLK_FREQ_72MHZ		//system clock to 72 mhz
#define MAIN_BASE_ADR 0x08000000		//main program base address
#define BOOT_BASE_ADR 0x08018000		//bootloader base address (size = 0x8000 - 32 KB)

//Includes
#include "stm32_delay_asm.h"		//include _delay_us and _delay_ms
#include "TFT_SPI.h"			//include func prototypes for TFT_SPI.c

static uint8_t RX_BUF[64];		//rx buffer for spi
void spi1_slave_init(void);		//SPI1 slave mode

static uint8_t byte_cnt = 0;		//byte counter for spi incoming data
//static uint8_t tx_cnt =0;				//tx counter
//static _Bool rxne_flag = 0;
///////////////////////

int main (void)
{
	//SystemInit();			//init system clock
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, BOOT_BASE_ADR);		//replace IRQ vector table (for SPI) 
	delay_init();			//init delay in stm32_delay_asm.c

	tft_clear();			//clear tft screen
	tft_w("BOOTLOADER");
	tft_newl();
	
	spi1_slave_init();				//init spi1 as slave
	GPIO_SetBits(GPIOC,GPIO_Pin_13);	//LED off
	tft_w("SPI1 READY");
	tft_newl();
	
	//while(!(SPI1->SR & SPI_SR_TXE)){}		//wait until TXE=1				
		//SPI1->DR = byte_cnt;
		
	while(1)
	{
	if((GPIOA->IDR & GPIO_Pin_4))		//if SPI not busy and byte_cnt !=0
	{
		//GPIO_ResetBits(GPIOC,GPIO_Pin_13);		//LED on
	/*	
	for (uint8_t i=0;i<byte_cnt;i++)
	{
		tft_w_ascii(RX_BUF[i]); 
	}
	*/
	/*
	if(RX_BUF[0])
	{
	tft_hex(byte_cnt);								
	tft_newl();		
	}
	*/
		
	if(RX_BUF[0] == '*') {tft_clear();}		//'*' clears display
		
	RX_BUF[0] = '\0';		//end of line
		
	byte_cnt = 0;
	SPI1->DR = byte_cnt;	//clear shift reg
	//tx_cnt = 0;
	//GPIO_SetBits(GPIOC,GPIO_Pin_13);		//LED off
}
}
	//NVIC_SystemReset();		//return to main program 


	
}

void spi1_slave_init(void)
{
	
	//set(AFIO->MAPR,AFIO_MAPR_SPI1_REMAP);	//remap spi1 to A15-B5
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);	
	
	//SPI1 init:
	SPI_InitTypeDef spi;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);		//Enable clock to SPI 1
	GPIO_InitTypeDef gpio;
	
	gpio.GPIO_Pin = GPIO_Pin_13;				//PC13
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;			
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;				//Choose 4,5,6,7 pins for NSS,SCK,MISO,MOSI
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;			//AF_OD
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);			//Initialize port A with these settings
	
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//Direction of SPI - full duplex (both MISO, MOSI are used)
	spi.SPI_Mode = SPI_Mode_Slave;												//master mode
	spi.SPI_DataSize = SPI_DataSize_8b;								//8-bit data length
	spi.SPI_CPOL = SPI_CPOL_Low;											//CPOL = 0
	spi.SPI_CPHA = SPI_CPHA_1Edge;									//CPHA = 0
	spi.SPI_NSS = SPI_NSS_Hard;										//NSS hard
	spi.SPI_FirstBit = SPI_FirstBit_MSB;					//MSB first
	//spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;		//Baud rate 
	SPI_Init(SPI1, &spi);			//Init SPI with this settings
	
	SPI_CalculateCRC(SPI1,DISABLE);			//disable CRC calculation for SPI1
	
	SPI1->CR2 |=SPI_CR2_SSOE;	//SSOE = 1 (enable NSS)
	
	__enable_irq();			//ENABLE global IRQ
	NVIC_EnableIRQ(SPI1_IRQn);		//enable SPI1 interrupts
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);		//enable RXNE IRQ for spi1
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);		//enable TXE IRQ
	
	SPI_Cmd(SPI1, ENABLE);		//enable SPI1
}

void SPI1_IRQHandler(void)		//interrup handler for SPI1
{
	
	if(SPI1->SR & SPI_SR_TXE)				//if TXE=1
	{
		SPI1->DR = (byte_cnt+1);			//send byte_cnt back to master (clear TXE) (the first value to be sent back is 0, so +1)
		//GPIO_ResetBits(GPIOC,GPIO_Pin_13);		//LED on
		
	}
	
	if((SPI1->SR & SPI_SR_RXNE))		//if RXNE=1
	{	
		RX_BUF[byte_cnt] = (SPI1->DR);		//read incoming byte from SPI Rx (clear RXNE flag)		
		byte_cnt++;
		//SPI1->DR = byte_cnt;			//send byte conter back (clear TXE)
		//GPIO_SetBits(GPIOC,GPIO_Pin_13);	//LED off
	}
	
}