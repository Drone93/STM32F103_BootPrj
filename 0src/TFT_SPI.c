/*VERSION 0.6 
 - Improved tft_w, tft_spi_command and tft_spi_data functions,
 - Now it's possible to type " /n" at the end of any word to jump on the new line
 - ',' '.' works correctly (see TFT_alphabet.h version 0.4)
 - tft_hex and tft_dec works now in with uint32_t
 - added tft_w_ascii function, which return ASCII symbol from input 8-bit code (0x41->'A')
 - tft_newl won't set cursor on the new line if it's been already in X=0,Y=0 point
 - tft_w - delay 1s, clear if crdY>19
 
 - in tft_w 0x0A means new line
 
 - 15.05.19 tft_dec, dec_value, hex, hex_value now operating in uint64_t
 - 16.05.19 added tft_float_dec function
 */
/*SETTINGS
type "USE_STDPERIPH_DRIVER" in Project defines(Options for Target->C/C++->Define)

Include the following paths in project (Options for Target->C/C++->Include Paths):
	.\00_Headers\CMSIS_Include
	.\00_Headers\Device_Include
	.\00_Headers\StdPeriph_Include
	.\00_Headers\Delay
	.\00_Headers\Top header
	Other source and headers's folders
Add: 
	stm32_delay.c from 00_Headers
	LCD_PCF8574_I2C and other source files
Configure Run-Time Environment:
		CMSIS->Core	- necessarily
		Device->Startup	- necessarily
		Device->STDPeriph Drivers (RCC, GPIO) - necessarily
		Device->STDPeriph Drivers (I2C, SPI, other interface)
*/

/*TFT pinout
GPIOB,12 - NSS
GPIOB,13 - SCK
GPIOB,14 - MISO
GPIOB,15 - MOSI

GPIOB,11 - D/Cx
GPIOB,10 - HW reset

GPIOB,1 - SD_CS
*/
//Includes
#include "stm32_top_header.h"	
#include "TFT_alphabet.h"	//Switch function and alphabet for TFT display

//Global VAriables
char bgR=0;	//RED background
char bgG=0;	//Green
char bgB=0;	//Blue

uint8_t crdX=0; //Global coordinates of cursor position
uint8_t crdY=0;

//Send command to TFT
void tft_spi_command(char command)
{
	SPI_Cmd(SPI2, ENABLE);	//enable SPI
	clr(GPIOB->ODR,11);			//D/Cx = 0
	clr(GPIOB->ODR,12);			//NSS \__
	while(!(SPI2->SR & SPI_SR_TXE)){}	//wait until TXE = 1
	SPI2->DR = command;			//send command into DR
	while(!(SPI2->SR & SPI_SR_RXNE)){}	//wait until RXNE = 1
	(void)SPI2->DR;
	while((SPI2->SR & SPI_SR_BSY)){}	//wait until BSY = 0 
	//SPI_Cmd(SPI2, DISABLE);						//Turn off SPI
	set(GPIOB->ODR,12);			//NSS _/
}

//Send data to TFT
void tft_spi_data(char data)
{
	SPI_Cmd(SPI2, ENABLE);	//enable SPI
	set(GPIOB->ODR,11);			//D/Cx = 1
	clr(GPIOB->ODR,12);			//NSS \__
	while(!(SPI2->SR & SPI_SR_TXE)){}	//wait until TXE = 1
	SPI2->DR = data;			//send command into DR
	while(!(SPI2->SR & SPI_SR_RXNE)){}	//wait until RXNE = 1
	(void)SPI2->DR;
	while((SPI2->SR & SPI_SR_BSY)){}	//wait until BSY = 0 
	//SPI_Cmd(SPI2, DISABLE);						//Turn off SPI
	set(GPIOB->ODR,12);			//NSS _/
}

//Init TFT with defaul settings (blue background, SPI ~18Mbps)
void tft_init_defaults(void)
{
	tft_spi_init(SPI_BaudRatePrescaler_2);		//hi-speed for TFT
	tft_init(0xC0, 0x00, 0x00, 0xFF);	//Blue background, display turned upside down
}
//Init TFT 
void tft_init(char MADCTL, char backR, char backG, char backB)
{
	clr(GPIOB->ODR,10);		//HW reset
	_delay_ms(10);
	set(GPIOB->ODR,10);
	_delay_ms(250);
	
	tft_spi_command(0x3A);	//COLMOD
	tft_spi_data(0x06);		//18-bit mode

	tft_spi_command(0x11);	//Sleep out
	_delay_ms(150);	//wait when SLPOUT is done

	tft_spi_command(0x29);	//Display on

	tft_spi_command(0x36);	//MADCTL
	tft_spi_data(MADCTL);
	tft_bgr(backR, backG, backB);	//background color
}

void tft_bgr(char backR, char backG, char backB)
{
	tft_spi_command(0x2A);	//CASET
	tft_spi_data(0x00);
	tft_spi_data(0x00);		//XS=0
	tft_spi_data(0x00);
	tft_spi_data(0x9F);		//XE=9F

	tft_spi_command(0x2B);	//RASET
	tft_spi_data(0x00);
	tft_spi_data(0x00);		//YS=1
	tft_spi_data(0x00);
	tft_spi_data(0x9F);		//YE=9F

	tft_spi_command(0x2C);	//RAMWR

	for(int i=0;i<0x62C1;i++)
	{
	tft_spi_data(backR);	//R
bgR = backR;		//refrash global color vars with the new ones
	tft_spi_data(backG);	//G
bgG = backG;	
	tft_spi_data(backB);	//B
bgB = backB;
	}//end for
}

//Clear screen
void tft_clear(void)
{
	tft_bgr(bgR, bgG, bgB);
	tft_adr(0,0);
}//end TFT_clear

void tft_adr(char X, char Y)
{
	crdX = X;
	crdY = Y;
}

//to the new line
void tft_newl(void)
{
if((crdX==0) && (crdY==0)) {return;}			//if X,Y coords have been already zeros, do nothing
	crdX=0;
	crdY++;
}
//Fill pixel with RGB color on x,y, coordinates
void tft_pixel_color (uint8_t R, uint8_t G, uint8_t B)
{
	tft_spi_command(0x2A);	//CASET
	tft_spi_data(0x00);
	tft_spi_data(crdX);		//XS=x
	tft_spi_data(0x00);
	tft_spi_data(crdX);		//XE=x

	tft_spi_command(0x2B);	//RASET
	tft_spi_data(0x00);
	tft_spi_data(crdY);		//YS=y
	tft_spi_data(0x00);
	tft_spi_data(crdY);		//YE=y

	tft_spi_command(0x2C);	//RAMWR
	tft_spi_data(R);	//R
	tft_spi_data(G);	//fill pixel with Green color
	tft_spi_data(B);	//B
}

//7x8 font numbers or letters
void tft_7x8(char num, char segX, char segY)
{
	int z = 0;			//flag for RAMWR
	//Fill array with 0s
	for(int i=0;i<sizeof(num_arr);i++) num_arr[i] = 0;
	//////SWITCH///Choose the proper letter or number from alphabet
	Switch (num);	//See TFT_alphabet.h

	tft_spi_command(0x2A);	//CASET
	tft_spi_data(0x00);
	tft_spi_data(segX*7);		//XS=0
	tft_spi_data(0x00);
	tft_spi_data(segX*7+6);		//XE=06 (0..6 = 7 pixels)

	tft_spi_command(0x2B);	//RASET
	tft_spi_data(0x00);
	tft_spi_data(segY*8);		//YS=0
	tft_spi_data(0x00);
	tft_spi_data(segY*8+7);		//YE=07 (0..7 = 8 pixels)

	tft_spi_command(0x2C);	//RAMWR
	////////
	for(int i=0;i<56;i++)		//7x8=56
	{
	z=0;
		for(int j=0;j<sizeof(num_arr);j++)
		{
	if((i==num_arr[j])&&(num_arr[j]!=0))	//num_arr is declared in tft_spi_alphabet.h
	{
	tft_spi_data(0xFF);	//white
	tft_spi_data(0xFF);	//white fill pixel with white color
	tft_spi_data(0xFF);	//white 
	z = 1;			//white pixel was written
	}//end if
		}//end for j
	if(!z)			//if z=0
	{
	tft_spi_data(bgR);	//R
	tft_spi_data(bgG);	//G		fill pixel with background color
	tft_spi_data(bgB);	//B
	}//end if
	}//end for i
}

void tft_w_ascii(uint8_t code)		//convert 8-bit code to symbol
{
	char toascii[1];			//array for conversion code to ascii symbols
	toascii[0] = code;
	tft_w(toascii);
}

//Write a word to TFT
void tft_w(char word[])
{
int i;	//letter counter
int j;
char nls=0;	//new line semaphore ' ' '/' 'n'
i=strlen(word);

	for (j=0;j<i;j++)
{
	if(crdX>17) {tft_newl();}	//if cursor -->| go to the next line
	if(crdY>19) {_delay_ms(1000); tft_clear();}			//if display is full
	
	if(word[j]==0x0A) {tft_newl(); break;}								//if new line
	//if(word[j]==0x0D) {tft_adr(0,crdY);}					//if carriage return
	
if(word[j]==' ') {set(nls,0);}			//if letter is ' ' set semaphore
if(word[j]=='/') 
{if(chs(nls,0)) set(nls,1); else nls=0;}	//if previos letter was ' ' 
if(word[j]=='n')
{if(chs(nls,1)) set(nls,2); else nls=0;}	//if previos letter was '/'

	tft_7x8(word[j],crdX,crdY);		//write letter
	crdX++;				//cursor slide
	}//end for
if(nls==0x07) 
	{
		for(int i=0;i<3;i++) {tft_bksps();}			//CLEAr last 3 letters (' ''/''n')
		tft_newl();			//if all 3 semaphores's already set	go to new line
		nls=0;
	}
}

//Backspace
void tft_bksps(void)
{	
crdX--;			//set cursor to previous position
	if(crdX==0xFF) {crdY--; crdX=17;}	//if cursor |<-- go to the previous line
	tft_7x8(' ',crdX,crdY);		//write space
}//end TFT_bksps

//Write a number in decimal to TFT on X, Y begin coordinates in 7x8 font
void tft_dec(uint64_t cel)
{
char wor[20];		//array for number word (max 10 letters)
char row[20];		//redirected word array

char ost=cel%10;	//remainder from 1st divis
int j=0;
wor[j]=ost;	//word[0]=ost=4
while (cel>=10){	//1. 32>10	2.return
ost=cel%10;		//1. ost=2
cel=cel/10;		//1. cel=3
wor[j]=ost;	//word[1]=ost=2
j++;			//j=2
}//end while
if(j>0) wor[j]=cel;	//word[2]=cel=3
for (int i=0;i<j+1;i++){

char buf=wor[i];
row[j-i]=buf;	//redirect array
}//end for
for (int i=0;i<j+1;i++){	//j - number of divisions
if(crdX>17) {crdY++; crdX=0;}	//if cursor -->| so go to the next line
tft_7x8(row[i],crdX,crdY);		//Global variables crdX,Y 
crdX++;
}//end for
}
//write floating point decimal value to tft (tolerance - number of digits after '.')
void tft_float_dec(double num, uint8_t tolerance)
{
	double _num = num;			//copy
	if(num<0) {tft_w("-"); _num = -_num;}		//if the number<0, type "-" and make _num positive
	uint8_t cnt=tolerance;
	uint64_t e;
	do
	{
		_num = _num / 10;								//normalize
		cnt++;
	}while((uint64_t)_num > 0);
	_num = _num *10;
	while(cnt > 0)
	{
	e = (uint64_t)_num;	//extract the whole part
	tft_dec(e);						//type digit
	cnt--;
	if(cnt == tolerance){tft_w(".");}				//type dot	
	_num = _num - (double)e;		//substract the whole part
	_num *= 10;						//multiply by ten
	}
}

//Write the name of value in dec format and the value itself
void tft_dec_value(char name[],uint64_t value)
{
	tft_w(name);
	tft_dec(value);
}

//Write the name of value in float dec format and the value itself
void tft_float_dec_value(char name[],double value,uint8_t tolerance)
{
	tft_w(name);
	tft_float_dec(value,tolerance);
}

//Write a number in hex to TFT on X, Y begin coordinates in 7x8 font
void tft_hex(uint64_t cel)
{
char wor[16];		//array for number word (max 8 letters)
char row[16];		//redirected word array
char A=0x37;		//address of A
char ost=cel%16;	//ostatok from 1st divis
int j=0;			//counter for number of divisions
wor[j]=ost;	
while (cel>=16){	
ost=cel%16;		
cel=cel/16;		
wor[j]=ost;	
j++;			
}//end while
if(j>0) wor[j]=cel;	//if more than 1 division occured
for (int i=0;i<j+1;i++){

char buf=wor[i];	//buffer for array redirection
row[j-i]=buf;	//redirect array
}//end for
for (int i=0;i<j+1;i++){	//j - number of divisions
if(crdX>17) {crdY++; crdX=0;}	//if cursor -->| so go to the next line
if(row[i]>9) row[i] = row[i]+A;	//get symbol address A..F
tft_7x8(row[i],crdX,crdY);		//Write row[i] element to segment with crdX,Y (global vars)
crdX++;							//cursor slide
}//end for
}

//Write name and value in hex format
void tft_hex_value(char name[],uint64_t value)
{
	tft_w(name);
	tft_hex(value);
}

/////////////////////Periph init functions
//Init SPI for TFT
void tft_spi_init(uint16_t SPI_BaudRatePrescaler_x)
{
	//Structures rename
	GPIO_InitTypeDef gpio;
	SPI_InitTypeDef spi;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);		//Enable clock to SPI
	
	gpio_all_outPP();			//All ports to general output PP

	gpio.GPIO_Pin = GPIO_Pin_14;				//Choose 14 pin for MISO
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//Input floating
	//gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);			//Initialize port B with this settings
	
	gpio.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;	//Choose 13,15 pins for SCK, MOSI
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;			//Alternate function PP
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);			//Initialize port B with this settings
	
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//Direction of SPI - full duplex (both MISO, MOSI are used)
	spi.SPI_Mode = SPI_Mode_Master;												//master mode
	spi.SPI_DataSize = SPI_DataSize_8b;								//8-bit data length
	spi.SPI_CPOL = SPI_CPOL_Low;											//CPOL = 0
	spi.SPI_CPHA = SPI_CPHA_1Edge;									//CPHA = 0
	spi.SPI_NSS = SPI_NSS_Soft;										//NSS by soft (GPIOB12)
	spi.SPI_FirstBit = SPI_FirstBit_MSB;					//MSB first
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_x;		//Baud rate (APB1 = 36 MHz)
	SPI_Init(SPI2, &spi);			//Init SPI with this settings
	
	SPI2->CR2 |=SPI_CR2_SSOE;	//SSOE = 1
	
	GPIOB->ODR = 0x1C02;			//PORTB 1, 10,11,12 = 1
	//Enable SPI
	SPI_Cmd(SPI2, ENABLE);		//enable SPI2
}

//Configure the whole port (RCC_APB..PeriphClockCMD must be enabled previously)
void gpio_config(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_All;
	gpio.GPIO_Mode = GPIO_Mode;
	GPIO_Init(GPIOx, &gpio);
}
//All ports to Output push-pull
void gpio_all_outPP(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//ENABLE clock to all ports
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	gpio_config(GPIOA, GPIO_Mode_Out_PP);
	gpio_config(GPIOB, GPIO_Mode_Out_PP);
	gpio_config(GPIOC, GPIO_Mode_Out_PP);
}

//All ports to Output OD
void gpio_all_outOD(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//ENABLE clock to all ports
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	gpio_config(GPIOA, GPIO_Mode_Out_OD);
	gpio_config(GPIOB, GPIO_Mode_Out_OD);
	gpio_config(GPIOC, GPIO_Mode_Out_OD);
}
