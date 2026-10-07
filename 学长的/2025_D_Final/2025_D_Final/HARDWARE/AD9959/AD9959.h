#ifndef _AD9959_H
#define _AD9959_H

#define uchar unsigned char
#define uint unsigned int 


//AD998 I/O控制总线

//端口C宏定义
#define SCLK_1 (GPIO_SetBits(GPIOD,GPIO_Pin_9)) 
#define SCLK_0 (GPIO_ResetBits(GPIOD,GPIO_Pin_9)) 

#define CS_1 (GPIO_SetBits(GPIOC,GPIO_Pin_6))
#define CS_0 (GPIO_ResetBits(GPIOC,GPIO_Pin_6))

#define IO_update_1 (GPIO_SetBits(GPIOD,GPIO_Pin_11)) 
#define IO_update_0 (GPIO_ResetBits(GPIOD,GPIO_Pin_11)) 

#define SDIO0_1 (GPIO_SetBits(GPIOG,GPIO_Pin_7))
#define SDIO0_0 (GPIO_ResetBits(GPIOG,GPIO_Pin_7))

#define PS0_1 (GPIO_SetBits(GPIOD,GPIO_Pin_15)) 
#define PS0_0 (GPIO_ResetBits(GPIOD,GPIO_Pin_15)) 

#define PS1_1 (GPIO_SetBits(GPIOA,GPIO_Pin_12))
#define PS1_0 (GPIO_ResetBits(GPIOA,GPIO_Pin_12))

#define PS2_1 (GPIO_SetBits(GPIOD,GPIO_Pin_13))	
#define PS2_0 (GPIO_ResetBits(GPIOD,GPIO_Pin_13))	

#define PS3_1 (GPIO_SetBits(GPIOA,GPIO_Pin_8))
#define PS3_0 (GPIO_ResetBits(GPIOA,GPIO_Pin_8))

//端口A宏定义
#define SDIO1_1 (GPIO_SetBits(GPIOB,GPIO_Pin_15))
#define SDIO1_0 (GPIO_ResetBits(GPIOB,GPIO_Pin_15))

#define SDIO2_1  (GPIO_SetBits(GPIOG,GPIO_Pin_6))
#define SDIO2_0 (GPIO_ResetBits(GPIOG,GPIO_Pin_6))

#define SDIO3_1  (GPIO_SetBits(GPIOB,GPIO_Pin_13))		
#define SDIO3_0 (GPIO_ResetBits(GPIOB,GPIO_Pin_13))	


#define Reset_1  (GPIO_SetBits(GPIOC,GPIO_Pin_8))
#define Reset_0 (GPIO_ResetBits(GPIOC,GPIO_Pin_8))


//-----------------------------------------------------------------
// 外部函数声明
//-----------------------------------------------------------------
extern void GPIO_AD9959_Configuration(void)  ;//I/O端口配置
extern void AD9959_Init(void);  //复位
extern void WrFrequencyTuningWorddata(double f,uchar *ChannelFrequencyTuningWorddata);
extern void IO_update(void)  ;
extern void WriteToAD9959ViaSpi(uchar RegisterAddress, uchar NumberofRegisters, uchar *RegisterData,uchar temp) ;
extern void WrPhaseOffsetTuningWorddata(double f,uchar *ChannelPhaseOffsetTuningWorddata);
extern void WrAmplitudeTuningWorddata(double f,uchar *ChannelAmplitudeTuningWorddata);//计算过程注意与计算频率区别
extern void WrAmplitudeTuningWorddata1(double f,uchar *ChannelAmplitudeTuningWorddata,uchar *ASRAmplituteWordata);

#endif
