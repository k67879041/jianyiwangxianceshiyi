#include "interrupt.h"
#include "stm32f4xx.h" 
void EXTI_KEY_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //使能SYSCFG时钟
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);  

	EXTI_InitTypeDef EXTI_InitStructure;//设置EXTI结构体结构体变量
	EXTI_InitStructure.EXTI_Line=EXTI_Line4; //中断线标号，配置某个中断线上的中断函数，取值范围：EXTI_Line0~EXTI_Line15
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt; //中断模式，选值：中断EXTI_Mode_Interrupt和事件EXTI_Mode_Event
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//触发方式，可以是下降沿触发EXTI_Trigger_Falling，上升沿触发                EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd=ENABLE; //中断使能
	EXTI_Init(&EXTI_InitStructure);  //初始化外设EXTI寄存器

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure; //设置NVIC结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; //使能按键外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //中断优先级分组初始化
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);  

	EXTI_InitStructure.EXTI_Line=EXTI_Line3; //中断线标号，配置某个中断线上的中断函数，取值范围：EXTI_Line0~EXTI_Line15
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt; //中断模式，选值：中断EXTI_Mode_Interrupt和事件EXTI_Mode_Event
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//触发方式，可以是下降沿触发EXTI_Trigger_Falling，上升沿触发                EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd=ENABLE; //中断使能
	EXTI_Init(&EXTI_InitStructure);  //初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; //使能按键外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //中断优先级分组初始化

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);  

	EXTI_InitStructure.EXTI_Line=EXTI_Line0; //中断线标号，配置某个中断线上的中断函数，取值范围：EXTI_Line0~EXTI_Line15
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt; //中断模式，选值：中断EXTI_Mode_Interrupt和事件EXTI_Mode_Event
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;//触发方式，可以是下降沿触发EXTI_Trigger_Falling，上升沿触发                EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd=ENABLE; //中断使能
	EXTI_Init(&EXTI_InitStructure);  //初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //使能按键外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //中断优先级分组初始化

}

void TIM3_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); ///使能 TIM3 时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //声明初始化参数结构体变量
	TIM_TimeBaseStructure.TIM_Period = 9999;        //以下开始填充
	TIM_TimeBaseStructure.TIM_Prescaler =7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //执行初始化
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure; //设置NVIC结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_Init(&NVIC_InitStructure); //中断优先级分组初始化
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );       //使能更新中断
	
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3 外设
}
