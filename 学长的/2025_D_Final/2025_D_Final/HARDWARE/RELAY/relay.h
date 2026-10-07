#ifndef __RELAY_H
#define __RELAY_H
#include "sys.h"
#include "stdbool.h"
#include "delay.h"
#define RELAY1_RCC  RCC_AHB1Periph_GPIOC
#define RELAY1_PORT GPIOC
#define RELAY1_PIN  GPIO_Pin_4

#define RELAY2_RCC  RCC_AHB1Periph_GPIOA
#define RELAY2_PORT GPIOA
#define RELAY2_PIN  GPIO_Pin_6

#define RELAY3_RCC  RCC_AHB1Periph_GPIOA
#define RELAY3_PORT GPIOA
#define RELAY3_PIN  GPIO_Pin_4

#define RELAY4_RCC  RCC_AHB1Periph_GPIOA
#define RELAY4_PORT GPIOA
#define RELAY4_PIN  GPIO_Pin_2

#define RELAY5_RCC  RCC_AHB1Periph_GPIOC
#define RELAY5_PORT GPIOC
#define RELAY5_PIN  GPIO_Pin_3

#define RELAY6_RCC  RCC_AHB1Periph_GPIOC
#define RELAY6_PORT GPIOC
#define RELAY6_PIN  GPIO_Pin_2

#define RELAY7_RCC  RCC_AHB1Periph_GPIOC
#define RELAY7_PORT GPIOC
#define RELAY7_PIN  GPIO_Pin_0

#define RELAY8_RCC  RCC_AHB1Periph_GPIOC
#define RELAY8_PORT GPIOC
#define RELAY8_PIN  GPIO_Pin_1

#define RELAY9_RCC  RCC_AHB1Periph_GPIOD
#define RELAY9_PORT GPIOD
#define RELAY9_PIN  GPIO_Pin_3

#define RELAY10_RCC  RCC_AHB1Periph_GPIOD
#define RELAY10_PORT GPIOD
#define RELAY10_PIN  GPIO_Pin_5

#define TYPE_IN_RCC     RCC_AHB1Periph_GPIOE
#define TYPE_IN_PORT    GPIOE
#define TYPE_IN_PIN     GPIO_Pin_9

#define CROSS_IN_RCC 		RCC_AHB1Periph_GPIOE
#define CROSS_IN_PORT		GPIOE
#define CROSS_IN_PIN 		GPIO_Pin_7
void RELAY_Init(void);
/*返回值为1则为交叉，返回值为0则为直连*/
bool Detect_Cross(void);
bool Detect_Type(void);
void Choose_Line_Couple(u8 Line_IN,u8 Line_OUT);
void Line1_To_MCU(void);
void Relay_Close(void);
typedef enum
{
	Line12_ = 0,
	Line36_,
	Line45_,
	Line78_
}LINE_TYPE;

#endif
