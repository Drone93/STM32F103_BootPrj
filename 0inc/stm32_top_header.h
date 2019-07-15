//Top header for STM32F103C8 CPU
/*SETTINGS
type "USE_STDPERIPH_DRIVER" in Project defines(Options for Target->C/C++->Define)


*/
//Includes
#include "string.h"					//strlen and stuff
#include "math.h"									//for log() functions
//#include "stm32f10x_conf.h"	//for some reason it won't be working if you don't include this
#include "stm32f10x.h"			//main header
#include "stm32f10x_rcc.h"	//Clock control header
#include "stm32f10x_gpio.h"		//GPIO control header
#include "stm32f10x_i2c.h"	//I2C interface header
#include "stm32_delay_asm.h"		//include _delay_us and _delay_ms
#include "TFT_SPI.h"			//include func prototypes for TFT_SPI.c
//Defines
#define shhbyte(x)	(x & 0xF0)	//serial high half-byte
#define slhbyte(x)	(x<<4)	//serial low half-byte
#define chs(byte,bit)	byte&(1<<bit)	//choose a bit
#define set(byte,bit)	byte|=( 1<<bit)	//set bit chosen in a byte to 1 (bit=0..7)
#define clr(byte, bit)  byte&=~(1<<bit) //clear bit chosen in a byte (to 0)
#define i2c_adr_w	0x7E	//address of pcf8574

//General prototypes
void gpio_config(GPIO_TypeDef* GPIOx, GPIOMode_TypeDef GPIO_Mode);		//config selected GPIO in selected mode (OD, PP, other)
void gpio_all_outPP(void);				//config all GPIOs to Push-pull
void gpio_all_outOD(void);				//All GPIOs to Open drain

//Prototypes for PCF8574_I2C.c
void i2c_init(void);						//Init I2C
void lcd_i2c_init(void);				//Init LCD on PCF8574
void lcd_i2c_command(char lcd_command);	//Send command to LCD
void lcd_i2c_data(char lcd_data);				//Send data to LCD
void lcd_i2c_w(char word[]);						//Send word to LCD
void lcd_i2c_hex(long cel);							//Number in hex format
void lcd_i2c_dec(long cel);							//					dec format
void lcd_i2c_clear(void);							//Clear screen
void lcd_i2c_adr(char lcd_adr);				//Set cursor at address (0x0..0xF or 0x40..0x50)



//Protos for TFT_SD_SPI.c
void sd_init(void);										//init SD card

#define FILENAMES_ON 1		//show or not show filenames in sd_find_files
#define FILENAMES_OFF 0
#define SIZES_ON 1				//show/not show sizes of files
#define SIZES_OFF 0
uint16_t sd_find_files(_Bool filenames, _Bool sizes);					//find files on SD card and show their names and sizes on TFT(filenames=1). return number of files
uint16_t sd_find_in_folder(char foldername[], _Bool filenames, _Bool sizes);			//find files in the folder. Foldername "ROOT" searches in root directory

void sd_open_file_txt(char foldername[], char filename[]);							//open text file in folder
void sd_open_file_bmp_v3(char foldername[], char filename[]);						//open BMP V3 file in folder

void sd_create_file(char foldername[], char filename[]);									//create empty file in folder

#define UPDATE 1
#define REWRITE 0
void sd_write_to_file(char foldername[], char filename[], char WR_data[], _Bool update);			//write data to file (1 cluster MAX!!!)
void sd_w_dec_to_file(char foldername[], char filename[],uint32_t cel,  _Bool update);					//WRITE decimal value to file

void sd_delete_file(char foldername[], char filename[]);							//delete file

void sd_show_size(void);						//show card size on TFT
void sd_showCSD(void);							//show CSD
void sd_showCID(void);							//CID
void sd_showOCR(void);							//OCR

void sd_showFAT_type(void);												//show FAT type (from BPB of SD)
void sd_showSectorsize(void);								//show size of sector (bytes)
void sd_showClustersize(void);						//show size of cluster (sectors)

void sd_showSpecs(void);							//show some SD specs (size, sector/cluster size, addresses of FATs, Root, User data) 


