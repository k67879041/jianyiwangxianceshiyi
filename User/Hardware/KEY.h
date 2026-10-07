/*
*********************************************************************************************************
*
*	模块名称 : 按键驱动模块
*	文件名称 : KEY.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __KEY_H
#define __KEY_H

#include "main.h"
#include "sys.h"



/* 键值定义 */
#define KeyAmount 2
enum{
	KEYNONE,
	KEY1,
	KEY2,
};

/* 键硬件结构定义 */
typedef struct{
	GPIO_TypeDef	*KEY_Port;
	uint16_t			KEY_Pin;		
	GPIO_PinState	KEY_DownSta;	//键按下状态, GPIO_PIN_SET / GPIO_PIN_RESET
	uint8_t				KEY_Value;		//返回键值
}KeyHardTypeDef;

/* 键状态判断参数定义 */
#define	KEY_SHORT_TIME			50		//短按上限时间 X*10 ms
#define	KEY_LONG_TIME				52		//长按下限时间 X*10 ms 必须大于KEY_SHORT_TIME
#define	KEY_LONG_STEP_TIME	20		//长按下处理时间间隔 X*10 ms
#define KEYBUFFERSIZE				5			//按键缓冲区大小 X*4Byte

void Task_KEY( void );

#endif

/*******************************************(END OF FILE) ************************************************/
