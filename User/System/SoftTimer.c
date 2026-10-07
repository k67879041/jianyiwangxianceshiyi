/*
*********************************************************************************************************
*
*	模块名称 : 软定时器模块
*	文件名称 : SoftTimer.c
*	版    本 : V1.0
*	说    明 : 配置TIM7为软定时器时基,缺省定时周期为1ms。
*
*				实现了多个软件定时器供主程序使用(精度1ms),可以通过修改枚举类型 SoftTimerID 增减定时器个数
*				实现了系统运行时间函数（1ms单位）
*				可选择 TIM2~TIM5 中的一个,利用捕获比较通道实现4个us级定时器。
*				其中 TIM2,TIM5 为32位定时器,定时时间可达 2^32 us
*						 TIM3,TIM4 为16位定时器,定时时间可达 2^16 us
*
*********************************************************************************************************
*/

#include "SoftTimer.h"

///* TIM7句柄,采用TIM7定时器产生软定时器时基 */
//TIM_HandleTypeDef htim7;

/* 定于软件定时器结构体变量 */
static SOFT_TMR 	s_tTmr[TMR_COUNT];

/* 全局运行时间，单位 ms */
__IO uint32_t g_RunTime = 0;


///* TIM7 init function 定时1ms */
//static void TIM7_Init(void)
//{
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//	
//  htim7.Instance = TIM7;
//  htim7.Init.Prescaler = 80-1;
//  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim7.Init.Period = 1000;
//  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
//  {
//    Error_Handler();
//  }
//	
//	/* TIM7 clock enable */
//	__HAL_RCC_TIM7_CLK_ENABLE();
//	/* TIM7 interrupt Init */
//	HAL_NVIC_SetPriority(TIM7_IRQn, 15, 0);
//	HAL_NVIC_EnableIRQ(TIM7_IRQn);	
//	
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }

//	HAL_TIM_Base_Start_IT(&htim7);
//}


/*
*********************************************************************************************************
*	函 数 名: SoftTimer_Init
*	功能说明: 配置 TIM7 中断，并初始化软件定时器变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void SoftTimer_Init(void)
{
	uint8_t 	i;

	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
		s_tTmr[i].CallBack = NULL;			/* 缺省无回调函数 */
	}
	
//	TIM7_Init();

#if defined (USE_TIM2) || defined (USE_TIM3)  || defined (USE_TIM4)	|| defined (USE_TIM5)
	SoftTimer_InitCCTimer();
#endif
}



/*
*********************************************************************************************************
*	函 数 名: SysTick_SoftTimerDec
*	功能说明: 每隔 1 ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参:  _tmr : 定时器变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void SoftTimer_Dec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}

			/* 如果有回调函数,则执行 */
			if( _tmr->CallBack ){
				_tmr->CallBack();
			}
		}
	}
}




/********************************************************************************************************
*	函 数 名: SoftTimer_ISR
*	功能说明: 软定时器中断服务程序, 每 1ms 进入1次
*	参    数:  无
*	返 回 值:  无
********************************************************************************************************/
void SoftTimer_ISR(void)
{
	uint8_t i;

	/* 每隔 1ms，对软件定时器的计数器进行减一操作 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		SoftTimer_Dec(&s_tTmr[i]);
	}

	/* 全局运行时间 + */
	g_RunTime ++;
}



/*
*********************************************************************************************************
*	函 数 名: SoftTimer_StartTimer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*							_period : 定时周期，单位1ms
*							_pCallBack : 定时时间到后，被执行的回调函数，若不须回调此参数设为 NULL 
*	返 回 值: 无
*********************************************************************************************************
*/
void SoftTimer_StartTimer(SoftTimerID _id, uint32_t _period, void *_pCallBack )
{
    if (_id >= TMR_COUNT)
    {
        return ;
    }

    INTX_DISABLE();  			/* 关中断 */

    s_tTmr[_id].Count = (_period + uwTickFreq - 1) / uwTickFreq;		/* 实时计数器初值 */
    s_tTmr[_id].PreLoad = (_period + uwTickFreq - 1) / uwTickFreq;	/* 计数器自动重装值，仅自动模式起作用 */
    s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 1次性工作模式 */
		s_tTmr[_id].CallBack = (void (*)(void))_pCallBack;

    INTX_ENABLE();  				/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: SoftTimer_StartAutoTimer
*	功能说明: 启动一个自动定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*							_period : 定时周期，单位10ms
*	返 回 值: 无
*********************************************************************************************************
*/
void SoftTimer_StartAutoTimer(SoftTimerID _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
        return ;
    }

    INTX_DISABLE();  		/* 关中断 */

    s_tTmr[_id].Count = (_period + uwTickFreq - 1) / uwTickFreq;			/* 实时计数器初值 */
    s_tTmr[_id].PreLoad = (_period + uwTickFreq - 1) / uwTickFreq;		/* 计数器自动重装值，仅自动模式起作用 */
    s_tTmr[_id].Flag = 0;							/* 定时时间到标志 */
    s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* 自动工作模式 */
		s_tTmr[_id].CallBack = NULL;			/* 自动工作模式不设置回调函数 */

    INTX_ENABLE();  			/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: SoftTimer_StopTimer
*	功能说明: 停止一个定时器
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 无
*********************************************************************************************************
*/
void SoftTimer_StopTimer(SoftTimerID _id)
{
    if (_id >= TMR_COUNT)
    {
        return ;
    }

    INTX_DISABLE();  	/* 关中断 */

    s_tTmr[_id].Count = 0;						/* 实时计数器初值 */
    s_tTmr[_id].Flag = 0;							/* 定时时间到标志 */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 一次工作模式 */
		s_tTmr[_id].CallBack = NULL;			/* 无回调函数 */

    INTX_ENABLE();  		/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: SoftTimer_CheckTimer
*	功能说明: 检测定时器是否超时
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
*********************************************************************************************************
*/
uint8_t SoftTimer_CheckTimer( SoftTimerID _id )
{
    if (_id >= TMR_COUNT)
    {
        return 0;
    }

    if (s_tTmr[_id].Flag == 1)
    {
        s_tTmr[_id].Flag = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*	函 数 名: SoftTimer_GetRunTime
*	功能说明: 获取CPU运行时间，单位10ms。最长可以表示 248.5天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*	形    参:  无
*	返 回 值: CPU运行时间，单位10ms
*********************************************************************************************************
*/
uint32_t SoftTimer_GetRunTime(void)
{
    uint32_t runtime;

    INTX_DISABLE();  	/* 关中断 */

    runtime = g_RunTime;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

    INTX_ENABLE();  		/* 开中断 */

    return runtime;
}




/*
*********************************************************************************************************
*	函 数 名: SoftTimer_CheckRunTime
*	功能说明: 计算当前运行时间和给定时刻之间的差值。处理了计数器循环。
*	形    参:  _LastTime 上个时刻
*	返 回 值: 当前时间和过去时间的差值，单位1ms
*********************************************************************************************************
*/
uint32_t SoftTimer_CheckRunTime(uint32_t _LastTime)
{
		uint32_t now_time;
		uint32_t time_diff;

		/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */
		INTX_DISABLE();  	/* 关中断 */
		now_time = g_RunTime;	
		INTX_ENABLE();  		/* 开中断 */
		
		if (now_time >= _LastTime)
		{
			time_diff = now_time - _LastTime;
		}
		else
		{
			time_diff = 0xFFFFFFFF - _LastTime + now_time + 1;
		}

		return time_diff;
}




//void TIM7_IRQHandler(void)
//{
//	/* TIM Update event */
//  if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) != RESET)
//  {
//    if (__HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) != RESET)
//    {
//      __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
//			SoftTimer_ISR();
//    }
//  }	
		
	

//		uint16_t itstatus = 0x0, itenable = 0x0;
//    
//		itstatus = TIM7->SR & TIM_IT_UPDATE;
//		itenable = TIM7->DIER & TIM_IT_UPDATE;
//		if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
//		{
//				/* 清除中断标志 */
//				TIM7->SR = (uint16_t)~TIM_IT_UPDATE;
//				SoftTimer_ISR();
//		}
//}




#if defined (USE_TIM2) || defined (USE_TIM3)  || defined (USE_TIM4)	|| defined (USE_TIM5)

#ifdef USE_TIM2
	#define TIM_HARD										TIM2
	#define	RCC_TIM_HARD_CLK_ENABLE()		__HAL_RCC_TIM2_CLK_ENABLE()
	#define TIM_HARD_IRQn								TIM2_IRQn
	#define TIM_HARD_IRQHandler					TIM2_IRQHandler
#endif

#ifdef USE_TIM3
	#define TIM_HARD										TIM3
	#define	RCC_TIM_HARD_CLK_ENABLE()		__HAL_RCC_TIM3_CLK_ENABLE()	
	#define TIM_HARD_IRQn								TIM3_IRQn
	#define TIM_HARD_IRQHandler					TIM3_IRQHandler
#endif

#ifdef USE_TIM4
	#define TIM_HARD										TIM4
	#define	RCC_TIM_HARD_CLK_ENABLE()		__HAL_RCC_TIM4_CLK_ENABLE()
	#define TIM_HARD_IRQn								TIM4_IRQn
	#define TIM_HARD_IRQHandler					TIM4_IRQHandler
#endif

#ifdef USE_TIM5
	#define TIM_HARD										TIM5
	#define	RCC_TIM_HARD_CLK_ENABLE()		__HAL_RCC_TIM5_CLK_ENABLE()
	#define TIM_HARD_IRQn								TIM5_IRQn
	#define TIM_HARD_IRQHandler					TIM5_IRQHandler
#endif

/* 保存 TIM定时中断到后执行的回调函数指针 */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);


/*
*********************************************************************************************************
*	函 数 名: SoftTimer_InitCCTimer
*	功能说明: 配置 TIMx，用于us级别硬件定时。TIMx将自由运行，永不停止.
*			TIMx可以用TIM2 - TIM5 之间的TIM, 这些TIM有4个通道, 挂在 APB1 上，输入时钟=SystemCoreClock / 2
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SoftTimer_InitCCTimer(void)
{
	TIM_HandleTypeDef  TimHandle = {0};
	uint32_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	TIM_TypeDef* TIMx = TIM_HARD;
	
	RCC_TIM_HARD_CLK_ENABLE();		/* 使能TIM时钟 */
	
    /*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	uiTIMxCLK = SystemCoreClock / 2;

	usPrescaler = uiTIMxCLK / 1000000 - 1;	/* 分频到周期 1us */
	
	if (TIMx == TIM2 || TIMx == TIM5)
	{
		usPeriod = 0xFFFFFFFF;
	}
	else
	{
		usPeriod = 0xFFFF;
	}

	/* 
       设置分频为usPrescaler后，那么定时器计数器计1次就是1us
       而参数usPeriod的值是决定了最大计数：
       usPeriod = 0xFFFF 表示最大0xFFFF微秒。
       usPeriod = 0xFFFFFFFF 表示最大0xFFFFFFFF微秒。
    */
	TimHandle.Instance 							 = TIMx;
	TimHandle.Init.Prescaler         = usPrescaler;
	TimHandle.Init.Period            = usPeriod;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		Error_Handler( );
	}

	/* 配置定时器中断，给CC捕获比较中断使用 */
	HAL_NVIC_SetPriority(TIM_HARD_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(TIM_HARD_IRQn);	

    
    /* 启动定时器 */
	HAL_TIM_Base_Start(&TimHandle);
}

/*
*********************************************************************************************************
*	函 数 名: SoftTimer_StartCCTimer
*	功能说明: 使用TIM2-5做单次定时器使用, 定时时间到后执行回调函数。可以同时启动4个定时器通道，互不干扰。
*             定时精度正负1us （主要耗费在调用本函数的执行时间）
*			  TIM2和TIM5 是32位定时器。定时范围很大
*			  TIM3和TIM4 是16位定时器。
*	形    参: _CC : 捕获比较通道几，1，2，3, 4
*             _uiTimeOut : 超时时间, 单位 1us. 对于16位定时器，最大 65.5ms; 对于32位定时器，最大 4294秒
*             _pCallBack : 定时时间到后，被执行的函数
*	返 回 值: 无
*********************************************************************************************************
*/
void SoftTimer_StartCCTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
		uint32_t cnt_now;
		uint32_t cnt_tar;
		TIM_TypeDef* TIMx = TIM_HARD;

		/* 无需补偿延迟，实测精度正负1us */    
		cnt_now = TIMx->CNT; 
		cnt_tar = cnt_now + _uiTimeOut;			/* 计算捕获的计数器值 */
		if (_CC == 1)
		{
				s_TIM_CallBack1 = (void (*)(void))_pCallBack;

				TIMx->CCR1 = cnt_tar; 			    		/* 设置捕获比较计数器CC1 */
				TIMx->SR = (uint16_t)~TIM_IT_CC1;   /* 清除CC1中断标志 */
				TIMx->DIER |= TIM_IT_CC1;						/* 使能CC1中断 */
		}
		else if (_CC == 2)
		{
				s_TIM_CallBack2 = (void (*)(void))_pCallBack;

				TIMx->CCR2 = cnt_tar;								/* 设置捕获比较计数器CC2 */
				TIMx->SR = (uint16_t)~TIM_IT_CC2;		/* 清除CC2中断标志 */
				TIMx->DIER |= TIM_IT_CC2;						/* 使能CC2中断 */
		}
		else if (_CC == 3)
		{
				s_TIM_CallBack3 = (void (*)(void))_pCallBack;

				TIMx->CCR3 = cnt_tar;								/* 设置捕获比较计数器CC3 */
				TIMx->SR = (uint16_t)~TIM_IT_CC3;		/* 清除CC3中断标志 */
				TIMx->DIER |= TIM_IT_CC3;						/* 使能CC3中断 */
		}
		else if (_CC == 4)
		{
				s_TIM_CallBack4 = (void (*)(void))_pCallBack;

				TIMx->CCR4 = cnt_tar;								/* 设置捕获比较计数器CC4 */
				TIMx->SR = (uint16_t)~TIM_IT_CC4;		/* 清除CC4中断标志 */
				TIMx->DIER |= TIM_IT_CC4;						/* 使能CC4中断 */
		}
		else
		{
				return;
		}
}

/*
*********************************************************************************************************
*	函 数 名: TIMx_IRQHandler
*	功能说明: TIM 中断服务程序
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM_HARD_IRQHandler(void)
{
		uint16_t itstatus = 0x0, itenable = 0x0;
		TIM_TypeDef* TIMx = TIM_HARD;	
    
		itstatus = TIMx->SR & TIM_IT_CC1;
		itenable = TIMx->DIER & TIM_IT_CC1;
		if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
		{
				TIMx->SR = (uint16_t)~TIM_IT_CC1;
				TIMx->DIER &= (uint16_t)~TIM_IT_CC1;		/* 禁能CC1中断 */	

				/* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
				s_TIM_CallBack1();
		}

		itstatus = TIMx->SR & TIM_IT_CC2;
		itenable = TIMx->DIER & TIM_IT_CC2;
		if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
		{
				TIMx->SR = (uint16_t)~TIM_IT_CC2;
				TIMx->DIER &= (uint16_t)~TIM_IT_CC2;		/* 禁能CC2中断 */	

				/* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
				s_TIM_CallBack2();
		}

		itstatus = TIMx->SR & TIM_IT_CC3;
		itenable = TIMx->DIER & TIM_IT_CC3;
		if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
		{
				TIMx->SR = (uint16_t)~TIM_IT_CC3;
				TIMx->DIER &= (uint16_t)~TIM_IT_CC3;		/* 禁能CC2中断 */	

				/* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
				s_TIM_CallBack3();
		}

		itstatus = TIMx->SR & TIM_IT_CC4;
		itenable = TIMx->DIER & TIM_IT_CC4;
		if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
		{
				TIMx->SR = (uint16_t)~TIM_IT_CC4;
				TIMx->DIER &= (uint16_t)~TIM_IT_CC4;		/* 禁能CC4中断 */	

				/* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
				s_TIM_CallBack4();
		}	
}
#endif


/********************************************(END OF FILE)***********************************************/
