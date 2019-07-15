/*
HEX-donor program 
1 stm32 is configured as USB-SPI bridge and sends HEX data  
from terminal to the 2 stm that's configured as SPI slave (in bootloader mode, see stm32f103_bootloader_prj) 
and reads the data to rewrite the main program

Setup:
CMSIS->core
Device->Startup
StdPeriph->framework,SPI,GPIO,RCC, EXTI(USB)

//desc VID/PID 0483 5740

 EP3_OUT_Callback Changed!!!!!!!!!!!!!! (usb_endp.c, usb_lib.h), IMR_MSK in usb_conf
*/

//Defines
//#define VCOMPORT_IN_FRAME_INTERVAL 1  in usb_endp.c instead of 5
//Includes
#include "stm32_delay_asm.h"		//include _delay_us and _delay_ms
//#include "TFT_SPI.h"			//include func prototypes for TFT_SPI.c
#include "stdio.h"
#include "string.h"			//strlen,srncmp
//USB-specific libs
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#define RX_SIZE_LEN	4					//first bytes containing the file's size (in bytes, ASCII hex format)
//global vars

extern volatile uint8_t Receive_Buffer[64];		//VIRTUAL_COM_receive buffer
//uint8_t rx_buf[64];												//buffer to copy Receive_Buffer content into
extern volatile uint32_t Receive_length ;			//size of received data
//extern volatile uint32_t length ;
//uint8_t Send_Buffer[64];											
uint32_t packet_sent=1;									//EP1_IN callback (Device->host packet sent flag)
uint32_t packet_receive=1;							//EP3_OUT callback (Host->Device pacet sent flag)

_Bool err_flag = FALSE;		//USB ERROR flag 
_Bool pmaovr_flag = FALSE;		//PMA overrun flag
_Bool spi_upd = FALSE;	//SPI update flag
_Bool ep3_sof = FALSE;

char str_tx[40]= {'\0'};		//additional buffer to send back to the terminal
char rx_str[64] = {'\0'};		//additional buffer to copy char data from Receive_bUFFER

uint8_t byte_cnt=0;			//byte counter to count data from slave
uint32_t data_size = 0;
//protos
void spi1_master_init(void);		//initialize SPI1 as Master 
uint8_t spi1_sendbyte(uint8_t byte);
uint8_t search_string(char *s1, char *s2);					//search for s2 in s1
uint16_t AsciiToHex(char* buffer,uint8_t buf_len);
//void spi1_init_baudrate (uint16_t SPI_BaudRatePrescaler_x);

/*
SPI 1 pins:
PA4 - NSS
PA5 - SCK
PA6 - MISO
PA7 - MOSI

*/

/*
USB-CDC features (usb_prop.c)
		9600 baud rate
    0x00,   stop bits-1
    0x00,   parity - none
    0x08    no. of bits 8
*/
int main (void)
{
	
	//Init USB
	SystemInit();
	Set_System();
	
  Set_USBClock();
  USB_Interrupts_Config();
	
  USB_Init();
	
	//SystemInit();			//init system clock
	delay_init();			//init delay in stm32_delay_asm.c
	
	spi1_master_init();		//init spi1 in master mode

	_Bool eof_flag = FALSE;		//End of File flag
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13);					//LED off
	
	uint16_t ep_stat = GetEPRxStatus(ENDP3);
	 while (1)
  {
		
    if (bDeviceState == CONFIGURED)
    {//bulk transfer, buffer 64 bytes, large files split in several smaller ones
			ep_stat = GetEPRxStatus(ENDP3);
			
      CDC_Receive_DATA();		//packet_receive = 0 here
			
      // Check to see if we have data yet
      if(Receive_length)								//packet_receive = 1 (EP3_OUT_Callback)
      {
				/*
				sprintf(str_tx,"REC_LEN:%d\r\n",Receive_length);			//monitor receive_length
				while(!packet_sent){}
				CDC_Send_DATA ((uint8_t*)str_tx,strlen(str_tx));	//send byte counter to the terminal
				*/
				if(Receive_length<sizeof(Receive_Buffer)) {Receive_Buffer[Receive_length] = '\0';}	//clear the last character
				
				data_size += Receive_length;		//+ data_size until the end of file ('*') is reached 
				
				uint8_t eof = search_string((char*)Receive_Buffer,"*");	//search for end of file('*')
				if(eof) {eof_flag = TRUE;}		//if eof occured, raise flag
				
																								//SPI 
					if(spi_upd) 
					{
						GPIO_ResetBits(GPIOA,GPIO_Pin_4);			//NSS \__
						for(uint8_t i=0;i<Receive_length;i++)
						{
							byte_cnt = spi1_sendbyte(Receive_Buffer[i]);		//send buffer via SPI to the slave(as fast as possible) and save upcoming data to byte_cnt
						}
						GPIO_SetBits(GPIOA,GPIO_Pin_4);			//NSS __/
						
						//sprintf(str_tx,"BYTES:%d \r\n",(byte_cnt+1));		//byte counter monitor (the slave sends back an index of a byte, so +1) 
						//byte_cnt = 0;																	//clear counter
						spi_upd = FALSE;																//lower the flag
										
					}
					
					//SetEPRxValid(ENDP3);				//set endpoint as VALID
		
				//while(!packet_sent){}														//WAIT until usb isn't busy
				//CDC_Send_DATA ((uint8_t*)str_tx,strlen(str_tx));	//send byte counter to the terminal			
    	  Receive_length = 0;
				
				
      }
			else														//if Receive_length = 0
			{
				
				if(eof_flag)		//if there was some data and flag is set
				{
					sprintf(str_tx,"BYTES_SENT:%d\r\n",data_size); 					//data_size
					while(!packet_sent){}
						CDC_Send_DATA ((uint8_t*)str_tx,strlen(str_tx));	//Device->Host send data, packet_sent = 0
					
						data_size = 0;																//clear data counter
						eof_flag = FALSE;
						Receive_Buffer[0] = '\0';										//clear buffer	
				}
			}
			}
    }
  }

//search for s2 content in s1
uint8_t search_string(char *s1, char *s2)
{
	uint8_t n =0;
	uint8_t m =0;
	uint8_t times = 0;	//how many times s2 appears in s1
	uint8_t len = strlen(s2);		//length of s2
	
	while(s1[n] != '\0')
	{
		if(s1[n] == s2[m])	//if 1st character matches
		{
			//keep searching
			while(s1[n] == s2[m] && s1[n] != '\0')
			{
				n++;
				m++;
			}
			
			if(m == len) {times++;}	//if the len  matches the sequence, we find out string 2
		}
		else					//if 1st char doesn't match
		{
				n++;
				//if(s1[n] == '\0') return times;	
		}
		//n++;
		m=0;			//reset the counter
	}
	return times;
}

uint8_t spi1_sendbyte(uint8_t byte)
{
	uint8_t rec_data;		//received data
	//GPIO_ResetBits(GPIOA,GPIO_Pin_4);			//NSS \__
	while(!(SPI1->SR & SPI_SR_TXE)){}	//wait until TXE = 1
	SPI1->DR = byte;			//send byte 
	while(!(SPI1->SR & SPI_SR_RXNE)){}	//wait until RXNE = 1
	rec_data = SPI1->DR;	//CLEAR RXNE flag and read received data
	while((SPI1->SR & SPI_SR_BSY)){}	//wait until BSY = 0 
	//GPIO_SetBits(GPIOA,GPIO_Pin_4);			//NSS __/
		return (rec_data);							//read upcoming data 
}

void spi1_master_init(void)
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
	
	gpio.GPIO_Pin = GPIO_Pin_4;				//Choose 4 pin for NSS
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;			//Out_PP
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);			//Initialize port A with this settings
	
	gpio.GPIO_Pin = GPIO_Pin_6;				//Choose 6 pin for MISO
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;			//Input AF OD
	//gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);			//Initialize port A with this settings
	
	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;	//Choose 7,5 pins for SCK, MOSI
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;			//Alternate function PP
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);			//Initialize port A with this settings
	
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//Direction of SPI - full duplex (both MISO, MOSI are used)
	spi.SPI_Mode = SPI_Mode_Master;												//master mode
	spi.SPI_DataSize = SPI_DataSize_8b;								//8-bit data length
	spi.SPI_CPOL = SPI_CPOL_Low;											//CPOL = 0
	spi.SPI_CPHA = SPI_CPHA_1Edge;									//CPHA = 0
	spi.SPI_NSS = SPI_NSS_Soft;										//NSS by soft (PA4)
	spi.SPI_FirstBit = SPI_FirstBit_MSB;					//MSB first
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//Baud rate - max speed
	SPI_Init(SPI1, &spi);			//Init SPI with this settings
	
	SPI_CalculateCRC(SPI1,DISABLE);			//disable CRC calculation for SPI1
	
	SPI1->CR2 |=SPI_CR2_SSOE;	//SSOE = 1 (enable NSS)
	
	SPI_Cmd(SPI1, ENABLE);		//enable SPI1
}

void spi1_init_baudrate (uint16_t SPI_BaudRatePrescaler_x)
{
	SPI_Cmd(SPI1, DISABLE);
	
	SPI_InitTypeDef spi;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//Direction of SPI - full duplex (both MISO, MOSI are used)
	spi.SPI_Mode = SPI_Mode_Master;												//master mode
	spi.SPI_DataSize = SPI_DataSize_8b;								//8-bit data length
	spi.SPI_CPOL = SPI_CPOL_Low;											//CPOL = 0
	spi.SPI_CPHA = SPI_CPHA_1Edge;									//CPHA = 0
	spi.SPI_NSS = SPI_NSS_Soft;										//NSS by soft (PA4)
	spi.SPI_FirstBit = SPI_FirstBit_MSB;					//MSB first
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_x;		//Baud rate - max speed
	SPI_Init(SPI1, &spi);			//Init SPI with this settings
	
	SPI1->CR2 |=SPI_CR2_SSOE;	//SSOE = 1 (enable NSS)
	
	SPI_Cmd(SPI1, ENABLE);		//enable SPI1
}

uint16_t AsciiToHex(char* buffer,uint8_t buf_len)
{
	uint8_t var;
	uint16_t result = 0;
	
	for(uint8_t i=0;i<buf_len;i++)
	{	
	var = (uint8_t)buffer[i];
	var >>= 4;
	result<<=4;
	if(var==3)
	{
		result += buffer[i]-(var<<4);
	}
	else if (var==4)
	{
		result += 9 + (buffer[i]-(var<<4));
	}
	}
	return result;
}

void DOVR_Callback(void)	//PMA OVERRUN Callback (called from USB_ISTR() interrupt handler in usb_istr.c)(USB_ISTR reg)
{
	_SetEPRxStatus(ENDP3,EP_RX_NAK);		
	pmaovr_flag = TRUE;
	sprintf(str_tx,"OVERRUN:%d\r\n",data_size);
	CDC_Send_DATA ((uint8_t*)str_tx,strlen(str_tx));
	pmaovr_flag = FALSE;
	data_size = 0;
	
}

void ERR_Callback(void)		//ERROR Callback (called from USB_ISTR() interrupt handler in usb_istr.c)
{
	err_flag = TRUE;
}

void SOF_Callback (void)		//SOF callback (called from USB_ISTR() interrupt handler in usb_istr.c)
{
	uint16_t istr = _GetISTR();		//get ISTR
	
	if((istr & ISTR_EP_ID) == ENDP3) {ep3_sof = TRUE;}		//if SOF is for ENDP3
}