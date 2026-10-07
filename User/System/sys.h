#ifndef _SYS_H
#define _SYS_H

#include "stm32f4xx.h"
#include "SoftTimer.h"
#include "msg.h"
#include "UART_FIFO.h"
#include "UserLib.h"
#include "malloc.h"

/*************************定义常用数据类型****************************/
typedef int32_t	s32;
typedef int16_t	s16;
typedef int8_t	s8;

typedef const int32_t	sc32;  
typedef const int16_t	sc16;  
typedef const int8_t	sc8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;  

typedef union{
	uint8_t 	uData8[4];
	uint32_t 	uData32;	
}union_u32TypeDef;

typedef union{
	uint8_t 	uData8[4];
	float 		fData32;
}union_f32TypeDef;


/*************************定义复位方式****************************/
typedef enum{
	IWDGRST = 1,			//独立看门狗复位标志
	WWDGRST,					//窗口看门狗复位标志
	SFTRST,						//软件复位标志
	PORRST,						//上电复位标志
	PINRST,						//按键复位标志
	LPWRRST,					//低电量复位标志
	ISRUN,						//正在运行
}ResetType;



/*************************定义DEBUG函数****************************/
#define 	DEBUG_EN     			   1

#if (DEBUG_EN== 1)
	#define DEBUGOUT(...)   printf(__VA_ARGS__)
#else
	#define DEBUGOUT(...)	{}
#endif



/*************************定义ID号起始地址****************************/
#ifdef STM32F0
	#define MCU_IdAddr					0x1FFFF7AC   	/*STM32F0唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FFFF7CC
#endif

#ifdef STM32F1
	#define MCU_IdAddr					0x1FFFF7E8	 	/*STM32F1唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FFFF7E0
#endif

#ifdef STM32F2
	#define MCU_IdAddr					0x1FFF7A10   	/*STM32F2唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FFF7A22
#endif

#ifdef STM32F3
	#define MCU_IdAddr					0x1FFFF7AC		/*STM32F3唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FFFF7CC
#endif

#ifdef STM32F4
	#define MCU_IdAddr					0x1FFF7A10	  	/*STM32F4唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FFF7A22
#endif

#ifdef STM32F7
	#define MCU_IdAddr					0x1FF0F420	  	/*STM32F7唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FF0F442
#endif

#ifdef STM32L0
	#define MCU_IdAddr					0x1FF80050	  	/*STM32L0唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FF8007C
#endif

#ifdef STM32L1
	#define MCU_IdAddr					0x1FF80050	 	/*STM32L1唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FF8004C
#endif

#ifdef STM32L4
	#define MCU_IdAddr					0x1FFF7590	 	/*STM32L4唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FFF75E0
#endif

#ifdef STM32H7
	#define MCU_IdAddr					0x1FF0F420	 	/*STM32H7唯一ID起始地址*/
	#define MCU_FlashSizeAddr		0x1FF0F442
#endif




typedef struct
{
	u16 data0:1;
	u16 data1:1;
	u16 data2:1;
	u16 data3:1;
	u16 data4:1;
	u16 data5:1;
	u16 data6:1;
	u16 data7:1;
	u16 data8:1;
	u16 data9:1;
	u16 data10:1;
	u16 data11:1;
	u16 data12:1;	
	u16 data13:1;
	u16 data14:1;
	u16 data15:1;	
}_gpio_group;



//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014 
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20) //0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20) //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) //0x40022410 
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) //0x40022810 

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n)  //输出 
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n)  //输入

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)  //输出 
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)  //输入

void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq);//时钟系统配置
//以下为汇编函数
void			WFI_SET(void);			//执行WFI指令
void			INTX_DISABLE(void);//关闭所有中断
void			INTX_ENABLE(void);	//开启所有中断
void			MSR_MSP(u32 addr);	//设置堆栈地址 


void 			GetMCUID( u8* pID );
ResetType GetResetFlag(void);
void 			Sys_Soft_Reset(void);


void 			Delay_ms(uint32_t n);
void 			Delay_us(uint32_t n);

void 			GPIO_group_OUT(_gpio_group *group,u16 outdata);

extern		void Error_Handler(void);
#endif

