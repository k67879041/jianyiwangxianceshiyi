#ifndef _AD5933_H_
#define _AD5933_H_

#include "sys.h"
#include "AD5933.h"
#include "stm32f4xx.h"
#define	AD5933_MCLK_USE_OUT	0	//0：内部时钟(16.776MHz)  1：外部时钟


//注意：
//AD5933测量较小电阻时，自身驱动能力不足，需要使用外部运放电路增强驱动能力，从而测量小于5.1K的小电阻
//测量大于或小于5.1K的电阻时，需要切换下面宏定义，且更改模块上跳线帽位置
		#define _DEF_RSBIGGER5K		0		//接法1，测量大于5.1K( 5.1K-10M)电阻时,用跳线帽将模块上排针P6的1、2连接，P6的3,4断开，P5的1,2断开；
//	#define _DEF_RSBIGGER5K		0		//接法2，测量小于5.1K(100R-5.1K)电阻时,用跳线帽将模块上排针P6的1、2断开，P6的3,4连接，P5的1,2连接；

#if _DEF_RSBIGGER5K == 1
#define REG_MIN	5100
#define REG_MAX	6000000
#define AD5933_Correction 101615461
#elif DEF_RSBIGGER5K == 0
#define REG_MIN	100
#define REG_MAX	8000
//979.4
//3.3K
//3293.0
//324.10
#define AD5933_Correction (940795 / 131.1 * 149.095) 
#define AD5933_Correction_100R (940795 / 131.1 * 149.095) 
#define AD5933_Correction_1000R ((940795 / 131.1 * 149.095) / 979.4 * 998.08) 
#define AD5933_Correction_10kR ((940795 / 131.1 * 149.095) / 9495.0 * 9880.0)
//#define AD5933_Correction (94960657)
#define AD5933_CorrectionPhase (2.923-3.2775)
#define AD5933_CorrectionPhase_100kR (2.993-3.2775)
#define AD5933_CorrectionPhase_1000R (3.123-3.5375)
#define AD5933_Real_Correction  (1)
#define AD5933_Image_Correction (1)
#define A (0.02669)
#define B (0.005306)

#endif

#define uint unsigned int


//IO方向设置
#define SDA_IN()  {GPIOB->MODER&=~(3<<(6*2));GPIOB->MODER|=0<<6*2;}
#define SDA_OUT() {GPIOB->MODER&=~(3<<(6*2));GPIOB->MODER|=1<<6*2;}

//IO操作函数	 
#define SCL    PBout(5) //SCL
#define SDA    PBout(6) //SDA
#define READ_SDA   PBin(6)  //输入SDA

#define LEN 8
#define AD5933_SYS_Init					(1)<<12
#define AD5933_Begin_Fre_Scan		(2)<<12
#define AD5933_Fre_UP						(3)<<12
#define AD5933_Fre_Rep					(4)<<12
#define AD5933_Get_Temp					(9)<<12
#define AD5933_Sleep						(10)<<12
#define AD5933_Standby					(11)<<12

#define AD5933_OUTPUT_2V				(0)<<9
#define AD5933_OUTPUT_1V				(3)<<9
#define AD5933_OUTPUT_400mV			(2)<<9
#define AD5933_OUTPUT_200mV			(1)<<9

#define AD5933_Gain_1						(1)<<8
#define AD5933_Gain_5						(0)<<8

#define AD5933_IN_MCLK					(0)<<3
#define AD5933_OUT_MCLK					(1)<<3

#define AD5933_Reset						(1)<<4

//定义函数
void Ini_I2c(void);
void SDA_1(void);
void SDA_0 (void);
void SCL_1(void);
void SCL_0 (void);
void GetACK(void);
void SendNACK(void);
void START(void);
void STOP(void);
void SendByte(u8 txd);	// 发送一个字节数据子函数
u8 ReadByte(void);  //读一个字节数据
void Write_Byte(char nAddr,uint nValue);
void SetPointer(char nAddr);
int Rece_Byte(char nAddr);
void Delay_ms(unsigned long nValue);
float Scale_imp (u8 *SValue,u8 *IValue,u8 *NValue,u8 *CValue);
u16 AD5933_Tempter(void);
float Get_resistance(u16 num);
float AD5933_Sweep (float Fre_Begin,float Fre_UP,u16 UP_Num,u16 OUTPUT_Vatage,u16 Gain,u16 SWeep_Rep);
float DA5933_Get_Rs(void);
float DA5933_Get_Cap(float Fre);
float DA5933_Get_L(float Fre);

float DA5933_Get_RsRealImage(float *R_Impedance,float *R_Rad, float *realData, float *imageData);

#endif

