#include "sys.h"

//时钟系统配置函数
//Fvco=Fs*(plln/pllm);
//SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//SYSCLK:系统时钟频率
//Fusb:USB,SDIO,RNG等的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等. 
//plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

//外部晶振为25M的时候,推荐值:plln=360,pllm=25,pllp=2,pllq=8.
//得到:Fvco=25*(360/25)=360Mhz
//     SYSCLK=360/2=180Mhz
//     Fusb=360/8=45Mhz
//返回值:0,成功;1,失败
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
    __HAL_RCC_PWR_CLK_ENABLE(); //使能PWR时钟
    
    //下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    //时使性能与功耗实现平衡，此功能只有STM32F42xx和STM32F43xx器件有，
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//设置调压器输出电压级别1
    
    RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;    //时钟源为HSE
    RCC_OscInitStructure.HSEState=RCC_HSE_ON;                      //打开HSE
    RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;//打开PLL
    RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;//PLL时钟源选择HSE
    RCC_OscInitStructure.PLL.PLLM=pllm; //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
    RCC_OscInitStructure.PLL.PLLN=plln; //主PLL倍频系数(PLL倍频),取值范围:64~432.  
    RCC_OscInitStructure.PLL.PLLP=pllp; //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
    RCC_OscInitStructure.PLL.PLLQ=pllq; //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
    ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//初始化
	
    if(ret!=HAL_OK) while(1);
    
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
    ret = HAL_PWREx_EnableOverDrive(); //开启Over-Driver功能
    if(ret!=HAL_OK) while(1);
#endif
    
    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
    RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;//AHB分频系数为1
    RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4; //APB1分频系数为4
    RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2; //APB2分频系数为2
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_5);//同时设置FLASH延时周期为5WS，也就是6个CPU周期。
		
    if(ret!=HAL_OK) while(1);
}


/*
*********************************************************************************************************
*    函 数 名: Delay_us
*    功能说明: us级延迟。 必须在systick定时器启动后才能调用此函数。
*    形    参:  n : 延迟长度，单位1 us
*    返 回 值: 无
*********************************************************************************************************
*/
void Delay_us(uint32_t n)
{
	uint32_t ticks;
	uint32_t told;
	uint32_t tnow;
	uint32_t tcnt = 0;
	uint32_t reload;


	reload = SysTick->LOAD;
	ticks = n * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */

	tcnt = 0;
	told = SysTick->VAL;             /* 刚进入时的计数器值 */

	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			/* SYSTICK是一个递减的计数器 */
			if (tnow < told)
			{
				tcnt += told - tnow;
			}
			/* 重新装载递减 */
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;

			/* 时间超过/等于要延迟的时间,则退出 */
			if (tcnt >= ticks)
			{
				break;
			}
		}
	}
}



/*
*********************************************************************************************************
*    函 数 名: Delay_ms
*    功能说明: ms级延迟。 必须在systick定时器启动后才能调用此函数。
*    形    参:  n : 延迟长度，单位1 ms
*    返 回 值: 无
*********************************************************************************************************
*/
void Delay_ms(uint32_t n)
{
	uint32_t ticks;
	uint32_t told;
	uint32_t tnow;
	uint32_t tcnt = 0;
	uint32_t reload;


	reload = SysTick->LOAD;
	ticks = n * (SystemCoreClock / 1000);	 /* 需要的节拍数 */

	tcnt = 0;
	told = SysTick->VAL;             /* 刚进入时的计数器值 */

	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			/* SYSTICK是一个递减的计数器 */
			if (tnow < told)
			{
				tcnt += (told - tnow);
			}
			/* 重新装载递减 */
			else
			{
				tcnt += (reload - tnow + told);
			}
			told = tnow;

			/* 时间超过/等于要延迟的时间,则退出 */
			if (tcnt >= ticks)
			{
				break;
			}
		}
	}
}




#ifdef  USE_FULL_ASSERT
//当编译提示出错的时候此函数用来报告错误的文件和所在行
//file：指向源文件
//line：指向在文件中的行数
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}


//获取MCU ID,未测试
void GetMCUID( u8* pID )
{
	u8 i;
	
	for( i=0;i<12;i++ )
	{
		pID[i] = *(unsigned char *)( MCU_IdAddr + i );
	}
}

//获取复位方式，未测试
ResetType GetResetFlag(void)
{
    ResetType ucFlag;

    //独立看门狗复位标志
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
    {
        ucFlag = IWDGRST ;
    }
    //窗口看门狗复位标志
    else if( __HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
    {
        ucFlag = WWDGRST ;
    }
    //软件复位标志
    else if( __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
    {
        ucFlag = SFTRST ;
    }
    //上电复位标志
    else if( __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
        ucFlag = PORRST ;
    }
    //按键复位标志
    else if( __HAL_RCC_GET_FLAG( RCC_FLAG_PINRST  ) != RESET)
    {
        ucFlag = PINRST ;
    }
    //低电量复位标志
    else if( __HAL_RCC_GET_FLAG( RCC_FLAG_LPWRRST  ) != RESET)
    {
        ucFlag = LPWRRST ;
    }

    __HAL_RCC_CLEAR_RESET_FLAGS();//清除复位状态标志位
    return ucFlag  ;
}

//软件复位，未测试
void Sys_Soft_Reset(void)
{
	NVIC_SystemReset();
  //  SCB->AIRCR = 0X05FA0000 | (uint32_t)0x04;
}



/****************************************************************************
* 名    称: void GPIO_group_OUT(_gpio_group *group,u16 outdata)
* 功    能：使用随意16个IO口组成一个16位并行输出口
* 入口参数：*group： 任意16个IO口为元素的结构体指针
            outdata: 16位输出数值
* 返回参数：无
* 说    明：outdata从高位开始赋值
****************************************************************************/
void GPIO_group_OUT(_gpio_group *group,u16 outdata)
{
  u8 t;
	for(t=0;t<16;t++)
	{               
		if((outdata&0x8000)>>15)  
		{
			switch(t)
			{
				case 0:	   group->data15=1; break;
				case 1:	   group->data14=1; break;
				case 2:	   group->data13=1; break;
				case 3:	   group->data12=1; break;
				case 4:	   group->data11=1; break;
				case 5:	   group->data10=1; break;
				case 6:	   group->data9=1;  break;
				case 7:	   group->data8=1;  break;
				case 8:	   group->data7=1;  break;
				case 9:	   group->data6=1;  break;
				case 10:	 group->data5=1;  break;
				case 11:	 group->data4=1;  break;
				case 12:	 group->data3=1;  break;
				case 13:	 group->data2=1;  break;
				case 14:	 group->data1=1;  break;
				case 15:	 group->data0=1;  break;
			}
		}
		else
		{
			switch(t)
			{
				case 0:	   group->data15=0; break;
				case 1:	   group->data14=0; break;
				case 2:	   group->data13=0; break;
				case 3:	   group->data12=0; break;
				case 4:	   group->data11=0; break;
				case 5:	   group->data10=0; break;
				case 6:	   group->data9=0;  break;
				case 7:	   group->data8=0;  break;
				case 8:	   group->data7=0;  break;
				case 9:	   group->data6=0;  break;
				case 10:	 group->data5=0;  break;
				case 11:	 group->data4=0;  break;
				case 12:	 group->data3=0;  break;
				case 13:	 group->data2=0;  break;
				case 14:	 group->data1=0;  break;
				case 15:	 group->data0=0;  break;
			}
		}
	 outdata<<=1; 	
	}
}

