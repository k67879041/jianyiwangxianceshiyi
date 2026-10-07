/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : LED.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __LED_H
#define __LED_H

#include "main.h"
#include "sys.h"

enum{
	LED1,
	LED2,
	
	LedAmount,
};

typedef struct{
	GPIO_TypeDef	*LED_Port;
	uint16_t			LED_Pin;		
	GPIO_PinState	LED_OnSta;	//LED亮灯状态, GPIO_PIN_SET / GPIO_PIN_RESET
	GPIO_PinState	LED_OffSta;	//LED灭灯状态, GPIO_PIN_SET / GPIO_PIN_RESET
}LedHardTypeDef;


typedef	struct{
	uint8_t 	LedPeriod;		//Led闪烁周期,LedPeriod*100ms
	uint8_t 	LedLightTime;	//Led点亮时间,LedLightTime*100ms =0灭,=LedPeriod全亮
	uint8_t 	LedFlashCnt;	//Led闪烁次数,1~99, =100周期闪烁,=0灭灯
	uint8_t 	LedCount;			//Led计数器,0~LedPeriod-1,<LedLightTime亮灯,>=LedLightTime灭灯
	uint8_t 	LEDChangeMode;//Led切换模式,0:直接切换,1:保留上次模式,完成新的闪烁次数后切换回原来的闪烁模式
}LedFlashTypeDef;


void LEDFunSet( uint8_t _ID,uint8_t _period,uint8_t _lighttime,uint8_t _flashtime,uint8_t _mode );
void Task_LED( void );

extern  uint8_t LED_upload;
#endif

/*******************************************(END OF FILE) ************************************************/
