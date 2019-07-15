/*
Setup:
CMSIS->core
Device->Startup
StdPeriph->framework,SPI,GPIO,RCC, EXTI(USB)

//desc VID/PID 0483 5740
*/
#define BOOT_BASE_ADR 0x08018000		//bootloader base address (size = 0x8000 - 32 KB)

static void (*jumpto)(void);				//jump to specific address prototype
void boot_jump (void);							//jump to bootloader (related to BOOT_BASE_ADR)
//Includes
#include "stm32_delay_asm.h"		//include _delay_us and _delay_ms
#include "TFT_SPI.h"			//include func prototypes for TFT_SPI.c
#include "stdio.h"
#include "string.h"			//strlen,srncmp
//USB-specific libs
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"


//global vars
extern volatile uint8_t Receive_Buffer[64];		//VIRTUAL_COM_receive buffer
extern volatile uint32_t Receive_length ;			//size of received data
//extern volatile uint32_t length ;
//uint8_t Send_Buffer[64];											
uint32_t packet_sent=1;									//USB flags (hw_config, usb_endp)
uint32_t packet_receive=1;

char str_tx[20]= {'\0'};		//additional buffer to send back to the terminal
char rx_str[10] = {'\0'};		//additional buffer to copy char data from Receive_bUFFER


int main (void)
{
	
	
	//Init USB
	Set_System();
  //Set_USBClock();
  //USB_Interrupts_Config();
  //USB_Init();
	
	//SystemInit();			//init system clock
	delay_init();			//init delay in stm32_delay_asm.c
	tft_init_defaults();		//init TFT display with defaults(see TFT_SPI.c)
	tft_w("TEST_PROGRAM");
	tft_newl();
	tft_hex(*(volatile unsigned long*)(BOOT_BASE_ADR+0x4));		//check if there's something other than FFFFFFFF
	tft_newl();
	
	

	/*
	_Bool boot_flag = FALSE;	//enter/not enter boot flag
	 while (!boot_flag)
  {
    if (bDeviceState == CONFIGURED)
    {
      CDC_Receive_DATA();
      // Check to see if we have data yet
      if(Receive_length > 0)								//
      {
				Receive_Buffer[Receive_length] = '\0';			//mark the last buffer element as end of line
				for(uint8_t s_cnt=0;s_cnt<Receive_length+1;s_cnt++)
				{
					rx_str[s_cnt] = Receive_Buffer[s_cnt];			//copy Receive_Buffer into rx_ctr
				}
				if(strncmp(rx_str,"BOOTNOW",7)==0) 						//if "BOOTNOW" received
					{
						tft_newl();
						sprintf(str_tx,"ENTER BOOT NOW\r\n");			//write ENTER BOOT NOW to str_tx buffer
						boot_flag = TRUE;									//so we can exit the while
					}//end if BOOTNOW
				else 																	//otherwise
						{
							sprintf(str_tx,"TYPE 'BOOTNOW'\r\n");
						}														
    	  // Echo
    		  CDC_Send_DATA ((uint8_t*)str_tx,strlen(str_tx));		//send echo back
					str_tx[0] = '\0';
    	  
    	  Receive_length = 0;
      }
    }
  }
	*/
	//NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);	//disable USB ITR

	boot_jump();									//jump to boot if exit while
}

void boot_jump (void)
{
	
	tft_newl();
	tft_w("ENTER BOOT NOW");
	for(uint8_t i=0;i<3;i++){tft_w(".");_delay_ms(500);}
	for(uint8_t i=0;i<3;i++){tft_bksps();_delay_ms(500);}
	uint32_t boot_start_adr = *(volatile unsigned long*)(BOOT_BASE_ADR+0x4);	//obtain boot starting address from flash
	jumpto = (void (*)(void))boot_start_adr;	
	jumpto();
}
