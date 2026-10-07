//	2025全国大学生电子设计大赛国家一等奖
//
//  文 件 名   : main.c
//  版 本 号   : v1.0
//  作    者   : 山西大学郑傲天 杨浩然 张弘成
//  日　　期   : 2025-09-06

//******************************************************************************/


#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "oled.h"
#include "AD5933.h"
#include <stdio.h>
#include <string.h>
#include "arm_math.h" 
#include "interrupt.h"
#include <stdbool.h>
#include <math.h>
#include "relay.h"
#include "AD9959.h"
#include "AD9959_Outset.h"
#include "adc.h"
#include "24cxx.h"                
#include <stdlib.h>
#include <complex.h>


//串联电阻 并联电阻 用于将测量范围控制在 0~无穷 ，反运算可算出真实值
#define R_S (98.822 + 0.1) 
#define R_P (9849.0)

                                       
#define TEST_MODE 0


float Test_1M_R;
float Test_50M_R;
extern u8 ADC_Val[1024];
volatile float R_Impedance,R_Rad, realData, imageData;	//所夹持电阻的阻值，实部数据，虚部数据
char disStr[64];
float phase;

float phaseR,phaseC;
float Z_Real,Z_Image;
float Z_theta;
bool update_flag = 0; 
bool short_update_flag = 0;
bool short_show_flag = 0;
bool wait_for_30M_calculate_flag = 0;
typedef enum
{
	Dual_Mode = 0,
	Single_Mode,
	Test

}MODE;

MODE Mode = Test;
u8 Key_cnt = 0;

typedef enum
{
	CONNECT_NONE = 0,
	UTP_St,
	SFTP_St,
	UTP_Cr,
	SFTP_Cr
}CONNECT_TYPE;

CONNECT_TYPE Connect_Type = CONNECT_NONE;


typedef enum
{
	SC_NONE = 0,
	Yes,
	No
}SHORT_CIRCUIT;

SHORT_CIRCUIT Short_Circuit = SC_NONE;

typedef enum
{
	LineNone = 0,
	Line12,
	Line36,
	Line45,
	Line78
}SHORT_TYPE;

SHORT_TYPE Short_Type = LineNone;

extern LINE_TYPE Line_Type;

double Cable_Length;
float Cable_Short_Length;
bool Calculate_flag = 0;
float Decay_dB;
//float C1;
//float C2;
//float C3;
//float u = 0.98;
u8 i ;
float mz,mp,mr,mi;
double complex RR;
u16 adc_enter,adc_exit;

u8 m1_save_success_flag =0;
u8 m50_save_success_flag=0;
u8 oled_update_flag = 0;
u8 already_flag = 0;
float I_50M,I_1M,KI;
float C_50M,C_1M;
u8 Val_1_5_cnt = 0;
float Cable40m = 39.99,Cable1m = 1.49;
volatile bool Cable_1_5M_flag = 0;
bool Division_2_flag = 0;
bool Normal_Dual_Flag = 0,Normal_Single_Flag = 0;
double RR_temp;
double RR1_correct=0.0,RR2_correct=0.0,
	RR3_correct=0.0,RR4_correct=0.0,RR_correct=0.0;
void BSP_Init()
{
	
//	I_50M = 15.26;
//	I_1M = 5.4;
	GPIO_ResetBits(RELAY9_PORT,RELAY9_PIN);
	GPIO_ResetBits(RELAY10_PORT,RELAY10_PIN);
	//I_50M = ((AT24CXX_ReadOneByte(2)<<8) +  AT24CXX_ReadOneByte(3))/100.0;
	//I_1M = ((AT24CXX_ReadOneByte(4)<<8) +  AT24CXX_ReadOneByte(5))/100.0;
	I_50M = 0.09;
	I_1M  = 0.14;
	C_1M=-0.000050046;
	C_50M=-0.002492 / 50.0;
	OLED_Refresh();
	update_flag = 0;

}
double R_S_temp;
int main(void)
{ 
	delay_init(168);
	LED_Init();					//初始化LED
 	OLED_Init();				//初始化OLED
	RELAY_Init();				//初始化继电器部分
	TIM3_Init();
	/*用于DDS模块初始化与衰减采集（ADC）*/
	GPIO_AD9959_Configuration();
	AD9959_Init();
	AD9959_enablechannel0(); 				//使能通道0
	AD9959_Setwavephase(30000000,0);			   	   
	AD9959_enablechannel3(); 				//使能通道3
	AD9959_Setwavephase(30000000,0);	
	ADCx_Init();
	
	/*用于控制调试模式与测量模式*/
	if(TEST_MODE)
	  Mode = Test;
	/*用于AT24C02存储校验数据*/
	AT24CXX_Init();
	while(AT24CXX_Check())  //检测AT24C02是否正常
	{
		delay_ms(500);
	}
	
	KEY_Init();             //初始化按键
	EXTI_KEY_Init();        //初始化按键中断
	BSP_Init();             //初始化部分数据以及OLED刷新
	while(1) 
	{
		DA5933_Get_RsRealImage(&R_Impedance,&R_Rad, &realData, &imageData);	//测量所夹持电阻的阻值，及DFT的相位，实部数据，虚部数据
		Z_Real  = R_Impedance * cos(R_Rad);                                 //算出真正阻抗的阻值
		Z_Image = R_Impedance * sin(R_Rad);									//算出真正阻抗的容值（-）或感值（+）
		
		switch(Line_Type)                                                   //修正每一路继电器的阻值
		{
			case Line12_:
				R_S_temp = R_S + RR1_correct + RR_correct;
			break;
			case Line36_:
				R_S_temp = R_S + RR2_correct + RR_correct;
			break;
			case Line45_:
				R_S_temp = R_S + RR3_correct + RR_correct;
			break;
			case Line78_:
				R_S_temp = R_S + RR4_correct + RR_correct;
			break;			
		}
		double complex ZRp   = R_P + 0.0 * I;                        
		double complex ZRs   = R_S_temp + 0.0 * I;
		double complex ZRead = Z_Real + Z_Image * I;
		double complex Z_correct   = 0.4 + 0.0 * I;
		RR = ZRp *(ZRead - ZRs)/(ZRp + ZRs - ZRead);                       //通过串并的阻抗算出真正的阻抗
		RR = RR - Z_correct;                                               //减去修正阻抗值（水晶头）
		switch(Mode)
		{
		case Dual_Mode:                                                    //双端模式的显示
			OLED_ShowChinese(32,0,3,16,1);
			OLED_ShowChinese(48,0,4,16,1);
			OLED_ShowChinese(80,0,1,16,1);
			OLED_ShowChinese(64,0,0,16,1);
			Line1_To_MCU();
			if(Detect_Type())                                              //判断屏蔽非屏蔽以及直连或交叉
			{
				if(Detect_Cross())                                     
					Connect_Type = UTP_Cr;
				else
					Connect_Type = UTP_St;
			}
			else
			{
				if(Detect_Cross()) 
					Connect_Type = SFTP_Cr;
				else
					Connect_Type = SFTP_St;
			}

			 
			switch(Connect_Type)
			{
				case UTP_St:
					OLED_ShowString(0,16,(unsigned char*)"UTP     ",16, 1);  
					OLED_ShowChinese(48,16,25,16,1);
					OLED_ShowChinese(64,16,26,16,1);
				
				break;
				case SFTP_St:
					OLED_ShowString(0,16,(unsigned char*)"SFTP    ",16, 1);  
					OLED_ShowChinese(48,16,25,16,1);
					OLED_ShowChinese(64,16,26,16,1);
			
				break;
				case UTP_Cr:
					OLED_ShowString(0,16,(unsigned char*)"UTP        ",16, 1);  
					OLED_ShowChinese(48,16,27,16,1);
					OLED_ShowChinese(64,16,28,16,1);
				
				break;
				case SFTP_Cr:
					OLED_ShowString(0,16,(unsigned char*)"SFTP    ",16, 1);  
					OLED_ShowChinese(48,16,27,16,1);
					OLED_ShowChinese(64,16,28,16,1);
				
				break;
				default:
					OLED_ShowChinese(40,16,22,16,1);
					OLED_ShowChinese(72,16,23,16,1); 
				break;
			}
			if(Connect_Type == SFTP_Cr || Connect_Type == SFTP_St )              //如果SFTP则用1m线的校准数据处理
				Cable_Short_Length = RR_temp / I_1M / 2.0;
			if(Connect_Type == UTP_Cr || Connect_Type == UTP_St )				 //如果UTP则用50m线的校准数据处理
				Cable_Short_Length = RR_temp / I_50M / 2.0;			

			
			sprintf(disStr,"    :%.3f        ",RR_temp);
			OLED_ShowString(0,32,(unsigned char*)disStr,16, 1);
			sprintf(disStr,"30M    :%.1fdB        ",-Decay_dB);
			OLED_ShowString(0,48,(unsigned char*)disStr,16, 1); 
			OLED_ShowChinese(0,32,7,16,1);
			OLED_ShowChinese(16,32,8,16,1);
			
			OLED_ShowChinese(24,48,9,16,1);
			OLED_ShowChinese(40,48,10,16,1);
			
		//12 36 45 78
		break;
		case Single_Mode:                                     //单端模式的显示
			//GPIO_SetBits(RELAY8_PORT,RELAY8_PIN);	
			OLED_ShowChinese(32,0,2,16,1);
			OLED_ShowChinese(0,16,15,16,1);
			OLED_ShowChinese(16,16,16,16,1);
		switch(Short_Type)                                    //判断线对短路
			{
				case Line12:
					Short_Circuit = Yes;

				break;
				case Line36:
					Short_Circuit = Yes;

				break;
				case Line45:
					Short_Circuit = Yes;

				break;
				case Line78:
					Short_Circuit = Yes;

				break;
				default:
					Short_Circuit = No;
					Cable_Short_Length = 0; 
					OLED_ShowString(80,16,"  ",16, 1);					
				break;
			}
//			Short_Circuit = SC_NONE;
//			Short_Type = LineNone;
			OLED_ShowChinese(48,0,4,16,1);
			OLED_ShowChinese(80,0,1,16,1);
			OLED_ShowChinese(64,0,0,16,1);
			OLED_ShowString(32,16,(unsigned char*)":",16, 1);
		  switch(Short_Circuit)
		  {
			  case Yes:
				  OLED_ShowChinese(48,16,17,16,1);
			   OLED_ShowChinese(64,16,24,16,1);
			  break;
			  case No:
				  OLED_ShowChinese(48,16,18,16,1);
			   OLED_ShowChinese(64,16,24,16,1);
			  break;
			  default:
				  OLED_ShowChinese(48,16,22,16,1);
				  OLED_ShowChinese(64,16,23,16,1);
			  break;  
		  }	
			
		
			if(Cable_Short_Length > Cable_Length)
				Cable_Short_Length = Cable_Length;
			
			
			sprintf(disStr,"        :%.3f m       ",Cable_Length);
			OLED_ShowString(0,32,(unsigned char*)disStr,16, 1); 
			sprintf(disStr,"        :%.3f m       ",Cable_Short_Length);
			OLED_ShowString(0,48,(unsigned char*)disStr,16, 1); 
			OLED_ShowChinese(0,32,11,16,1);
			OLED_ShowChinese(16,32,12,16,1);
			OLED_ShowChinese(32,32,13,16,1);
			OLED_ShowChinese(48,32,14,16,1);
			OLED_ShowChinese(0, 48,15,16,1);
			OLED_ShowChinese(16,48,16,16,1);
			OLED_ShowChinese(32,48,19,16,1);
			OLED_ShowChinese(48,48,20,16,1);
			break;
			default:
			  sprintf(disStr,"Z:%.5f  ",sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)));
			  OLED_ShowString(0,0,(unsigned char*)disStr,16, 1); 
			  sprintf(disStr,"Re:%.5f  ",creal(RR));
			  OLED_ShowString(0,16,(unsigned char*)disStr,16, 1); 
				sprintf(disStr,"Im:%.5f  ",cimag(RR));
			  OLED_ShowString(0,32,(unsigned char*)disStr,16, 1); 
			  sprintf(disStr,"Phase:%.5f  ",R_Rad);
			  OLED_ShowString(0,48,(unsigned char*)disStr,16, 1); 
			break;
		}
		//GPIO_ResetBits(RELAY8_PORT,RELAY8_PIN);	
//		delay_ms(200);
		if(oled_update_flag)
		{
			OLED_Refresh();
			oled_update_flag = 0;
		}
		if(update_flag)
		{
			OLED_Refresh();
			update_flag = 0;
			if(Mode == Dual_Mode)
			{
				
				wait_for_30M_calculate_flag = 1;
			}
		}
	  
	}
}


void EXTI3_IRQHandler(void) 
{ 
	if(EXTI_GetITStatus(EXTI_Line3)!=RESET)//双端测量按键
	{ 
		delay_ms(10);
		if(0 == GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3))
		{
			Mode = Dual_Mode;
			update_flag = 1;
			RR_temp = creal(RR);
			//Val_1_5_cnt++;
			Key_cnt++;
			OLED_Clear();
		}
	    EXTI_ClearITPendingBit(EXTI_Line3); //清除LINE上的中断标志位  
	} 
}
void EXTI4_IRQHandler(void) 
{ 
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET)//单端测量按键
	{ 
		delay_ms(10);
		if(0 == GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
		{
			short_update_flag = 1;
			Mode = Single_Mode;
			Short_Circuit = SC_NONE;
			Short_Type = LineNone;
			delay_ms(250);
			delay_ms(250);
			if(0 == GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
				Normal_Single_Flag = 1;
			else
				Normal_Single_Flag = 0;
			Key_cnt++;
			OLED_Clear();
		}
	    EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE上的中断标志位  
	} 
}
void EXTI0_IRQHandler(void) 
{ 
	//标定按键，按第一下标定1m线的电容，
	//第二下1m线的电阻，第三下50m线的电容，第四下50m线的电阻 
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
	{ 
		delay_ms(10);
		if(1 == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			OLED_Clear();
			static u8 state = 0;
			switch(state)
			{
				case 0:
					C_1M =  1.0 / (2 * 3.1415926 / 10.0 * cimag(RR));
					m1_save_success_flag = 1;
					state = 1;
				break;
				case 1:
					I_1M = creal(RR) / 2.0;
					m1_save_success_flag = 1;
					state = 2;
				break;
				case 2:
					C_50M =  1.0 / (2 * 3.1415926 / 10.0 * cimag(RR))/50.0;
					m50_save_success_flag = 1;
					state = 3;
				break;
				case 3:
					I_50M =  creal(RR)/100.0;
					m50_save_success_flag = 1;	
					state = 4;				
				break;
				case 4:
					already_flag = 1;
				break;
			}
			delay_ms(500);
			OLED_Clear();
		}
	    EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE上的中断标志位  
	} 
}
uint16_t sum = 0,cnt;

/*用于各项定时任务：继电器的切换等*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //判断中断状态
	{
		
		LED1_Toggle();
		if(Test == Mode)
		{
			update_flag = 1;
			Calculate_flag = 1;
			static u8 relay_state = 6;
			static u8 tcnt = 0;
			tcnt++;
			if(tcnt >= 4)
			{
				  tcnt = 0;
					switch(relay_state)
					{
						case 0:
							relay_state = 1;
							Choose_Line_Couple(1,2);
						break;
						case 1:
							relay_state = 2;
							Choose_Line_Couple(3,6);
						break;
						case 2:
							relay_state = 3;
							Choose_Line_Couple(5,4);
						break;
						case 3:
							relay_state = 0;
							Choose_Line_Couple(7,8);
						break;
					}
			}
		
		}

		if(m50_save_success_flag)
		{
			m50_save_success_flag = 0;
			OLED_ShowString(0,16,"M50 Save Success!!",16, 1); 
			OLED_Refresh();
		}
		if(m1_save_success_flag)
		{
			m1_save_success_flag = 0;
			OLED_ShowString(0,16,"M1 Save Success!! ",16, 1); 
			OLED_Refresh();
		}
		if(already_flag)
		{
			already_flag = 0;
			m1_save_success_flag = 0;
			OLED_ShowString(0,16,"Already Saved  !! ",16, 1); 
			Division_2_flag = 1;
			OLED_Refresh();
		}
		static u8 state = 0;
		volatile static float Z_I_for_type_judge1,Z_I_for_type_judge2;
		volatile static float Z_R_for_type_judge1,Z_R_for_type_judge2;
		volatile static float RR_Real,RR_Image;
		volatile static float RR_Real_Keep,RR_Image_Keep;
		if(Mode == Single_Mode)
		{
			if(short_update_flag)
			{
				switch(state)
				{
					case 0:
						Choose_Line_Couple(1,2);
						state = 1;
						update_flag = 1;

					break;
					case 1:
						DA5933_Get_RsRealImage(&R_Impedance,&R_Rad, &realData, &imageData);	
						Z_Real  = R_Impedance * cos(R_Rad);
						Z_Image = R_Impedance * sin(R_Rad);
						double complex ZRp   = R_P + 0.0 * I;
						double complex ZRs   = R_S_temp + 0.0 * I;
						double complex ZRead = Z_Real + Z_Image * I;
					
						RR = ZRp *(ZRead - ZRs)/(ZRp + ZRs - ZRead);
						RR_Real = creal(RR);
					    RR_Image = cimag(RR);
						if((sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) > 0.0) && (sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) < 150.0))
						{
							Short_Type = Line12;
							RR_Real_Keep = RR_Real;
							//RR_Image_Keep = RR_Image;
						}			
						else
						{
							Z_I_for_type_judge1 = cimag(RR);
							Z_R_for_type_judge1 = creal(RR);
							//RR_Real_Keep = RR_Real;
							RR_Image_Keep = RR_Image;
						}
						state = 2;
						Choose_Line_Couple(3,6);
						update_flag = 1;
					break;
					case 2:
						DA5933_Get_RsRealImage(&R_Impedance,&R_Rad, &realData, &imageData);	
						Z_Real  = R_Impedance * cos(R_Rad);
						Z_Image = R_Impedance * sin(R_Rad);
						ZRp   = R_P + 0.0 * I;
						ZRs   = R_S_temp + 0.0 * I;					
						ZRead = Z_Real + Z_Image * I;			
						RR = ZRp *(ZRead - ZRs)/(ZRp + ZRs - ZRead);
						RR_Real = creal(RR);
					  RR_Image = cimag(RR);
						if((sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) > 0.0) && (sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) < 150.0))
						{
							Short_Type = Line36;
							RR_Real_Keep = RR_Real;
							//RR_Image_Keep = RR_Image;
						}		
						else
						{
							Z_I_for_type_judge1 = cimag(RR);
							Z_R_for_type_judge1 = creal(RR);
							//RR_Real_Keep = RR_Real;
							RR_Image_Keep = RR_Image;
							
						}
						state = 3;	
						Choose_Line_Couple(5,4);
						update_flag = 1;						
					break;
					case 3:
						DA5933_Get_RsRealImage(&R_Impedance,&R_Rad, &realData, &imageData);
						Z_Real  = R_Impedance * cos(R_Rad);
						Z_Image = R_Impedance * sin(R_Rad);
						ZRp   = R_P + 0.0 * I;
						ZRs   = R_S_temp + 0.0 * I;				
						ZRead = Z_Real + Z_Image * I;		
						RR = ZRp *(ZRead - ZRs)/(ZRp + ZRs - ZRead);
						RR_Real = creal(RR);
					  RR_Image = cimag(RR);
						if((sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) > 0.0) && (sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) < 150.0))
						{
							Short_Type = Line45;
							RR_Real_Keep = RR_Real;
							//RR_Image_Keep = RR_Image;
						}							
						state = 4;	
						Choose_Line_Couple(7,8);
						update_flag = 1;
					break;
					case 4:
						DA5933_Get_RsRealImage(&R_Impedance,&R_Rad, &realData, &imageData);
						Z_Real  = R_Impedance * cos(R_Rad);
						Z_Image = R_Impedance * sin(R_Rad);
						ZRp   = R_P + 0.0 * I;
						ZRs   = R_S_temp + 0.0 * I;		
						ZRead = Z_Real + Z_Image * I;		
						RR = ZRp *(ZRead - ZRs)/(ZRp + ZRs - ZRead);
						RR_Real = creal(RR);
					  RR_Image = cimag(RR);
						if((sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) > 0.0) && (sqrt(creal(RR)*creal(RR)+cimag(RR)*cimag(RR)) < 150.0))
						{
							Short_Type = Line78;
							RR_Real_Keep = RR_Real;
							
						}							
						state = 5;	
						update_flag = 1;	

					break;

					case 5:
						state = 6;	
					if(Short_Type == LineNone)
					{
							RR_Real_Keep  = RR_Real;
							RR_Image_Keep = RR_Image;
					}

						if(Normal_Single_Flag)
						{
							Connect_Type = SFTP_St;
							OLED_ShowChar(100,0,'S',16,1);
							Cable_Short_Length = RR_Real_Keep / I_1M / 2.0;
							Cable_Length = 1.0 / (2 * 3.1415926 / 10 * RR_Image_Keep) / C_1M;//-0.01854
							
						}
						else
						{ 
							Connect_Type = UTP_St;
							OLED_ShowChar(100,0,'U',16,1);
							Cable_Short_Length = RR_Real_Keep / I_50M / 2.0;
							Cable_Length = 1.0 / (2 * 3.1415926 / 10 * RR_Image_Keep) / C_50M;//-0.0002832
						}
						Choose_Line_Couple(7,8);
						Normal_Single_Flag = 0;
					break;
					case 6:
						state = 0;
						short_update_flag = 0;
						update_flag = 1;
						Z_I_for_type_judge1 = 0;
						Z_R_for_type_judge1 = 0; 
						Z_I_for_type_judge2 = 0;
						Z_R_for_type_judge2 = 0; 
					break;
					
				}
				
			}
			
		}
		else
		{
			if(wait_for_30M_calculate_flag)
			{
				static u8 state_30m = 0;				
				switch(state_30m)
				{
					case 0:

						state_30m = 1;						
					break;
					case 1:
						GPIO_SetBits(RELAY9_PORT,RELAY9_PIN);
						GPIO_SetBits(RELAY10_PORT,RELAY10_PIN);
						state_30m = 2;
					break;
					case 2:
						state_30m = 3;
					case 3:

						state_30m = 4;
					case 4:
						state_30m = 5;
					case 5:
						state_30m = 6;
					case 6:
					
					for(int i = 0;i<10;i++)
					{
						sum += Get_ADC_Value(ADC_Channel_1) ;
					}
					
					adc_enter = sum/10 ;
					sum = 0;			
					for(int i = 0;i<10;i++)
					{
						sum += Get_ADC_Value(ADC_Channel_3) ;
					}
					adc_exit =sum/10 ;
						
					Decay_dB = 20 * log10((double)adc_exit/adc_enter);;
					GPIO_ResetBits(RELAY9_PORT ,RELAY9_PIN);
					GPIO_ResetBits(RELAY10_PORT,RELAY10_PIN);
					state_30m = 7;
					break;
					case 7:

						state_30m = 8;						
					break;
					case 8:
						GPIO_SetBits(RELAY9_PORT,RELAY9_PIN);
						GPIO_SetBits(RELAY10_PORT,RELAY10_PIN);
						state_30m = 9;
					break;
					case 9:
						state_30m = 10;
					case 10:
						state_30m = 11;
					case 11:
						state_30m = 12;
					case 12:
						state_30m = 13;
					case 13:
					
					for(int i = 0;i<10;i++)
					{
						sum += Get_ADC_Value(ADC_Channel_1) ;
					}
					
					adc_enter = sum/10 ;
					sum = 0;			
					for(int i = 0;i<10;i++)
					{
						sum += Get_ADC_Value(ADC_Channel_3) ;
					}
					adc_exit =sum/10 ;
						
					Decay_dB = 20 * log10((double)adc_exit/adc_enter);
					GPIO_ResetBits(RELAY9_PORT ,RELAY9_PIN);
					GPIO_ResetBits(RELAY10_PORT,RELAY10_PIN);
					state_30m = 0;
					wait_for_30M_calculate_flag = 0;
					oled_update_flag = 1;
					break;					

				}
				
				
				if(state == 0)
				{
					update_flag = 1;
					state = 1;
				}
			}

		}
		

	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update );        //清除中断标志
}

