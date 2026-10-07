#include "relay.h"


LINE_TYPE Line_Type = Line78_;

void RELAY_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RELAY1_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY1_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY1_PORT,RELAY1_PIN);
	
  RCC_AHB1PeriphClockCmd(RELAY2_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY2_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY2_PORT,RELAY2_PIN);
  
  RCC_AHB1PeriphClockCmd(RELAY3_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY3_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY3_PORT,RELAY3_PIN);
  
  RCC_AHB1PeriphClockCmd(RELAY4_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY4_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY4_PORT,RELAY4_PIN);
  
  RCC_AHB1PeriphClockCmd(RELAY5_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY5_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY5_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY5_PORT,RELAY5_PIN);
  
  RCC_AHB1PeriphClockCmd(RELAY6_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY6_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY6_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY6_PORT,RELAY6_PIN);
  
  RCC_AHB1PeriphClockCmd(RELAY7_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY7_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY7_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY7_PORT,RELAY7_PIN);
  
  RCC_AHB1PeriphClockCmd(RELAY8_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY8_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY8_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY8_PORT,RELAY8_PIN);
  
   RCC_AHB1PeriphClockCmd(RELAY9_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY9_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY9_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY9_PORT,RELAY9_PIN);
  
   RCC_AHB1PeriphClockCmd(RELAY10_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = RELAY10_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RELAY10_PORT, &GPIO_InitStructure);	
  GPIO_SetBits(RELAY10_PORT,RELAY10_PIN);
  
  RCC_AHB1PeriphClockCmd(TYPE_IN_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = TYPE_IN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(TYPE_IN_PORT, &GPIO_InitStructure);	

  RCC_AHB1PeriphClockCmd(CROSS_IN_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = CROSS_IN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(CROSS_IN_PORT, &GPIO_InitStructure);	
  
}
/*返回值为1则为UTP，返回值为0则为SFTP*/
bool Detect_Type()
{
	if(1 == GPIO_ReadInputDataBit(TYPE_IN_PORT,TYPE_IN_PIN))
		return 1;
	else
		return 0;
}
/*返回值为1则为交叉，返回值为0则为直连*/
bool Detect_Cross()
{
	if(1 == GPIO_ReadInputDataBit(CROSS_IN_PORT,CROSS_IN_PIN))
		return 1;
	else
		return 0;
}
static void Choose_IN_Line(u8 Line)
{
	switch(Line)
	{
		case 1:
			GPIO_SetBits(RELAY1_PORT,RELAY1_PIN);
			GPIO_ResetBits(RELAY2_PORT,RELAY2_PIN);
			GPIO_ResetBits(RELAY3_PORT,RELAY3_PIN);
		  GPIO_SetBits(RELAY4_PORT,RELAY4_PIN);
		break;
		case 3:
			GPIO_SetBits(RELAY4_PORT,RELAY4_PIN);
			GPIO_SetBits(RELAY2_PORT,RELAY2_PIN);
			GPIO_ResetBits(RELAY3_PORT,RELAY3_PIN);	
			GPIO_SetBits(RELAY1_PORT,RELAY1_PIN);		
		break;
		case 5:
			GPIO_SetBits(RELAY1_PORT,RELAY1_PIN);
	     	GPIO_SetBits(RELAY2_PORT,RELAY2_PIN);
			GPIO_ResetBits(RELAY4_PORT,RELAY4_PIN);
			GPIO_SetBits(RELAY3_PORT,RELAY3_PIN);
		break;
		case 7:
			GPIO_SetBits(RELAY1_PORT,RELAY1_PIN);
	     	GPIO_SetBits(RELAY2_PORT,RELAY2_PIN);
			GPIO_SetBits(RELAY4_PORT,RELAY4_PIN);
			GPIO_SetBits(RELAY3_PORT,RELAY3_PIN);
		break;
	}
	
}
static void Choose_OUT_Line(u8 Line)
{	
	switch(Line)
	{
		case 2:
			GPIO_SetBits(RELAY7_PORT,RELAY7_PIN);
	    GPIO_SetBits(RELAY8_PORT,RELAY8_PIN);
			GPIO_ResetBits(RELAY5_PORT,RELAY5_PIN);
			GPIO_ResetBits(RELAY6_PORT,RELAY6_PIN);
	  	Line_Type = Line12_;
		break;
		case 4:
			GPIO_SetBits(RELAY7_PORT,RELAY7_PIN);
	    GPIO_SetBits(RELAY8_PORT,RELAY8_PIN);
			GPIO_SetBits(RELAY5_PORT,RELAY5_PIN);
			GPIO_ResetBits(RELAY6_PORT,RELAY6_PIN);	
			Line_Type = Line45_;		
		break;
		case 6:
		    GPIO_SetBits(RELAY5_PORT,RELAY5_PIN);
	     	GPIO_SetBits(RELAY8_PORT,RELAY8_PIN);
			GPIO_ResetBits(RELAY7_PORT,RELAY7_PIN);
			GPIO_SetBits(RELAY6_PORT,RELAY6_PIN);
			Line_Type = Line36_;
		break;
		case 8:
			GPIO_SetBits(RELAY5_PORT,RELAY5_PIN);
	    GPIO_SetBits(RELAY8_PORT,RELAY8_PIN);
			GPIO_SetBits(RELAY7_PORT,RELAY7_PIN);
			GPIO_SetBits(RELAY6_PORT,RELAY6_PIN);
			Line_Type = Line78_;
		break;
	}
}
void Choose_Line_Couple(u8 Line_IN,u8 Line_OUT)
{
	Choose_IN_Line(Line_IN);
	Choose_OUT_Line(Line_OUT);
}

void Line1_To_MCU()
{
	GPIO_ResetBits(RELAY1_PORT,RELAY1_PIN);
	GPIO_SetBits(RELAY2_PORT,RELAY2_PIN);
	GPIO_SetBits(RELAY3_PORT,RELAY3_PIN);
	GPIO_SetBits(RELAY4_PORT,RELAY4_PIN);
	GPIO_SetBits(RELAY5_PORT,RELAY5_PIN);
	GPIO_ResetBits(RELAY8_PORT,RELAY8_PIN);
	GPIO_SetBits(RELAY7_PORT,RELAY7_PIN);
	GPIO_SetBits(RELAY6_PORT,RELAY6_PIN);
}
void Relay_Close()
{
	GPIO_ResetBits(RELAY1_PORT,RELAY1_PIN);
	GPIO_ResetBits(RELAY2_PORT,RELAY2_PIN);
	GPIO_ResetBits(RELAY3_PORT,RELAY3_PIN);
	GPIO_ResetBits(RELAY4_PORT,RELAY4_PIN);
	GPIO_ResetBits(RELAY5_PORT,RELAY5_PIN);
	GPIO_ResetBits(RELAY8_PORT,RELAY8_PIN);
	GPIO_ResetBits(RELAY7_PORT,RELAY7_PIN);
	GPIO_ResetBits(RELAY6_PORT,RELAY6_PIN);
}
