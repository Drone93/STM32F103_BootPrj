/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern volatile _Bool pmaovr_flag;		//PMA overrun flag
extern volatile _Bool spi_upd;	//SPI update flag
extern volatile uint32_t packet_sent;
extern volatile uint32_t packet_receive;
extern char Receive_Buffer[64];
uint32_t Receive_length;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void EP1_IN_Callback (void)
{
  packet_sent = 1;
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)		//Called from CTR_LP() function when EP3_CTR_RX = 1 (OUT/SETUP transaction is successfully completed)
{
	/*
	Hardware sets the STAT_RX bits to NAK when a
	correct transfer has occurred (CTR_RX=1) corresponding to a OUT or SETUP (control only)
	transaction addressed to this endpoint, so the software has the time to elaborate the
	received data before it acknowledge a new transaction
	*/
	
	if(Receive_length)					//if the previous data was not read, report overrun and retrn
		{	
			//_SetEPRxStatus(ENDP3,EP_RX_NAK);		//NAK endpoint 3
				DOVR_Callback();
				//Receive_length =0;
				packet_receive = 0;
				return;										//set PMA overrun flag and return from callback				
		}
	
  packet_receive = 1;
  Receive_length = GetEPRxCount(ENDP3);
  PMAToUserBufferCopy((unsigned char*)Receive_Buffer, ENDP3_RXADDR, Receive_length);
	spi_upd = TRUE;																		//raise spi flag
	
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
