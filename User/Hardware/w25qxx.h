#ifndef __W25QXX_H
#define __W25QXX_H

#include "sys.h"
#include "spi.h"
#include "main.h"

/**** W25X系列/Q系列芯片列表 ****/
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern u16 W25QXX_TYPE;			//定义W25QXX芯片型号		   

/**** 最大超时时间,单位ms ****/
#define W25Q_TIMEOUT	1000

/****************************** 硬件相关定义 ******************************/
//-------------------SPI句柄由CubeMX生成,在 spi.h 中-------------------
#define	hW25QX_SPI						hspi1
//------------- NSS端口和引脚定义由CubeMX生成,在 main.h 中-------------
#define W25QX_Enable()				HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET)
#define W25QX_Disable()				HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET)

/*************** 指令表 ***************/
#define W25X_WriteEnable				0x06 
#define W25X_WriteDisable				0x04 
#define W25X_ReadStatusReg1			0x05 
#define W25X_ReadStatusReg2			0x35 
#define W25X_ReadStatusReg3			0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData						0x03 
#define W25X_FastReadData				0x0B 
#define W25X_FastReadDual				0x3B 
#define W25X_PageProgram				0x02 
#define W25X_BlockErase					0xD8 
#define W25X_SectorErase				0x20 
#define W25X_ChipErase					0xC7 
#define W25X_PowerDown					0xB9 
#define W25X_ReleasePowerDown		0xAB 
#define W25X_DeviceID						0xAB 
#define W25X_ManufactDeviceID		0x90 
#define W25X_JedecDeviceID			0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9


/********** 存储地址分配 ***********/
//总容量为 128Mb，即 16MB。由 1024 个块组成，每个块包含 128 个扇区，每个块的大小为 64KB,每个扇区为 4KB。
#define	W25X_CFFT_SIN_8k		0x00980000			//  8k正弦表地址
#define	W25X_CFFT_COS_8k		0x00984000			//  8k余弦表地址
#define	W25X_CFFT_SIN_16k		0x00988000			// 16k正弦表地址
#define	W25X_CFFT_COS_16k		0x00990000			// 16k余弦表地址
#define	W25X_CFFT_SIN_32k		0x00998000			// 32k正弦表地址
#define	W25X_CFFT_COS_32k		0x009A8000			// 32k余弦表地址
#define	W25X_CFFT_SIN_64k		0x009B8000			// 64k正弦表地址
#define	W25X_CFFT_COS_64k		0x009D8000			// 64k余弦表地址

#define	W25X_GBK12ADDR			0x00A00000			// GBK12字库地址
#define	W25X_GBK16ADDR			0x00A8D000			// GBK16字库地址
#define	W25X_GBK24ADDR			0x00B49000			// GBK24字库地址
#define	W25X_GBK32ADDR			0x00CEE000			// GBK32字库地址
#define	W25X_TPAdjADDR_L		0x00FDB000			// 触屏校准参数地址 横屏
#define	W25X_TPAdjADDR_P		0x00FDB020			// 触屏校准参数地址 竖屏

#define W25X_IMAGE_1_START   0x00200000  // 240×320 的图片1张（约 150KB）// 240×320 的图片1张（约 150KB,153608字节），end 0x225808,0x100000-0x1b0000有不明数据
#define W25X_IMAGE_2_START_test 0x001b0000 //60*28的测试图片1张（3368字节）.end 0x001b3368
#define W25X_IMAGE_3_START 0x225808 //240×320 的图片1张（约 150KB,153608字节），end 0x24B010

void W25QXX_Read( uint32_t ReadAddr, uint8_t* pBuffer, uint16_t NumByteToRead );
void W25QXX_Write( uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite );
void W25QXX_Init(void);

void W25QXX_PowerDown(void);
void W25QXX_WAKEUP(void);
void W25QXX_Write_Enable(void);
void W25QXX_Write_Disable(void);
void W25QXX_Erase_Chip(void);
void W25QXX_Erase_Sector( uint32_t Dst_Addr );

#endif
