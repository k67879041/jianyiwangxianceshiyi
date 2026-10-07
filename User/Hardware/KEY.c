/*
*********************************************************************************************************
*
*	模块名称 :  按键驱动模块,适用于正点原子阿波罗STM32F429开发板
*	文件名称 :  KEY.c
*	版    本 :  V1.0
* 日    期 :	2023-07-28
*	说    明 :  
*						1.键硬件配置由 CubeMX 完成，要求在 CubeMX 中自定义灯引脚名称为 KEYUP, KEYDOWN, KEYLEFT 和 KEYRIGHT
*							在生成的 main.h 文件中会有 KEYUP_Pin, KEYUP_GPIO_Port, ...  宏定义
*							所以本驱动需要包含 main.h 头文件
*						2.提供灯定时调度任务函数, 此函数需定时调用, 建议调用周期为10ms。
*********************************************************************************************************
*/

#include "KEY.h"


/* KEY 硬件注册, 此处用到的 KEY 引脚宏定义需与 main.h 保持一致 
								 硬件顺序须与 KEY.h 中枚举保持一致            */
static const KeyHardTypeDef	KEYHARD[ KeyAmount ] = {
	KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_RESET, KEY1,
	KEY2_GPIO_Port, KEY2_Pin, GPIO_PIN_RESET, KEY2,
};


/*
*********************************************************************************************************
*	函 数 名: KeyScan
*	功能说明: 点亮指定的LED指示灯。
*	参    数: 无
*	返 回 值: 键值
*********************************************************************************************************
*/
static uint8_t KeyScan( void )
{
	uint8_t i,ret;
	
	ret = KEYNONE;
	
	for( i=0;i<KeyAmount;i++ )
	{
		if( HAL_GPIO_ReadPin( KEYHARD[i].KEY_Port, KEYHARD[i].KEY_Pin )==KEYHARD[i].KEY_DownSta ){
			ret = KEYHARD[i].KEY_Value;
			break;
		}
	}
	
	return ret;
}


/*
*********************************************************************************************************
*	函 数 名: Task_KEY
*	功能说明: 按键扫描任务函数, 定时调度, 建议调度周期 10ms。
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Task_KEY( void )
{	
	static uint8_t  keytemp[3];			//键值缓存
	static uint16_t presstime=0;		//记录按键按下时间
	//static uint16_t releasetime=0;	//记录按键释放时间,用于判断双击
	
	if( !SoftTimer_CheckTimer(TMR_KEYSCAN_ID) )	return;
	
	keytemp[2] = keytemp[1];
	keytemp[1] = keytemp[0];
	keytemp[0] = KeyScan();
	
	//最近两次按键相同才处理，滤除按键抖动
	if( keytemp[0]==keytemp[1] )
	{		
		//刚按下
		if( keytemp[2]==KEYNONE && keytemp[0]!=KEYNONE )
		{
			presstime = 0;
			vPutMsg( MSG_KEY_PRESS, keytemp[0] );
		}
		
		//按下保持
		else if( keytemp[2]!=KEYNONE && keytemp[0]!=KEYNONE )
		{
			presstime++;
			if( presstime>KEY_LONG_TIME && (presstime-KEY_LONG_TIME)%KEY_LONG_STEP_TIME==0 ){
				vPutMsg( MSG_KEY_LONGHOLD, keytemp[0] );
			}
		}
		
		//释放
		else if( keytemp[2]!=KEYNONE && keytemp[0]==KEYNONE )
		{
			if( presstime<KEY_SHORT_TIME ){
				vPutMsg( MSG_KEY_SHORTRELEASE, keytemp[2] );
			}
			else if( presstime>=KEY_LONG_TIME ){
				vPutMsg( MSG_KEY_LONGRELEASE, keytemp[2] );
			}
		}
	}
}


/********************************************** (END OF FILE) ***************************************************/
