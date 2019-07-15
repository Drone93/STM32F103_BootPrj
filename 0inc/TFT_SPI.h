/*VERSION 0.6 16.05.19*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TFT_SPI_H
#define __TFT_SPI_H

#ifndef uint64_t
#include "stdint.h"		//so the compiler knows what's the uintxx_t mean 
#endif


//Prototypes for TFT_SPI.c
void tft_spi_init(uint16_t SPI_BaudRatePrescaler_x);							//Init SPI (prescaler - parameter for speed), SPI2 used
void tft_init(char MADCTL, char backR, char backG, char backB);	//init TFT with MADCTL and background color
void tft_init_defaults(void);																		//Init TFT with defaults (blue background, SPI ~18Mbps)
void tft_spi_command(char command);	//Send command to TFT
void tft_spi_data(char data);				//Send data to TFT

void tft_bgr(char backR, char backG, char backB);		//set background color
void tft_clear(void);																//CLEAR TFT
void tft_adr(char X, char Y);									//set address for 7x8 letter coordinates
void tft_newl(void);													//New line
void tft_7x8(char num, char segX, char segY);		//WRITE white letter on the chosen coordinates
void tft_w(char word[]);											//write word (white)
void tft_w_ascii(uint8_t code);		//convert 8-bit code to symbol
void tft_dec(uint64_t cel);												//WRITE number in dec format

void tft_float_dec(double num, uint8_t tolerance);										//write floating-point number
void tft_float_dec_value(char name[],double value,uint8_t tolerance);	//

void tft_dec_value(char name[],uint64_t value);		//WRITE name and value of variable in dec format("a ", a);
void tft_hex(uint64_t cel);												//WRITE number in hex
void tft_hex_value(char name[],uint64_t value);	//write name and value in hex
void tft_bksps(void);													//backspase

void tft_pixel_color (uint8_t R, uint8_t G, uint8_t B);					//fill pixel on x,y, coords with RGB

#endif
