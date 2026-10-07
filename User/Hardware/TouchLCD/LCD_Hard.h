#ifndef __LCD_HARD_H
#define __LCD_HARD_H

#include "sys.h"
#include "main.h"

/********************** LCD硬件连接定义 ***********************/
//背光控制引脚定义
#define LCD_BL_On()		HAL_GPIO_WritePin( LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET )
#define LCD_BL_Off()	HAL_GPIO_WritePin( LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET )

//LCD地址结构体
typedef struct
{
	__IO uint16_t LCD_REG;
	__IO uint16_t LCD_RAM;
}LCD_TypeDef;

//根据实际连接方式设置 LCD_BASE 地址
//使用NOR/SRAM的 Bank1.sector1,地址位HADDR[27,26]=00 A18作为数据命令区分线 
//注意设置时STM32内部会右移一位对齐!
#define LCD_BASE        ((u32)(0x6C000000 | 0x00001FFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
/****************** end of LCD硬件连接定义 ********************/



/********************* 扫描方向定义 ************************/
#define L2R_U2D  0 		//从左到右,从上到下
#define L2R_D2U  1 		//从左到右,从下到上
#define R2L_U2D  2 		//从右到左,从上到下
#define R2L_D2U  3 		//从右到左,从下到上

#define U2D_L2R  4 		//从上到下,从左到右
#define U2D_R2L  5 		//从上到下,从右到左
#define D2U_L2R  6 		//从下到上,从左到右
#define D2U_R2L  7		//从下到上,从右到左
/****************** end of 扫描方向定义 ********************/



/********************* LCD设备定义 ************************/
typedef struct  //LCD结构体
{
	uint16_t	id;						//LCD ID
	uint8_t		dir;					//横屏还是竖屏控制：0，竖屏；1，横屏。
	uint16_t	width;				//LCD 宽度
	uint16_t	height;				//LCD 高度
	uint16_t	wramcmd;			//开始写gram指令
	uint16_t	setxcmd;			//设置x坐标指令
	uint16_t	setycmd;			//设置y坐标指令	
	uint16_t	POINT_COLOR;	//画笔颜色
	uint16_t	BACK_COLOR; 	//背景色
}LCDDevTypeDef;
/****************** end of LCD设备定义 ********************/


/********************* 初始化 1963 芯片需要用到的参数 ************************/

#define SSD_HOR_RESOLUTION		800		//LCD水平分辨率
#define SSD_VER_RESOLUTION		480		//LCD垂直分辨率

#define SSD_HOR_PULSE_WIDTH		1		//水平脉宽
#define SSD_HOR_BACK_PORCH		46		//水平前廊
#define SSD_HOR_FRONT_PORCH		210		//水平后廊

#define SSD_VER_PULSE_WIDTH		1		//垂直脉宽
#define SSD_VER_BACK_PORCH		23		//垂直前廊
#define SSD_VER_FRONT_PORCH		22		//垂直前廊

//如下几个参数，自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

/****************** end of 初始化 1963 芯片需要用到的参数 ********************/


extern LCDDevTypeDef lcddev;		//显示器设备

void 	LCD_WR_REG(vu16 regval);
void 	LCD_WR_DATA(vu16 data);
u16 	LCD_RD_DATA(void);
void 	LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue);
u16 	LCD_ReadReg(u16 LCD_Reg);
void 	LCD_WriteRAM_Prepare(void);
void 	LCD_WriteRAM(u16 RGB_Code);
u16 	LCD_BGR2RGB(u16 c);
void 	LCD_DisplayOn(void);
void 	LCD_DisplayOff(void);
void 	LCD_Display_Dir(u8 dir);
void 	LCD_SSD_BackLightSet(u8 pwm);

void 	LCD_SetCursor(u16 Xpos, u16 Ypos);
u32 	LCD_ReadPoint(u16 x,u16 y);
void 	LCD_DrawPoint( u16 x, u16 y, u16 color);
void 	LCD_Fast_DrawPoint(u16 x,u16 y,u32 color);
void 	LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);

void 	LCD_Hard_Init(void);


#endif  
	 
	 



