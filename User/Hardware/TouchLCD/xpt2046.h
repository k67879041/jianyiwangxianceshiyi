#ifndef __XPT2046_H__
#define __XPT2046_H__

#include "sys.h"
#include "main.h"


/*
触摸事件处理方式
1: 采用专门的外部函数处理,适用于软键盘,要求函数原型为 
		void Touch_Action( MSG_CODE_X T_msg, uint16_t T_x, uint16_t T_y );
0: 发送消息,由主程序处理
*/
#define	TOUCH_ACTION	1
#if TOUCH_ACTION==1
	extern	void Touch_Action( MSG_CODE_X T_msg, uint16_t T_x, uint16_t T_y );
#endif


#define	TOUCH_SHORT_TIME			50		//短按上限时间 X*10 ms
#define	TOUCH_LONG_TIME				52		//长按下限时间 X*10 ms 必须大于KEY_SHORT_TIME
#define	TOUCH_LONG_STEP_TIME	20		//长按下处理时间间隔 X*10 ms

typedef struct{
	uint16_t	x;
	uint16_t	y;
}CoordinateTypeDef;

// 校准因子结构体 
typedef struct         
{
	long double An,  		 //注:sizeof(long double) = 8
              Bn,     
              Cn,   
              Dn,    
              En,    
              Fn,     
              Divider;	
}XPT2046_Calibration;

typedef struct{
	uint8_t CMD_RDX;
	uint8_t CMD_RDY;
	CoordinateTypeDef	Coord_S;	// 起始坐标
	CoordinateTypeDef	Coord_C;	// 当前坐标
	uint16_t PressTime;					// 按下时间
	float dX_X;			// 坐标计算参数
  float dX_Y;
  float dX;
  float dY_X;
  float dY_Y;
  float dY;
}TouchDevTypeDef;

//电阻屏芯片连接引脚, GPIO配置由 CubeMX 生成, 端口、引脚宏定义在 main.h 中
//#define PEN  		PFin(11)  	//T_PEN
//#define DOUT 		PBin(2)   	//T_MISO
//#define TDIN 		PBout(1)  	//T_MOSI
//#define TCLK 		PAout(5)  	//T_CLK
//#define TCS  		PBout(0)  	//T_CS
#define T_CLK_H()		HAL_GPIO_WritePin(T_CLK_GPIO_Port,T_CLK_Pin,GPIO_PIN_SET)
#define T_CLK_L()		HAL_GPIO_WritePin(T_CLK_GPIO_Port,T_CLK_Pin,GPIO_PIN_RESET)
#define T_MOSI_H()	HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,GPIO_PIN_SET)
#define T_MOSI_L()	HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,GPIO_PIN_RESET)
#define T_CS_H()		HAL_GPIO_WritePin(T_CS_GPIO_Port,T_CS_Pin,GPIO_PIN_SET)
#define T_CS_L()		HAL_GPIO_WritePin(T_CS_GPIO_Port,T_CS_Pin,GPIO_PIN_RESET)
#define T_PEN()			HAL_GPIO_ReadPin(T_PEN_GPIO_Port,T_PEN_Pin)
#define T_MISO()		HAL_GPIO_ReadPin(T_MISO_GPIO_Port,T_MISO_Pin)


extern TouchDevTypeDef Touch_dev;

uint8_t XPT2046_Scan( uint8_t type, CoordinateTypeDef *coordinate );
uint8_t XPT2046_Calculate_CalibrationFactor ( CoordinateTypeDef * pDisplayCoordinate, CoordinateTypeDef * pScreenSample, XPT2046_Calibration * pCalibrationFactor );
void 		XPT2046_Init( uint8_t lcddir );
void		Task_Touch( void );


#endif

















