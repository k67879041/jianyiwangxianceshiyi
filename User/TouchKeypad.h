#ifndef __TOUCHKEYPAD_H
#define __TOUCHKEYPAD_H

#include <string.h>
#include "sys.h"
#include "TouchLCD.h"

/***************************************
按键尺寸
1. 按键文字尺寸12*6,  按键最小尺寸16*8,  按键最小间隔2
2. 按键文字尺寸16*8,  按键最小尺寸24*12, 按键最小间隔3
3. 按键文字尺寸24*12, 按键最小尺寸32*16, 按键最小间隔4
4. 按键文字尺寸32*16, 按键最小尺寸40*20, 按键最小间隔5
5. 按键文字尺寸48*24, 按键最小尺寸56*28, 按键最小间隔7
***************************************/
#define TOUCHKEY_SIZE						3

// 键盘颜色
#define	KeypadBackColor					LGRAY
#define	KeypadEdgeColor					BLACK
#define KEYReleaseBackColor			WHITE
#define KEYReleasePointColor		BLACK
#define KEYPressBackColor				GREEN
#define KEYPressPointColor			WHITE
#define KEYSecondBackColor			GREEN

// 定义特殊键值
#define	KEY_CLEAR				0x11
#define	KEY_CAPS				0x14
#define	KEY_BACKSPACE		0x08

typedef enum
{
	KEYPAD_NONE = 0,		// 0. 无键盘
	KEYPAD_PHONE,				// 1. 手机键盘(12键)
	KEYPAD_CALC,				// 2. 计算器键盘(16键)
	KEYPAD_ENG,					// 3. 英文键盘(35键)
	KEYPAD_ENGCAPS,			// 4. 英文大写键盘(35键)
	KEYPAD_SYMBOL,			// 5. 符号键盘(未完成)	
	DIY_KEYPAD,

	KEYPAD_NUM,					// 触摸键盘数量
}KeypadType;


//触摸按键
typedef struct
{
	uint8_t		T_sta;		//触摸状态 0释放 1按下 2长按
	uint8_t		T_key;		//键值
	uint16_t	sx;				//起始坐标
	uint16_t	sy;
	uint16_t	width;		//大小
	uint16_t	height;
	uint16_t	ReleaseBackColor;		//释放状态背景色
	uint16_t	ReleasePointColor;	//释放状态画笔颜色
	uint16_t	PressBackColor;			//按下状态背景色
	uint16_t	PressPointColor;		//按下状态画笔颜色
	uint8_t		mess0[10];					//释放状态显示内容
	uint8_t		mess1[10];					//按下状态显示内容
}TouchkeyTypeDef;

// 触摸键盘
typedef struct{
	KeypadType	Type;
	uint16_t		sx;						// 起始坐标
	uint16_t		sy;
	uint16_t		width;				//大小
	uint16_t		height;
	TouchkeyTypeDef	Key[40];
}KeypadTypeDef;

extern	KeypadTypeDef *CurrentKeypad;

void		TouchKeypad_Select( KeypadType keypadsel );
void		TouchKeypad_Init( void );
void		Touch_Action( MSG_CODE_X T_msg, uint16_t T_x, uint16_t T_y );
#endif  
	 
	 



