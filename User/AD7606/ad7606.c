#include <stdio.h>
#include <string.h>
#include "ad7606.h"
#include "main.h"
#include "spi.h"
#include "Function.h"

uint16_t	adc7606Index = 0; //采样深度索引
int16_t	adc7606Buff[ SAMPLING_DEPTH ][ SAMPLING_CHANNEL ]={0};//初始化各个通道的采集数据


static void AD7606_Reset(void)
{
	AD7606_Rst_0;
	AD7606_Rst_1;
	for(int i=40;i>0;i--)	//高电平脉宽不小于50ns
	{
		__NOP();
	}
	AD7606_Rst_0;
}

void AD7606_Init(void)
{
	AD7606_Reset();
	AD7606_Cs_1;
	AD7606_ConvA_1;
}

void AD7606_Start(void)
{
	AD7606_ConvA_0;
	for(int i=20;i>0;i--)	//低电平脉宽不小于25ns
	{
		__NOP();
	}
	AD7606_ConvA_1;
}
void AD7606_GetADCArray(float *array)
{
	for(int i=0;i<8;i++)
	{
		array[i]=adc7606Buff[0][i]*RESOLUTION;
	}
}
void AD7606_Stop(void)
{
     HAL_NVIC_DisableIRQ(EXTI4_IRQn);//打印出来直接关闭中断	
}


void printfADC(void)
{
	DEBUGOUT("ADC采集完成\r\n");
    for (int i = 0; i < SAMPLING_CHANNEL; i++) {
		DEBUGOUT("channel%d:  ",i);
        for (int j = 0; j < SAMPLING_DEPTH; j++) {
            DEBUGOUT("%.4fv ",RESOLUTION*adc7606Buff[j][i]); // 用数据实际值替代之前的乘法
            if (j == SAMPLING_CHANNEL - 1) {
                DEBUGOUT("\r\n");
            }
        }
    }
}



//外部中断回调函数,不要在中断用串口打印
void AD7606_GetADC( void )
{
	AD7606_Cs_0;
	HAL_SPI_Receive(&hspi2, (uint8_t *)adc7606Buff[ adc7606Index ], SAMPLING_CHANNEL, 10);//接受到的数据存储在adcBuffPtr数组中
	adc7606Index+=1;
	//如果接受满了，则直接返回，否则继续接受
	if(adc7606Index>=SAMPLING_DEPTH){
		adc7606Index=0;
		vPutMsg( MSG_ADC_AD7606_GET, 0 );
		return ;
	}

	AD7606_Cs_1;
    AD7606_Start();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//单次采样结束则BUSY位置0，下降沿触发中断
{
    if(GPIO_Pin==AD7606_BUSY_Pin)    AD7606_GetADC();
}

