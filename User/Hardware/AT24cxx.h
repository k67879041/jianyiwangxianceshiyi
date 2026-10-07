#ifndef __AT24CXX_H
#define __AT24CXX_H
#include "sys.h"
#include "i2c.h"
	
#define AT_IIC		hi2c1


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767

//STM32F429开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE 						AT24C02
#define AT24C02_WriteAddr		0xA0
#define AT24C02_ReadAddr		0xA1


uint8_t AT24C02_ReadOneByte( uint8_t ReadAddr );
void		AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);
void		AT24C02_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint8_t NumToRead);
void		AT24C02_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint8_t NumToWrite);


#endif
