/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : UART_FIFO.h
*	版    本 : V1.0
*	说    明 : 头文件
		启明欣欣407开发板串口分配：
		【串口1】 带CH340芯片,用于上位机调试
			PA9      ------> USART1_TX
			PA10     ------> USART1_RX			
		【串口2】 未引出
		【串口3】 
			PB10     ------> USART3_TX
			PB11     ------> USART3_RX
		【串口4】 未引出
		【串口5】 未引出
		【串口6】
			PC6	     ------> USART6_TX
			PC7      ------> USART6_RX
*
*	注意：串口使能/失能后要同步修改 SoftTimer.h 中枚举 SoftTimerID 。释放/屏蔽相关软定时器
*********************************************************************************************************
*/

#ifndef _USART_FIFO_H_
#define _USART_FIFO_H_

#include <stdio.h>
#include "sys.h"



/* 定义使能的串口, 0 表示不使能（不增加代码大小）， 1表示使能 */
#define	UART1_FIFO_EN			1
#define	UART2_FIFO_EN			0
#define	UART3_FIFO_EN			0
#define	UART4_FIFO_EN			0
#define	UART5_FIFO_EN			0
#define	UART6_FIFO_EN			0


/* 定义串口缓存存放位置, 需与内存管理 malloc 配合 */
#define InSRAM						1			// 内部 SRAM 由编译器自动分配
#define ExtSRAM						2			// 外部扩展 SRAM 由 malloc 函数分配
#define InCCMRAM					3			// 内部 CCMRAM 由 malloc 函数分配
#define	UARTBufferArea		InCCMRAM

//采用两线制485通信，此部分代码用不上
///* PB2 控制RS485芯片的发送使能 */
//#define RS485_TXEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
//#define RS485_TXEN_GPIO_PORT              GPIOB
//#define RS485_TXEN_PIN                    GPIO_PIN_2

//#define RS485_RX_EN()	RS485_TXEN_GPIO_PORT->BSRR = (uint32_t)RS485_TXEN_PIN << 16U
//#define RS485_TX_EN()	RS485_TXEN_GPIO_PORT->BSRR = RS485_TXEN_PIN

/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART3 */
	COM4 = 3,	/* UART4 */
	COM5 = 4,	/* UART5 */
	COM6 = 5,	/* USART6 */
}COM_PORT_E;



/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
	#define UART1_TX_BUF_SIZE								 1*1024
	#define UART1_RX_BUF_SIZE								42*1024
#endif

#if UART2_FIFO_EN == 1
	#define UART2_TX_BUF_SIZE								256
	#define UART2_RX_BUF_SIZE								256
#endif

#if UART3_FIFO_EN == 1
	#define UART3_TX_BUF_SIZE								256
	#define UART3_RX_BUF_SIZE								256
#endif

#if UART4_FIFO_EN == 1
	#define UART4_TX_BUF_SIZE								256
	#define UART4_RX_BUF_SIZE								256
#endif

#if UART5_FIFO_EN == 1
	#define UART5_TX_BUF_SIZE								256
	#define UART5_RX_BUF_SIZE								256
#endif

#if UART6_FIFO_EN == 1
	#define UART6_TX_BUF_SIZE								256
	#define UART6_RX_BUF_SIZE								256
#endif








/* 串口设备结构体 */
typedef struct
{
    USART_TypeDef *uart;				/* STM32内部串口设备指针 */
	
		uint8_t				RxTimeout;		/* 接收超时标志 */
		uint8_t 			Sending;			/* 正在发送中 */
	
    uint8_t 			*pTxBuf;			/* 发送缓冲区 */
    uint8_t 			*pRxBuf;			/* 接收缓冲区 */
    uint16_t 			usTxBufSize;	/* 发送缓冲区大小 */
    uint16_t 			usRxBufSize;	/* 接收缓冲区大小 */
	
    __IO uint16_t usTxWrite;		/* 发送缓冲区写指针 */
    __IO uint16_t usTxRead;			/* 发送缓冲区读指针 */
    __IO uint16_t usTxCount;		/* 等待发送的数据个数 */

    __IO uint16_t usRxWrite;		/* 接收缓冲区写指针 */
    __IO uint16_t usRxRead;			/* 接收缓冲区读指针 */
    __IO uint16_t usRxCount;		/* 还未读取的新数据个数 */

    void (*SendBefor)(void); 		/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
    void (*SendOver)(void); 		/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
    void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
}UART_T;


void			comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void			comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t		comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void			comRBackChar(COM_PORT_E _ucPort, uint16_t _ulCount);	//该函数有问题，待解决
uint16_t	comGetOneFrame(COM_PORT_E _ucPort, uint8_t *_pstr);

void			comClearTxFifo(COM_PORT_E _ucPort);
void			comClearRxFifo(COM_PORT_E _ucPort);

//void		RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
//void		RS485_SendStr(char *_pBuf);

uint8_t 	comIsTxEmpty(COM_PORT_E _ucPort);
uint8_t 	comIsRxTimeout( COM_PORT_E _ucPort );

void			bsp_InitUart(void);

void 			UART_ISR( uint8_t com );

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
