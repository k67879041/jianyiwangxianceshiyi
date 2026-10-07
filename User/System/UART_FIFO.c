/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : UART_FIFO.c
*	版    本 : V1.0
*	说    明 : 采用串口中断+FIFO模式实现多个串口的同时访问
*	修改记录 :
* 
* 本串口驱动在安富莱代码基础上修改, 适用于 STM32F40x 系列 MCU, 采用 LL 库驱动
* HAL库串口驱动太复杂, 效率太低, 尤其是设置了锁定机制, 生生将全双工端口变成了半双工
* 因此, CubeMX 配置串口后, 代码要选择 LL 库
* 
* 1. 在串口设备结构体中增加串口接收超时标志 RxTimeout 
* 2. 采用软定时器来判断串口接收超时, 定时器回调函数(UARTx_Timeout())中默认操作为修改 RxTimeout 状态标志
* 	 主程序可根据接收超时状态查询函数 comIsRxTimeout( COM_PORT_E _ucPort )来查询帧接收状态。
* 3. 所有串口接收数据统一进 pRxBuf 缓冲区，去除安富莱1.2版本485接收每个字节执行回调函数的设定。
*    接收超时后可以根据串口用途修改回调函数(UARTx_Timeout())，调用外部回调函数及时读走一帧数据。
* 4. 所有串口配置信息集中到 UART_FIFO.h
*	5. 2023-11-25 新增宏定义 UARTBufferArea 定义串口缓存位置, InSRAM,ExtSRAM,InCCMRAM 
*		 分别对应内部SRAM,外部SRAM和内部CCMRAM
*
*********************************************************************************************************
*/

#include "UART_FIFO.h"


/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	#if UARTBufferArea==InSRAM
		static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
		static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
	#endif
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	#if UARTBufferArea==InSRAM
		static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
		static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
	#endif
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	#if UARTBufferArea==InSRAM
		static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
		static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
	#endif
#endif

#if UART4_FIFO_EN == 1
	static UART_T g_tUart4;
	#if UARTBufferArea==InSRAM
		static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
		static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
	#endif
#endif

#if UART5_FIFO_EN == 1
	static UART_T g_tUart5;
	#if UARTBufferArea==InSRAM
		static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
		static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
	#endif
#endif

#if UART6_FIFO_EN == 1
	static UART_T g_tUart6;
	#if UARTBufferArea==InSRAM
		static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* 发送缓冲区 */
		static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* 接收缓冲区 */
	#endif
#endif
		

/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	参    数: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: uart指针
*********************************************************************************************************
*/
static UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_tUart4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_tUart5;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM6)
	{
		#if UART6_FIFO_EN == 1
			return &g_tUart6;
		#else
			return 0;
		#endif
	}	
	else
	{
		Error_Handler();
		//Error_Handler(__FILE__, __LINE__);
		return 0;
	}
}



/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while (1)
		{
			__IO uint16_t usCount;

			INTX_DISABLE();
			usCount = _pUart->usTxCount;
			INTX_ENABLE();

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
			else if(usCount == _pUart->usTxBufSize)/* 数据已填满缓冲区 */
			{
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
				}  
			}
		}

		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		INTX_DISABLE();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		INTX_ENABLE();
	}

	SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);	/* 使能发送中断（缓冲区空） */
}



/*
*********************************************************************************************************
*	函 数 名: UartGetChar
*	功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*	参    数: _pUart : 串口设备
*			  _pByte : 存放读取数据的指针
*	返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	INTX_DISABLE();
	usCount = _pUart->usRxCount;
	INTX_ENABLE();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		INTX_DISABLE();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		INTX_ENABLE();
		return 1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	参    数: _ucPort: 端口号(COM1 - COM8)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

	UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	参    数: _ucPort: 端口号(COM1 - COM8)
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	函 数 名: comGetChar
*	功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*	参    数: _ucPort: 端口号(COM1 - COM8)
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}

	return UartGetChar(pUart, _pByte);
}


/*
*********************************************************************************************************
*	函 数 名: comGetOneFrame
*	功能说明: 读取串口接收缓冲区全部数据
*						串口收到最后一个字节后，如果5ms内没有数据进入，
*						则执行本函数，将串口缓冲区的数据全部取出。
*	参    数: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 字符个数
*********************************************************************************************************
*/
uint16_t  comGetOneFrame(COM_PORT_E _ucPort, uint8_t *_pstr)
{
	UART_T   *_pUart;
	uint16_t i,usCount,usRxRead;
	
	_pUart  =  ComToUart(_ucPort);	
	if (_pUart == 0)	return 0;

	INTX_DISABLE();
	usCount = _pUart->usRxCount;
	usRxRead = _pUart->usRxRead;
	INTX_ENABLE();

	//DEBUGOUT("\r\nCount = %d, RxRead = %d, RxWrite = %d", usCount, usRxRead, _pUart->usRxWrite );
	
	/* 如果读和写索引相同，则返回0 */
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
    //拷贝所有数据
    for( i = 0 ; i < usCount ; i++)
    {
        _pstr[i] = _pUart->pRxBuf[ ( usRxRead + i ) % _pUart->usRxBufSize ];
    }
		
		/* 改写FIFO读索引 */
		INTX_DISABLE();
		_pUart->usRxRead = ( usRxRead + usCount ) % _pUart->usRxBufSize;
		_pUart->usRxCount -= usCount;
		INTX_ENABLE();
		
	//DEBUGOUT("\r\nCount = %d, RxRead = %d, RxWrite = %d", usCount, _pUart->usRxRead, _pUart->usRxWrite );
		return usCount;
	}
}



/*
*********************************************************************************************************
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	参    数: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usTxWrite = 0;
	pUart->usTxRead = 0;
	pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	参    数: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usRxWrite = 0;
	pUart->usRxRead = 0;
	pUart->usRxCount = 0;
}


///* 如果是3线制RS485通信，请按如下格式编写函数， 我们仅举了 USART3作为RS485的例子 */

///*
//*********************************************************************************************************
//*	函 数 名: RS485_InitTXE
//*	功能说明: 配置RS485发送使能口线 TXE
//*	参    数: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void RS485_InitTXE(void)
//{
//	GPIO_InitTypeDef gpio_init;
//	
//	/* 打开GPIO时钟 */
//	RS485_TXEN_GPIO_CLK_ENABLE();
//	
//	/* 配置引脚为推挽输出 */
//	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;			/* 推挽输出 */
//	gpio_init.Pull = GPIO_NOPULL;					/* 上下拉电阻不使能 */
//	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;	/* GPIO速度等级 */
//	gpio_init.Pin = RS485_TXEN_PIN;
//	HAL_GPIO_Init(RS485_TXEN_GPIO_PORT, &gpio_init);	
//}

///*
//*********************************************************************************************************
//*	函 数 名: RS485_SetBaud
//*	功能说明: 修改485串口的波特率。
//*	参    数: _baud : 8倍过采样  波特率.0-12.5Mbps
//*                     16倍过采样 波特率.0-6.25Mbps
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void RS485_SetBaud(uint32_t _baud)
//{
//	comSetBaud(COM3, _baud);
//}

///*
//*********************************************************************************************************
//*	函 数 名: RS485_SendBefor
//*	功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
//*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
//*	参    数: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void RS485_SendBefor(void)
//{
//	RS485_TX_EN();	/* 切换RS485收发芯片为发送模式 */
//}

///*
//*********************************************************************************************************
//*	函 数 名: RS485_SendOver
//*	功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
//*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
//*	参    数: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void RS485_SendOver(void)
//{
//	RS485_RX_EN();	/* 切换RS485收发芯片为接收模式 */
//}

///*
//*********************************************************************************************************
//*	函 数 名: RS485_SendBuf
//*	功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
//*	参    数: _ucaBuf : 数据缓冲区
//*			  _usLen : 数据长度
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
//{
//	comSendBuf(COM3, _ucaBuf, _usLen);
//}

///*
//*********************************************************************************************************
//*	函 数 名: RS485_SendStr
//*	功能说明: 向485总线发送一个字符串，0结束。
//*	参    数: _pBuf 字符串，0结束
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void RS485_SendStr(char *_pBuf)
//{
//	RS485_SendBuf((uint8_t *)_pBuf, strlen(_pBuf));
//}

///*
//*********************************************************************************************************
//*	函 数 名: RS485_ReciveNew
//*	功能说明: 接收到新的数据
//*	参    数: _byte 接收到的新数据
//*	返 回 值: 无
//*********************************************************************************************************
//*/
////extern void MODH_ReciveNew(uint8_t _byte);
//void RS485_ReciveNew(uint8_t _byte)
//{
////	MODH_ReciveNew(_byte);
//}



/*
*********************************************************************************************************
*   函 数 名: comIsTxEmpty
*   功能说明: 判断发送缓冲区是否为空。
*   参    数:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。255,参数错
*********************************************************************************************************
*/
uint8_t comIsTxEmpty(COM_PORT_E _ucPort)
{
   UART_T *pUart;
   uint8_t Sending;
   
   pUart = ComToUart(_ucPort);
   if (pUart == 0)
   {
      return 255;
   }

   Sending = pUart->Sending;

   if (Sending != 0)
   {
      return 0;
   }
   return 1;
}



/*
*********************************************************************************************************
*	函 数 名: comIsRxTimeout
*	功能说明: 检查串行接收是否超时,已判断帧结束
*	参    数: _ucPort COM1,COM2...
*	返 回 值: 0,未超时  1,超时  255,参数错
*********************************************************************************************************
*/
uint8_t comIsRxTimeout( COM_PORT_E _ucPort )
{
		UART_T *pUart;

		pUart = ComToUart(_ucPort);
		if (pUart == 0)
		{
			return	255;		//错误
		}
		
		if( pUart->RxTimeout==1 ){
			pUart->RxTimeout = 0;
			return 1;
		}
		else return 0;
}

//// 用于MODBUS通信的帧处理函数,需在外部定义
//extern void MODS_RxTimeOut( COM_PORT_E _com );

#if UART1_FIFO_EN == 1
static void UART1_Timeout( void )
{
		g_tUart1.RxTimeout = 1;
}
static void vReciveCOM1Data(uint8_t _byte)
{
		SoftTimer_StartTimer( TMR_UART1_Timeout_ID, 5, (void *)UART1_Timeout);
}
#endif


#if UART2_FIFO_EN == 1
static void UART2_Timeout( void )
{
		g_tUart2.RxTimeout = 1;
		//MODS_RxTimeOut( COM2 );
}
static void vReciveCOM2Data(uint8_t _byte)
{
  // 启动一个定时器，5ms 。
	SoftTimer_StartTimer( TMR_UART2_Timeout_ID, 5, (void *)UART2_Timeout);
	// Modbus数据包间隔3.5个字符位，9600波特率约为3.65ms，115200波特率约为 304 us
	//SoftTimer_StartCCTimer(3, 35000000/UART2_BAUD + 1, (void *)UART2_Timeout);
}
#endif


#if UART3_FIFO_EN == 1
static void UART3_Timeout( void )
{
		g_tUart3.RxTimeout = 1;
}
static void vReciveCOM3Data(uint8_t _byte)
{
  // 启动一个定时器。
	SoftTimer_StartTimer( TMR_UART3_Timeout_ID, 5, (void *)UART3_Timeout);
	// Modbus数据包间隔3.5个字符位，9600波特率约为3.65ms，115200波特率约为 304 us
	//SoftTimer_StartCCTimer(3, 35000000/UART3_BAUD, (void *)UART3_Timeout);
}
#endif


#if UART4_FIFO_EN == 1
static void UART4_Timeout( void )
{
		g_tUart4.RxTimeout = 1;
}
static void vReciveCOM4Data(uint8_t _byte)
{
  // 启动一个定时器。
	SoftTimer_StartTimer( TMR_UART4_Timeout_ID, 5, (void *)UART4_Timeout);
}
#endif


#if UART5_FIFO_EN == 1
static void UART5_Timeout( void )
{
		g_tUart5.RxTimeout = 1;
}
static void vReciveCOM5Data(uint8_t _byte)
{
  // 启动一个定时器。
	SoftTimer_StartTimer( TMR_UART5_Timeout_ID, 5, (void *)UART5_Timeout);
}
#endif


#if UART6_FIFO_EN == 1
static void UART6_Timeout( void )
{
		g_tUart6.RxTimeout = 1;
}
static void vReciveCOM6Data(uint8_t _byte)
{
  // 启动一个定时器。
	SoftTimer_StartTimer( TMR_UART6_Timeout_ID, 5, (void *)UART6_Timeout);
}
#endif


/*
*********************************************************************************************************
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32 串口设备 */
	
	#if UARTBufferArea==InSRAM
		g_tUart1.pTxBuf = g_TxBuf1;				/* 发送缓冲区指针 */
		g_tUart1.pRxBuf = g_RxBuf1;				/* 接收缓冲区指针 */
	#elif UARTBufferArea==ExtSRAM
		g_tUart1.pTxBuf = MEM_Malloc( &Ext_SRAM, UART1_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart1.pRxBuf = MEM_Malloc( &Ext_SRAM, UART1_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#elif UARTBufferArea==InCCMRAM
		g_tUart1.pTxBuf = MEM_Malloc( &In_CCMRAM, UART1_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart1.pRxBuf = MEM_Malloc( &In_CCMRAM, UART1_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#endif
	
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart1.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart1.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart1.ReciveNew = vReciveCOM1Data;			/* 接收到新数据后的回调函数 */
	g_tUart1.Sending = 0;							/* 正在发送中标志 */
	g_tUart1.RxTimeout = 0;						/* 接收超时标志 */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 串口设备 */
	
	#if UARTBufferArea==InSRAM
		g_tUart2.pTxBuf = g_TxBuf2;				/* 发送缓冲区指针 */
		g_tUart2.pRxBuf = g_RxBuf2;				/* 接收缓冲区指针 */
	#elif UARTBufferArea==ExtSRAM
		g_tUart2.pTxBuf = MEM_Malloc( &Ext_SRAM, UART2_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart2.pRxBuf = MEM_Malloc( &Ext_SRAM, UART2_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#elif UARTBufferArea==InCCMRAM
		g_tUart2.pTxBuf = MEM_Malloc( &In_CCMRAM, UART2_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart2.pRxBuf = MEM_Malloc( &In_CCMRAM, UART2_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#endif
	
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart2.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart2.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart2.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart2.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart2.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart2.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart2.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart2.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart2.ReciveNew = vReciveCOM2Data;			/* 接收到新数据后的回调函数 */
	g_tUart2.Sending = 0;							/* 正在发送中标志 */
	g_tUart2.RxTimeout = 0;						/* 接收超时标志 */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = USART3;						/* STM32 串口设备 */
	
	#if UARTBufferArea==InSRAM
		g_tUart3.pTxBuf = g_TxBuf3;				/* 发送缓冲区指针 */
		g_tUart3.pRxBuf = g_RxBuf3;				/* 接收缓冲区指针 */
	#elif UARTBufferArea==ExtSRAM
		g_tUart3.pTxBuf = MEM_Malloc( &Ext_SRAM, UART3_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart3.pRxBuf = MEM_Malloc( &Ext_SRAM, UART3_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#elif UARTBufferArea==InCCMRAM
		g_tUart3.pTxBuf = MEM_Malloc( &In_CCMRAM, UART3_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart3.pRxBuf = MEM_Malloc( &In_CCMRAM, UART3_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#endif
	
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart3.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart3.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart3.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart3.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart3.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart3.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart3.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart3.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart3.ReciveNew = vReciveCOM3Data;			/* 接收到新数据后的回调函数 */
	g_tUart3.Sending = 0;							/* 正在发送中标志 */
	g_tUart3.RxTimeout = 0;						/* 接收超时标志 */
#endif

#if UART4_FIFO_EN == 1
	g_tUart4.uart = UART4;						/* STM32 串口设备 */
	
	#if UARTBufferArea==InSRAM
		g_tUart4.pTxBuf = g_TxBuf4;				/* 发送缓冲区指针 */
		g_tUart4.pRxBuf = g_RxBuf4;				/* 接收缓冲区指针 */
	#elif UARTBufferArea==ExtSRAM
		g_tUart4.pTxBuf = MEM_Malloc( &Ext_SRAM, UART4_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart4.pRxBuf = MEM_Malloc( &Ext_SRAM, UART4_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#elif UARTBufferArea==InCCMRAM
		g_tUart4.pTxBuf = MEM_Malloc( &In_CCMRAM, UART4_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart4.pRxBuf = MEM_Malloc( &In_CCMRAM, UART4_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#endif
	
	g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart4.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart4.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart4.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart4.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart4.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart4.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart4.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart4.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart4.ReciveNew = vReciveCOM4Data;						/* 接收到新数据后的回调函数 */
	g_tUart4.Sending = 0;						/* 正在发送中标志 */
	g_tUart4.RxTimeout = 0;						/* 接收超时标志 */
#endif

#if UART5_FIFO_EN == 1
	g_tUart5.uart = UART5;						/* STM32 串口设备 */
	
	#if UARTBufferArea==InSRAM
		g_tUart5.pTxBuf = g_TxBuf5;				/* 发送缓冲区指针 */
		g_tUart5.pRxBuf = g_RxBuf5;				/* 接收缓冲区指针 */
	#elif UARTBufferArea==ExtSRAM
		g_tUart5.pTxBuf = MEM_Malloc( &Ext_SRAM, UART5_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart5.pRxBuf = MEM_Malloc( &Ext_SRAM, UART5_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#elif UARTBufferArea==InCCMRAM
		g_tUart5.pTxBuf = MEM_Malloc( &In_CCMRAM, UART5_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart5.pRxBuf = MEM_Malloc( &In_CCMRAM, UART5_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#endif
	
	g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart5.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart5.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart5.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart5.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart5.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart5.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart5.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart5.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart5.ReciveNew = vReciveCOM5Data;						/* 接收到新数据后的回调函数 */
	g_tUart5.Sending = 0;						/* 正在发送中标志 */
	g_tUart5.RxTimeout = 0;						/* 接收超时标志 */
#endif


#if UART6_FIFO_EN == 1
	g_tUart6.uart = USART6;						/* STM32 串口设备 */
	
	#if UARTBufferArea==InSRAM
		g_tUart6.pTxBuf = g_TxBuf6;				/* 发送缓冲区指针 */
		g_tUart6.pRxBuf = g_RxBuf6;				/* 接收缓冲区指针 */
	#elif UARTBufferArea==ExtSRAM
		g_tUart6.pTxBuf = MEM_Malloc( &Ext_SRAM, UART6_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart6.pRxBuf = MEM_Malloc( &Ext_SRAM, UART6_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#elif UARTBufferArea==InCCMRAM
		g_tUart6.pTxBuf = MEM_Malloc( &In_CCMRAM, UART6_TX_BUF_SIZE );				/* 发送缓冲区指针 */
		g_tUart6.pRxBuf = MEM_Malloc( &In_CCMRAM, UART6_RX_BUF_SIZE );				/* 接收缓冲区指针 */
	#endif
	
	g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart6.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart6.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart6.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart6.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart6.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart6.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart6.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart6.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart6.ReciveNew = vReciveCOM6Data;	/* 接收到新数据后的回调函数 */
	g_tUart6.Sending = 0;						/* 正在发送中标志 */
	g_tUart6.RxTimeout = 0;						/* 接收超时标志 */
#endif
}



/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-H7开发板
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
#if UART1_FIFO_EN == 1		/* 串口1 */
	CLEAR_BIT(USART1->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
  CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* 使能RX接收中断 */
//	SET_BIT(USART1->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
#endif

#if UART2_FIFO_EN == 1		/* 串口2 */
	CLEAR_BIT(USART2->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
  CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* 使能RX接收中断 */
#endif

#if UART3_FIFO_EN == 1			/* 串口3 */
	CLEAR_BIT(USART3->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
  CLEAR_BIT(USART3->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(USART3->CR1, USART_CR1_RXNEIE);	/* 使能RX接收中断 */
#endif

#if UART4_FIFO_EN == 1			/* 串口4 TX = PC10   RX = PC11 */
	CLEAR_BIT(UART4->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
  CLEAR_BIT(UART4->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(UART4->CR1, USART_CR1_RXNEIE);	/* 使能RX接收中断 */
#endif

#if UART5_FIFO_EN == 1			/* 串口5 TX = PC12   RX = PD2 */
	CLEAR_BIT(UART5->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
  CLEAR_BIT(UART5->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(UART5->CR1, USART_CR1_RXNEIE);	/* 使能RX接收中断 */
#endif

#if UART6_FIFO_EN == 1			/* USART6 */
	CLEAR_BIT(USART6->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
  CLEAR_BIT(USART6->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(USART6->CR1, USART_CR1_RXNEIE);	/* 使能RX接收中断 */
#endif
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	
	UartVarInit();		/* 必须先初始化全局变量,再配置硬件 */

	InitHardUart();		/* 配置串口的硬件参数(波特率等) */

	//RS485_InitTXE();	/* 配置RS485芯片的发送使能硬件，配置为推挽输出 */
}



/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	参    数: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = READ_REG(_pUart->uart->SR);
	uint32_t cr1its     = READ_REG(_pUart->uart->CR1);
	uint32_t cr3its     = READ_REG(_pUart->uart->CR3);
	uint32_t errorflags = 0x00U;
	
	errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
	if (errorflags == SET)	return;
	
	
	/* 处理接收中断  */
	//if ((isrflags & USART_SR_RXNE) != RESET)
	if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;
		
		//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);
		
		ch = READ_REG(_pUart->uart->DR);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}
		
		

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
			}
		}
	}

	/* 处理发送缓冲区空中断 */
	if ( ((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			//USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

			/* 使能数据发送完毕中断 */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
			SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			_pUart->Sending = 1;
			
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	/* 数据bit位全部发送完毕的中断 */
	if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);

			/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
			
			_pUart->Sending = 0;
		}
		else
		{
			/* 正常情况下，不会进入此分支 */

			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名: UART_ISR
*	功能说明: 串口中断服务程序外部调用入口
*	参    数: com : 串口号, 1~6对应串口1~6
*	返 回 值: 无
*********************************************************************************************************
*/
void UART_ISR( uint8_t com )
{
	switch( com )
	{
		#if UART1_FIFO_EN == 1
			case 1:	UartIRQ(&g_tUart1);	break;
		#endif
		#if UART2_FIFO_EN == 1
			case 2:	UartIRQ(&g_tUart2);	break;
		#endif
		#if UART3_FIFO_EN == 1
			case 3:	UartIRQ(&g_tUart3);	break;
		#endif
		#if UART4_FIFO_EN == 1
			case 4:	UartIRQ(&g_tUart4);	break;
		#endif
		#if UART5_FIFO_EN == 1
			case 5:	UartIRQ(&g_tUart5);	break;
		#endif
		#if UART6_FIFO_EN == 1
			case 6:	UartIRQ(&g_tUart6);	break;
		#endif
			
		default:	break;
	}
}


/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
#if 1	/* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
	comSendChar(COM1, ch);
	
	return ch;
#else	/* 采用阻塞方式发送每个字符,等待数据发送完毕 */
	/* 写一个字节到USART1 */
	USART1->DR = ch;
	
	/* 等待发送结束 */
	while((USART1->SR & USART_SR_TC) == 0)
	{}
	
	return ch;
#endif
}

///*
//*********************************************************************************************************
//*	函 数 名: fgetc
//*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
//*	参    数: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//int fgetc(FILE *f)
//{

//#if 1	/* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
//	uint8_t ucData;

//	while(comGetChar(COM2, &ucData) == 0);

//	return ucData;
//#else
//	/* 等待接收到数据 */
//	while((USART1->SR & USART_SR_RXNE) == 0)
//	{}

//	return (int)USART1->DR;
//#endif
//}


