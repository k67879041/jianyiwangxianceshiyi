#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "string.h"
#include "stdlib.h"

#include "FFTInc.h"

#include "main.h"
//#include "iwdg.h"
#include "spi.h"
#include "dac.h"
#include "adc.h"

#include "sys.h"
#include "LED.h"
#include "KEY.h"
#include "AT24cxx.h"
#include "w25qxx.h"
#include "ExtSRAM.h"
#include "TouchLCD.h"
#include "TouchKeypad.h"

#include "AD9959.h"
#include "ad7606.h"

//系统运行指示
#define Led_SysRunning()		LEDFunSet( LED1, 20, 10, 100, 0 )
#define Led_SysRunning2()		LEDFunSet( LED2, 20, 10, 100, 0 )
//通信指示
#define Led_Communicate()		LEDFunSet( LED1, 2, 1, 3, 1 )


void JudgCPUResetMode(void);
void PrintSystemMess( void );

void UserSystemInit( void );

void IWDG_Feed(void);

void Task_Msg( void );
void Task_SoftTimer( void );
void Task_UART1( void );

// 引脚结构体定义（与写入函数保持一致）
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GpioPin;
uint16_t readPinsByArray( GpioPin* pins);
void setPinsByArray(GpioPin* pins, uint16_t value) ;

void manager_interface(void);
void function_Task1(uint8_t mode);
void function_Task2(uint8_t mode);
void function_Task(void);
void function_adc_task(void);
void safely_switch(void);
// 用于返回长度和类型的结构体
// 定义线缆类型
float getRandomCableLength(void) ;

void fresh_AD(void);
void Time_count(void);
#endif

