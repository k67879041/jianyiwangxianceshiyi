/*
适用于STM32的软IIC驱动,引脚由CubeMX配置,宏定义在main.h中
特别注意：SDA引脚因为需要输入输出,应配置为开漏输出,在读之前输出1,即可读取该引脚的实时电平状态
*/



#include "Soft_IIC.h"

/*
*********************************************************************************************************
*	函 数 名: IICs_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void IICs_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频180MHz ，MDK编译环境，1级优化
      
		循环次数为50时，SCL频率 = 333KHz 
		循环次数为30时，SCL频率 = 533KHz，  
	 	循环次数为20时，SCL频率 = 727KHz， 
  */
	for (i = 0; i < 80; i++);
}



/*****************************************************************
初始化IIC端口
*****************************************************************/
void IICs_Init(void)
{
	I2Cs_SCL_1();
	I2Cs_SDA_1();
}


/*****************************************************************
开始 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
*****************************************************************/
static void IICs_Start(void)
{
	I2Cs_SCL_1();
	I2Cs_SDA_1();
  IICs_Delay();
	I2Cs_SDA_0();
  IICs_Delay();
	I2Cs_SCL_0();
  IICs_Delay();
}


/*****************************************************************
停止 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
*****************************************************************/
static void IICs_Stop(void)
{
	I2Cs_SDA_0();
	I2Cs_SCL_1();
  IICs_Delay();
	I2Cs_SDA_1();
}


/*****************************************************************
主机产生应答信号，用于接收从机数据时给从机应答
连续读取的非最末字节后需要输出应答信号
只读取一个字节或连续读取的最后一个字节不需要应答
参数：ack:	ACK产生应答信号;	NACK不产生应答信号
*****************************************************************/
static void IICs_Ack( IICAckTypedef ack )
{
	if( ack )	I2Cs_SDA_1();
	else I2Cs_SDA_0();
//	IICs_SDA = ack;
  IICs_Delay();
	I2Cs_SCL_1();
  IICs_Delay();
	I2Cs_SCL_0();
  IICs_Delay();
	I2Cs_SDA_1();		//释放SDA
}


/*****************************************************************
等待从机的应答信号，用于向从机发送数据时确定从机是否在线
理论上每发送一个字节都需要等待应答
但是有些外设应答机制不健全,所以根据情况选择是否需要等待
参数：		ackmode:	ACK 等待应答;	NACK 不等待应答,直接输出一个SCK脉冲
返回值：	1 接收到应答;	0 未接收到
*****************************************************************/
static uint8_t IICs_WaitAck( IICAckTypedef ackmode )
{
	uint8_t ret = 1,ucErrTime = 0;
	
	I2Cs_SDA_1();	/* CPU释放SDA总线 */
	IICs_Delay();
	I2Cs_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	IICs_Delay();
	
	if( ackmode==ACK )
	{
		while(I2Cs_SDA_READ()==1)
		{
			ucErrTime++;
			if(ucErrTime>=250)
			{
				ret = 0;
				break;
			}
		}
	}
	
	I2Cs_SCL_0();
	IICs_Delay();
	
	return ret;
}


/*****************************************************************
发送一个字节
参数：		dat:	待发送数据
*****************************************************************/
static void IICs_SendByte( uint8_t dat )
{
	uint8_t i;
		
	//I2Cs_SCL_0();
	for(i=0;i<8;i++)
	{
		if(dat&0x80)	I2Cs_SDA_1();
		else					I2Cs_SDA_0();
		
		IICs_Delay();
		I2Cs_SCL_1();
		IICs_Delay();
		I2Cs_SCL_0();
		
		if(i==7)	I2Cs_SDA_1();	//释放总线
		
		dat <<= 1;
		IICs_Delay();
	}
}


/*****************************************************************
读取一个字节
参数：	ack:	ACK 读取结束后产生应答信号;	NACK 读取结束后不产生应答信号
返回值：读取到的数据
*****************************************************************/
static uint8_t IICs_ReadByte( IICAckTypedef ack )
{
	uint8_t i,recdat = 0;
		
	I2Cs_SDA_1();
	IICs_Delay();
	
	for(i=0;i<8;i++)
	{
		recdat <<= 1;		
		I2Cs_SCL_1();
		IICs_Delay();
		if( I2Cs_SDA_READ()==1 )	recdat++;
		I2Cs_SCL_0();
		IICs_Delay();
		
		I2Cs_SDA_1();	//增加的指令,释放总线
	}
	
	IICs_Ack( ack );
	
	return	recdat;
}



/*****************************************************************
从IIC设备中读取
参数：	SlaveAddr:	从机设备地址
				cReg：			从机寄存器
				*pbuf：			数据存储缓冲区
				cnt：				数据个数
				ackmode：		ACK 等待应答;	NACK 不等待应答,直接输出一个SCK脉冲
返回值：0:正确读取;	1:设备地址错;	2:设备寄存器错;	3:设备读地址错
*****************************************************************/
uint8_t	IICs_Read( uint8_t SlaveAddr, uint8_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode )
{
	uint8_t i;
	
	IICs_Start();
	IICs_SendByte(SlaveAddr);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 1;}
	IICs_SendByte(cReg);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 2;}
	
	IICs_Start();
	IICs_SendByte(SlaveAddr+1);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 3;}
	
	for( i=0;i<cnt;i++ )
	{
		if( i==cnt-1 )	pbuf[i] = IICs_ReadByte(NACK);	//最后一个不应答
		else						pbuf[i] = IICs_ReadByte(ACK);	//没接收完，应答
	}
	
	IICs_Stop();
	return 0;
}

/*****************************************************************
从IIC设备中读取（寄存器地址为16位）
参数：	SlaveAddr:	从机设备地址
				cReg：			从机寄存器
				*pbuf：			数据存储缓冲区
				cnt：				数据个数
				ackmode：		ACK 等待应答;	NACK 不等待应答,直接输出一个SCK脉冲
返回值：0:正确读取;	1:设备地址错;	2:设备寄存器错;	3:设备读地址错
*****************************************************************/
uint8_t	IICs_Read2( uint8_t SlaveAddr, uint16_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode )
{
	uint8_t i;
	
	IICs_Start();
	IICs_SendByte(SlaveAddr);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 1;}
	IICs_SendByte(cReg/256);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 2;}
	IICs_SendByte(cReg%256);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 2;}
	
	IICs_Start();
	IICs_SendByte(SlaveAddr+1);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 3;}
	
	for( i=0;i<cnt;i++ )
	{
		if( i==cnt-1 )	pbuf[i] = IICs_ReadByte(NACK);	//最后一个不应答
		else						pbuf[i] = IICs_ReadByte(ACK);	//没接收完，应答
	}
	
	IICs_Stop();
	return 0;
}

/*****************************************************************
写入到IIC设备
参数：	SlaveAddr:	从机设备地址
				cReg：			从机寄存器
				*pbuf：			数据存储缓冲区
				cnt：				数据个数
				ackmode：		ACK 等待应答;	NACK 不等待应答,直接输出一个SCK脉冲
返回值：0:正确写入;	1:设备地址错;	2:设备寄存器错;
*****************************************************************/
uint8_t IICs_Write( uint8_t SlaveAddr, uint8_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode )
{
	uint8_t i;
	
	IICs_Start();
	
	IICs_SendByte(SlaveAddr);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 1;}
	IICs_SendByte(cReg);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 2;}
	
	for( i=0;i<cnt;i++ )
	{
		IICs_SendByte( pbuf[i] );
		IICs_WaitAck(ackmode);
	}
	
	IICs_Stop();
	return 0;
}

/*****************************************************************
写入到IIC设备（寄存器地址为16位）
参数：	SlaveAddr:	从机设备地址
				cReg：			从机寄存器
				*pbuf：			数据存储缓冲区
				cnt：				数据个数
				ackmode：		ACK 等待应答;	NACK 不等待应答,直接输出一个SCK脉冲
返回值：0:正确写入;	1:设备地址错;	2:设备寄存器错;
*****************************************************************/
uint8_t IICs_Write2( uint8_t SlaveAddr, uint16_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode )
{
	uint8_t i;
	
	IICs_Start();
	
	IICs_SendByte(SlaveAddr);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 1;}
	IICs_SendByte(cReg/256);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 2;}
	IICs_SendByte(cReg%256);
	if(	!IICs_WaitAck(ackmode) )	{IICs_Stop();return 2;}
	
	for( i=0;i<cnt;i++ )
	{
		IICs_SendByte( pbuf[i] );
		IICs_WaitAck(ackmode);
	}
	
	IICs_Stop();
	return 0;
}


void IICs_Scan( void )
{
	uint8_t	sAddr;
	IICs_Init();
	
	
	for( sAddr=0; sAddr<0xFE; sAddr+=2 )
	{		
		IICs_Start();
		
		IICs_SendByte(sAddr);
		
		if(	IICs_WaitAck(ACK) )	{

				DEBUGOUT( "\r>>SlaveAddr = 0x%02x is online", sAddr );

		}
		
		IICs_Stop();
	}
}

