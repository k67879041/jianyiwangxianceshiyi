#include "AT24cxx.h"




//在AT24C02指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
uint8_t AT24C02_ReadOneByte( uint8_t ReadAddr )
{
	uint8_t ret=0;
	
	HAL_I2C_Mem_Read( &AT_IIC, AT24C02_ReadAddr, ReadAddr, I2C_MEMADD_SIZE_8BIT, &ret, 1, 100 );
	
	return ret;
}


//在AT24C02指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
	HAL_I2C_Mem_Write( &AT_IIC, AT24C02_WriteAddr, WriteAddr, I2C_MEMADD_SIZE_8BIT, &DataToWrite, 1, 100 );
}


//在AT24C02里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24C02_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint8_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24C02_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}


//在AT24C02里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24C02_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint8_t NumToWrite)
{
	while(NumToWrite--)
	{
		AT24C02_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
