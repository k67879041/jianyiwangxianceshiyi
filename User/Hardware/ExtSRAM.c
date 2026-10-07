/*
*********************************************************************************************************
*
*	模块名称 :	ExtSRAM驱动模块
*	文件名称 :	ExtSRAM.c
*	版    本 :	V1.0
*	说    明 :	本程序基于启明欣欣F407开发板设计,使用 MCU 型号 STM32F407ZGT6
*							开发板采用的 SRAM 芯片为 IS62WV51216 , 容量为 1M Byte, 采用 FSMC 接口连接
*	 						本驱动采用 HAL 库驱动, 适用于 STM32F42x 系列 MCU, 
* 						1. FSMC 接口配置采用 CubeMX 完成, 可直接通过地址访问
*							2. 本驱动提供SRAM测试函数
*
*********************************************************************************************************
*/
#include "ExtSRAM.h"


/*
*********************************************************************************************************
*	函 数 名: ExtSRAM_Test
*	功能说明: 扫描测试外部SRAM的全部单元。
*	形    参: 无
*	返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint32_t ExtSRAM_Test(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};
	
	/* 写SRAM */
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}
	
	/* 读SRAM */
	err = 0;
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}
	printf("\r\nSDRAM check round 1 error = %d", err);
	if (err > 0)
	{
		return (4 * err);
	}

	#if 1
	/* 对SRAM 的数据求反并写入 */
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE/4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* 再次比较SRAM的数据 */
	err = 0;
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i<EXT_SRAM_SIZE/4;i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	printf("\r\nSDRAM check round 2 error = %d", err);
	if (err>0)
	{
	return (4 * err);
	}
	#endif

	/* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
	pBytes = (uint8_t *)EXT_SRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* 比较SRAM的数据 */
	err = 0;
	pBytes = (uint8_t *)EXT_SRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	printf("\r\nSDRAM check round 3 error = %d", err);
	if (err > 0)
	{
		return err;
	}
	return 0;
}


