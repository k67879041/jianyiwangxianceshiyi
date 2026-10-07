/*
*********************************************************************************************************
*
*	模块名称 :	w25q系列芯片驱动模块
*	文件名称 :	w25qxx.c
*	版    本 :	V1.0
*	说    明 :	本程序基于正点原子阿波罗开发板设计,使用 MCU 型号 STM32F429IGT6
*							开发板采用 SPI 存储芯片为 w25q256, 容量为 32M Byte, 4K bytes 为一个 Sector,
*							16 个 Sector 为 1 个 Block,共有 512 个 Block, 8192 个 Sector
*	修改记录 : 
* 						本驱动在正点原子代码基础上修改, 适用于 STM32F42x 系列 MCU, 采用 HAL 库驱动
* 						1. SPI 接口配置采用 CubeMX 完成, 硬件NSS, CPOL高, CPHA=2 edge
*
*
*********************************************************************************************************
*/


#include "w25qxx.h"

uint16_t W25QXX_TYPE;

//写入一字节
static void W25QXX_WriteByte( uint8_t _data )
{
	HAL_SPI_Transmit( &hW25QX_SPI, &_data, 1, W25Q_TIMEOUT );
}

//读取一字节
static uint8_t W25QXX_ReadByte( void )
{
	uint8_t temp;
	HAL_SPI_Receive( &hW25QX_SPI, &temp, 1, W25Q_TIMEOUT );
	return temp;
}

/*
*********************************************************************************************************
*	函 数 名: W25QXX_ReadSR
*	功能说明: 读取W25QXX的状态寄存器
*	参    数: regno 状态寄存器[1,2,3],默认读取状态寄存器1
*	返 回 值: 状态寄存器值
*						W25QXX一共有3个状态寄存器
*						状态寄存器1：默认:0x00
*							BIT7  6   5   4   3   2   1   0
*							SPR   RV  TB BP2 BP1 BP0 WEL BUSY
*								SPR:默认0,状态寄存器保护位,配合WP使用
*								TB,BP2,BP1,BP0:FLASH区域写保护设置
*								WEL:写使能锁定
*								BUSY:忙标记位(1,忙;0,空闲)
*						状态寄存器2：
*							BIT7  6   5   4   3   2   1   0
*							SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
*						状态寄存器3：
*							BIT7      6    5    4   3   2   1   0
*							HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
*********************************************************************************************************
*/
static uint8_t W25QXX_ReadSR( uint8_t regno )
{  
	uint8_t temp=0,command=0;
	
	switch(regno)
	{
		case 1:
			command=W25X_ReadStatusReg1;    //读状态寄存器1指令
			break;
		case 2:
			command=W25X_ReadStatusReg2;    //读状态寄存器2指令
			break;
		case 3:
			command=W25X_ReadStatusReg3;    //读状态寄存器3指令
			break;
		default:
			command=W25X_ReadStatusReg1;    
		break;
	}
	
	W25QX_Enable();
	W25QXX_WriteByte(command);        //发送读取状态寄存器命令    
	temp = W25QXX_ReadByte( );        //读取一个字节
	W25QX_Disable();
	
	return temp;   
}


/*
*********************************************************************************************************
*	函 数 名: W25QXX_WriteSR
*	功能说明: 写W25QXX状态寄存器
*	参    数: regno 状态寄存器[1,2,3],默认写状态寄存器1
*	返 回 值: 无
*********************************************************************************************************
*/
//static void W25QXX_WriteSR( uint8_t regno, uint8_t sr )   
//{   
//	uint8_t command=0;
//	switch(regno)
//	{
//			case 1:
//					command=W25X_WriteStatusReg1;    //写状态寄存器1指令
//					break;
//			case 2:
//					command=W25X_WriteStatusReg2;    //写状态寄存器2指令
//					break;
//			case 3:
//					command=W25X_WriteStatusReg3;    //写状态寄存器3指令
//					break;
//			default:
//					command=W25X_WriteStatusReg1;    
//					break;
//	}
//	
//	W25QX_Enable();
//	W25QXX_WriteByte( command );            //发送写取状态寄存器命令    
//	W25QXX_WriteByte( sr );                 //写入一个字节
//	W25QX_Disable();
//}

//等待空闲
static void W25QXX_Wait_Busy(void)   
{	
	while( (W25QXX_ReadSR(1)&0x01)==0x01 ) ;   // 等待BUSY位清空
}

//W25QXX写使能	
//将WEL置位   
void W25QXX_Write_Enable(void)
{
	W25QX_Enable();
    W25QXX_WriteByte( W25X_WriteEnable );   //发送写使能
	W25QX_Disable();
}

//W25QXX写禁止	
//将WEL清零  
void W25QXX_Write_Disable(void)
{
	W25QX_Enable();
    W25QXX_WriteByte( W25X_WriteDisable );  //发送写禁止指令 
	W25QX_Disable();
} 


/*
*********************************************************************************************************
*	函 数 名: W25QXX_ReadID
*	功能说明: 读取芯片ID
*	参    数: 无
*	返 回 值: 芯片ID, 各芯片ID见头文件宏定义
*********************************************************************************************************
*/
static uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;
	
	W25QX_Enable();
	W25QXX_WriteByte(0x90);//发送读取ID命令	    
	W25QXX_WriteByte(0x00); 	    
	W25QXX_WriteByte(0x00); 	    
	W25QXX_WriteByte(0x00); 	 			   
	Temp = W25QXX_ReadByte()<<8;  
	Temp |= W25QXX_ReadByte();
	W25QX_Disable();
	
	return Temp;
}


//擦除整个芯片		  
//等待时间超长...
void W25QXX_Erase_Chip(void)
{                                   
	W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_Wait_Busy();   
	
	W25QX_Enable();
	W25QXX_WriteByte( W25X_ChipErase );     //发送片擦除命令
	W25QX_Disable();
						
	W25QXX_Wait_Busy();   				   				//等待芯片擦除结束
}


/*
*********************************************************************************************************
*	函 数 名: W25QXX_Erase_Sector
*	功能说明: 擦除一个扇区(4K), 擦除一个扇区的最少时间:150ms
*	参    数: Dst_Addr 扇区地址, 即32位/24位存储器地址
*	返 回 值: 芯片ID, 各芯片ID见头文件宏定义
*********************************************************************************************************
*/
void W25QXX_Erase_Sector( uint32_t Dst_Addr )
{  
	//监视falsh擦除情况,测试用
 	//DEBUGOUT("\r\nfe:%x\r\n",Dst_Addr);
	
 	Dst_Addr*=4096;
	
	W25QXX_Write_Enable();                  //SET WEL 	 
	W25QXX_Wait_Busy();

	W25QX_Enable();
	W25QXX_WriteByte( W25X_SectorErase );   //发送扇区擦除指令 
	if(W25QXX_TYPE==W25Q256)                //如果是W25Q256的话地址为4字节的，要发送最高8位
	{
			W25QXX_WriteByte((uint8_t)((Dst_Addr)>>24)); 
	}
	W25QXX_WriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
	W25QXX_WriteByte((uint8_t)((Dst_Addr)>>8));   
	W25QXX_WriteByte((uint8_t)Dst_Addr);
	W25QX_Disable();
	
	W25QX_Disable();
	W25QXX_Wait_Busy();   				    //等待擦除完成
}



/*
*********************************************************************************************************
*	函 数 名: W25QXX_Read
*	功能说明: 读取 SPI FLASH
*	参    数: pBuffer:数据存储区
*						ReadAddr:开始读取的地址
*						NumByteToRead:要读取的字节数(最大65535)
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Read( uint32_t ReadAddr, uint8_t* pBuffer, uint16_t NumByteToRead )
{
	W25QX_Enable();
	
	W25QXX_WriteByte( W25X_ReadData );    //发送读取命令
	if(W25QXX_TYPE==W25Q256)            //如果是W25Q256的话地址为4字节的，要发送最高8位
	{
			W25QXX_WriteByte((uint8_t)((ReadAddr)>>24));    
	}
	W25QXX_WriteByte((uint8_t)((ReadAddr)>>16));   //发送24bit地址    
	W25QXX_WriteByte((uint8_t)((ReadAddr)>>8));   
	W25QXX_WriteByte((uint8_t)ReadAddr);   
  
	HAL_SPI_Receive( &hW25QX_SPI, pBuffer, NumByteToRead, W25Q_TIMEOUT );
	
	W25QX_Disable();
}


/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Page
*	功能说明: 连续写入少于256字节的数据
*	参    数: pBuffer:数据存储区
*						WriteAddr:开始写入的地址
*						NumByteToWrite:要写入的字节数(最大256)
*								W25Q芯片连续写入以页(256 Byte)为单位, 连续写入是地址低8位自动递增, 
*								超出页范围会循环覆盖页起始地址
*	返 回 值: 无
*********************************************************************************************************
*/
static void W25QXX_Write_Page( uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite )
{
// 	uint16_t i;
	
	W25QXX_Write_Enable();                  //SET WEL
	
	W25QX_Enable();
	
	W25QXX_WriteByte( W25X_PageProgram );   //发送写页命令   
	if(W25QXX_TYPE==W25Q256)                //如果是W25Q256的话地址为4字节的，要发送最高8位
	{
			W25QXX_WriteByte((uint8_t)((WriteAddr)>>24)); 
	}
	W25QXX_WriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
	W25QXX_WriteByte((uint8_t)((WriteAddr)>>8));   
	W25QXX_WriteByte((uint8_t)WriteAddr);
	
	HAL_SPI_Transmit( &hW25QX_SPI, pBuffer, NumByteToWrite, W25Q_TIMEOUT );
	//for(i=0;i<NumByteToWrite;i++) W25QXX_WriteByte( pBuffer[i] );//循环写数
	
	W25QX_Disable();
	
	W25QXX_Wait_Busy();					   //等待写入结束
}


/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_NoCheck
*	功能说明: 无检验写SPI FLASH, 具有自动换页功能
*						必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*	参    数: pBuffer:数据存储区
*						WriteAddr:开始写入的地址
*						NumByteToWrite:要写入的字节数(最大65536)
*	返 回 值: 无
*********************************************************************************************************
*/
static void W25QXX_Write_NoCheck( uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite )   
{ 			 		 
	uint16_t pageremain;
	
	pageremain = 256 - WriteAddr%256; //单页剩余的字节数
	
	if( NumByteToWrite<=pageremain ) pageremain = NumByteToWrite;	//不超出页范围
	
	while(1)
	{	   
		W25QXX_Write_Page( WriteAddr, pBuffer, pageremain );
		
		if( NumByteToWrite==pageremain ) break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;
			NumByteToWrite -= pageremain;			  //减去已经写入了的字节数
			
			if(NumByteToWrite>256) pageremain=256; //一次可以写入256个字节
			else pageremain = NumByteToWrite; 	  //不够256个字节了
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write
*	功能说明: 在指定地址开始写入指定长度的数据, 该函数带擦除操作!
*						必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*	参    数: pBuffer:数据存储区
*						WriteAddr:开始写入的地址
*						NumByteToWrite:要写入的字节数(最大65536)
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write( uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite )
{	
	uint8_t *W25QXX_BUF;
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;
	uint16_t i;
	
  W25QXX_BUF = W25QXX_BUFFER;
 	secpos = WriteAddr/4096; //扇区地址
	secoff = WriteAddr%4096; //在扇区内的偏移
	secremain = 4096-secoff; //扇区剩余空间大小
	
 	//DEBUGOUT("\r\nad:0x%x,nb:%d\r\n",WriteAddr,NumByteToWrite);//测试用
	
 	if( NumByteToWrite<=secremain ) secremain = NumByteToWrite; //不超出扇区范围
	while(1)
	{
		W25QXX_Read( secpos*4096, W25QXX_BUF, 4096 );//读出整个扇区的内容
		
		for(i=0;i<secremain;i++) //校验数据,待写入区域只要有一个字节不是0xFF,就必须擦除整个扇区
		{
			if( W25QXX_BUF[secoff+i]!=0XFF ) break;//需要擦除
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];
			}
			W25QXX_Write_NoCheck( secpos*4096, W25QXX_BUF, 4096 );//写入整个扇区
		}
		else W25QXX_Write_NoCheck( WriteAddr, pBuffer, secremain );//写已经擦除了的,直接写入扇区剩余区间
		
		if(NumByteToWrite==secremain)	break;//写入结束了
		else//写入未结束
		{
			secpos++;								//扇区地址增1
			secoff = 0;							//偏移位置为0
		  pBuffer += secremain;		//指针偏移
			WriteAddr += secremain;	//写地址偏移
		  NumByteToWrite -= secremain;	//字节数递减
			
			if( NumByteToWrite>4096 )	secremain = 4096;	//下一个扇区还是写不完
			else secremain = NumByteToWrite;			//下一个扇区可以写完了
		}
	}
}


void W25QXX_Init(void)
{ 
  uint8_t temp;
	
	W25QXX_TYPE = W25QXX_ReadID();	        //读取FLASH ID.
	if(W25QXX_TYPE==W25Q256)                //SPI FLASH为W25Q256
	{
		temp = W25QXX_ReadSR(3);              //读取状态寄存器3，判断地址模式
		if((temp&0x01)==0)			        			//如果不是4字节地址模式,则进入4字节地址模式
		{
			W25QX_Enable();
			W25QXX_WriteByte( W25X_Enable4ByteAddr );//发送进入4字节地址模式指令
			W25QX_Disable();
		}
	}
}

//进入掉电模式
void W25QXX_PowerDown(void)   
{
	W25QX_Enable();
	W25QXX_WriteByte( W25X_PowerDown );     //发送掉电命令
	W25QX_Disable();
	Delay_us(3);                            //等待TPD  
}

//唤醒
void W25QXX_WAKEUP(void)   
{
	W25QX_Enable();
	W25QXX_WriteByte( W25X_ReleasePowerDown );  //  send W25X_PowerDown command 0xAB
	W25QX_Disable();
	Delay_us(3);                                //等待TRES1
}


