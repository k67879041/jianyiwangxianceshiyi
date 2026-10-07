/*
*********************************************************************************************************
*
*	模块名称 :  LED指示灯驱动模块,适用于正点原子阿波罗STM32F429开发板
*	文件名称 :  LED.c
*	版    本 :  V1.0
* 日    期 :	2023-07-03
*	说    明 :  驱动LED指示灯
*						1.LED灯硬件配置由 CubeMX 完成，要求在 CubeMX 中自定义灯引脚名称为 LEDRED 和 LEDGREEN
*							在生成的 main.h 文件中会有 LEDRED_Pin, LEDRED_GPIO_Port, LEDGREEN_Pin, LEDGREEN_GPIO_Port 宏定义
*							所以本驱动需要包含 main.h 头文件
*						2.提供灯定时调度任务函数, 及基于此的闪烁方式配置函数, 可以自定义闪烁周期、亮灭比值及闪烁次数等
*							此函数需定时调用, 建议调用周期为50ms。
*********************************************************************************************************
*/

#include "LED.h"
#include "TouchLCD.h"

static LedFlashTypeDef LED[ LedAmount ];
static LedFlashTypeDef LEDTmp[ LedAmount ];
uint8_t LED_upload;
/* LED 硬件注册, 此处用到的LED引脚宏定义需与 main.h 保持一致 
								 硬件顺序须与 LED.h 中枚举保持一致            */
static const LedHardTypeDef	LEDHARD[ LedAmount ] = {
	LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET, GPIO_PIN_SET,
	LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET, GPIO_PIN_SET,
};


/*
*********************************************************************************************************
*	函 数 名: LedOn
*	功能说明: 点亮指定的LED指示灯。
*	参    数:  _LedID : 指示灯序号
*	返 回 值: 无
*********************************************************************************************************
*/
void LedOn( uint8_t _LedID )
{
	if( _LedID>=LedAmount )	return;
	
	HAL_GPIO_WritePin( LEDHARD[_LedID].LED_Port, LEDHARD[_LedID].LED_Pin, LEDHARD[_LedID].LED_OnSta );
	//LEDHARD[_LedID].LED_Port->BSRR = LEDHARD[_LedID].LED_Pin<<16;
}

/*
*********************************************************************************************************
*	函 数 名: LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	参    数:  _LedID : 指示灯序号
*	返 回 值: 无
*********************************************************************************************************
*/
static void LedOff( uint8_t _LedID )
{
	if( _LedID>=LedAmount )	return;
	
	HAL_GPIO_WritePin( LEDHARD[_LedID].LED_Port, LEDHARD[_LedID].LED_Pin, LEDHARD[_LedID].LED_OffSta );
	//LEDHARD[_LedID].LED_Port->BSRR = LEDHARD[_LedID].LED_Pin;
}

/*
*********************************************************************************************************
*	函 数 名: LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	参    数:  _LedID : 指示灯序号，范围 1
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void LedToggle( uint8_t _LedID )
{
	if( _LedID>=LedAmount )	return;
	
	HAL_GPIO_TogglePin( LEDHARD[_LedID].LED_Port, LEDHARD[_LedID].LED_Pin );
	//LEDHARD[_LedID].LED_Port->ODR ^= LEDHARD[_LedID].LED_Pin;
}


/*
*********************************************************************************************************
*	函 数 名: LEDFunSet
*	功能说明: 设置Led灯工作方式。
*	参    数:  _ID: 指示灯序号
*						 _period:   闪烁周期,x*50ms
*						 _lighttime:点亮时间,x*50ms =0灭,=_period全亮
*						 _flashcnt: 闪烁次数,1~99, =100一直闪烁,=0灭灯
*	返 回 值: 无
*********************************************************************************************************
*/
void LEDFunSet( uint8_t _ID,uint8_t _period,uint8_t _lighttime,uint8_t _flashcnt,uint8_t _mode )
{
	if(_ID>=LedAmount) return;
	if(_lighttime>_period) return;
	
	LED[_ID].LEDChangeMode = _mode;
	
	if(_mode==1 && LEDTmp[_ID].LEDChangeMode==0)
	{
		LEDTmp[_ID].LedCount 			= LED[_ID].LedCount;
		LEDTmp[_ID].LedPeriod 		= LED[_ID].LedPeriod;
		LEDTmp[_ID].LedLightTime 	= LED[_ID].LedLightTime;
		LEDTmp[_ID].LedFlashCnt 	= LED[_ID].LedFlashCnt;
		LEDTmp[_ID].LEDChangeMode = 1;	//已保存上次配置,在恢复之前不再存储新的配置
	}
	
	LED[_ID].LedCount = 0;
	LED[_ID].LedFlashCnt = _flashcnt;
	LED[_ID].LedLightTime = _lighttime;
	LED[_ID].LedPeriod = _period;
}



/*
*********************************************************************************************************
*	函 数 名: Task_LED
*	功能说明: Led任务函数, 定时调度, 建议调度周期 50ms。
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Task_LED( void )
{
	uint8_t i;
//	static uint8_t LED_old[LedAmount];
	
	
	if( !SoftTimer_CheckTimer(TMR_LED_ID) )	return;
	
	for(i=0;i<LedAmount;i++)
	{
//		if((LED_old[i]!=LED[i].LedPeriod)||!LED[i].LedPeriod)
//		 {
//			if(LED_upload)
//			{
//			  if(i==0&&LED[i].LedPeriod==20) sprintf((char*)str,"LED%d:%d hz",(int)(i+1),(int)(20/LED[i].LedPeriod));
//				else if(LED[i].LedPeriod) sprintf((char*)str,"LED%d:%d hz    ",(int)(i+1),(int)(20/LED[i].LedPeriod));
//				else if(1) sprintf((char*)str,"LED%d:OFF     ",(int)(i+1));
//				
//				LCD_Show_Str(0,32*i,240,320,(uint8_t *)str,32,0);
//				if(i==0&&LED[i].LedPeriod==20) LCD_Show_Str(144,16,240,320,(uint8_t *)"（默认）",16,0);
//			}
//			LED_old[i]=LED[i].LedPeriod;
//		 }
		if(LED[i].LedPeriod!=0)
		{
			LED[i].LedCount++;
			
			if(LED[i].LedCount==LED[i].LedLightTime)	LedOff( i );
			
			else if(LED[i].LedCount==LED[i].LedPeriod)
			{				
				LED[i].LedCount = 0;
				
				if( LED[i].LedFlashCnt==100 ) LedOn( i );
				
				else if( LED[i].LedFlashCnt>0 && LED[i].LedFlashCnt<100 )
				{
					LedOn( i );
					LED[i].LedFlashCnt--;
				}
				
				else if( LED[i].LedFlashCnt==0 )
				{
					if( LED[i].LEDChangeMode==1 )
					{
						LED[i].LedCount 			= LEDTmp[i].LedCount;
						LED[i].LedPeriod 			= LEDTmp[i].LedPeriod;
						LED[i].LedLightTime 	= LEDTmp[i].LedLightTime;
						LED[i].LedFlashCnt 		= LEDTmp[i].LedFlashCnt;
						LED[i].LEDChangeMode	= 0;
						
						LEDTmp[i].LedCount 			= 0;
						LEDTmp[i].LedPeriod 		= 0;
						LEDTmp[i].LedLightTime 	= 0;
						LEDTmp[i].LedFlashCnt 	= 0;
						LEDTmp[i].LEDChangeMode = 0;
					}
					else{
						LED[i].LedCount 			= 0;
						LED[i].LedPeriod 			= 0;
						LED[i].LedLightTime 	= 0;
						LED[i].LedFlashCnt 		= 0;
						LED[i].LEDChangeMode	= 0;
					}
				}
			}
		}
	}
}


/********************************************** (END OF FILE) ***************************************************/
