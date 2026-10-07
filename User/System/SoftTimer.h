/*
*********************************************************************************************************
*
*	模块名称 : 软定时器模块
*	文件名称 : SoftTimer.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __SOFTTIMER_H
#define __SOFTTIMER_H

#include "sys.h"

/*
	定义用于CC定时器的TIM，以使 TIM2 - TIM5
*/
//#define USE_TIM2
//#define USE_TIM3
//#define USE_TIM4
//#define USE_TIM5


typedef	enum{
	TMR_TMP_ID,											/* 临时ID,用于测试 */
	TMR_KEYSCAN_ID,									/* 定时扫描键盘,重复定时 10ms */
	TMR_TOUCH_ID,										/* 定时扫描触屏,重复定时 10ms */
	TMR_LED_ID,											/* 定时调度LED任务,重复定时 50ms */	
	
	TMR_UART1_Timeout_ID,						/* 串口1接收超时判断,单次定时 5ms */
	TMR_Time_count_ID,
	TMR_AD9959_FixedStepSweep_ID,
	TMR_fresh_AD_ID,
//	TMR_UART2_Timeout_ID,						/* 串口2接收超时判断,单次定时 5ms */
//	TMR_UART3_Timeout_ID,						/* 串口3接收超时判断,单次定时 5ms */
//	TMR_UART4_Timeout_ID,						/* 串口4接收超时判断,单次定时 5ms */
//	TMR_UART5_Timeout_ID,						/* 串口5接收超时判断,单次定时 5ms */
//	TMR_UART6_Timeout_ID,						/* 串口6接收超时判断,单次定时 5ms */
	
//	TMR_DAC_INTERVAL_ID,						/* DAC输出间隔定时,单次定时 1s */
	
	TMR_COUNT,											/* 软件定时器的个数,必须定义在枚举最后位置 */
}SoftTimerID;

typedef enum
{
    TMR_ONCE_MODE = 0,		/* 一次工作模式 */
    TMR_AUTO_MODE = 1			/* 自动定时工作模式 */
}TMR_MODE_E;

/* 定时器结构体 */
typedef struct
{
	volatile TMR_MODE_E 	Mode;			/* 计数器模式,1次性 */
	volatile uint8_t 			Flag;			/* 定时到达标志  */
	volatile uint32_t 		Count;		/* 计数器 */
	volatile uint32_t 		PreLoad;	/* 计数器预装值 */
	void (*CallBack)(void);					/* 定时时间到后执行的回调函数 */
}SOFT_TMR;

/* 提供给其他C文件调用的函数 */
void 			SoftTimer_Init(void);
void 			SoftTimer_StartTimer(SoftTimerID _id, uint32_t _period, void *_pCallback );
void 			SoftTimer_StartAutoTimer(SoftTimerID _id, uint32_t _period);
void 			SoftTimer_StopTimer(SoftTimerID _id);
uint8_t 	SoftTimer_CheckTimer(SoftTimerID _id);
uint32_t 	SoftTimer_GetRunTime(void);
uint32_t 	SoftTimer_CheckRunTime(uint32_t _LastTime);

void			SoftTimer_ISR(void);

#if defined (USE_TIM2) || defined (USE_TIM3)  || defined (USE_TIM4)	|| defined (USE_TIM5)
	void 		SoftTimer_InitCCTimer(void);
	void 		SoftTimer_StartCCTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);
#endif

#endif

/******************************************(END OF FILE) *****************************************/
