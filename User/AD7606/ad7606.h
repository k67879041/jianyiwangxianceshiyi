#ifndef __AD_7606_H_
#define __AD_7606_H_

/*************************
 * Ad7606接法(spi(2)+Gpio(4))~2025.7.25
SPI2_SCLK(PB10)-----------RD	//配置为SPI2的SCLK引脚
SPI2_MISO(PB14)-----------D7	//配置为SPI2的MISO引脚
+
AD7606Cs(PC0)-------CS			//配置为手动SPI时片选引脚
AD7606BUSY(PC4)------BUSY		//配置为外部中断，下降沿
AD7606Rst(PC1)------RST			//配置为输出模式
TIM1_CH1(PC5)-------CONVA（CA，CB）//配置为输出模式，用于触发ADC采样
************************/
#ifdef __cplusplus
 extern "C" {
#endif


#include <stdint.h>


#define SAMPLING_DEPTH			10
#define SAMPLING_CHANNEL	    8


#define AD7606_Cs_1					HAL_GPIO_WritePin(AD7606_CS_GPIO_Port, AD7606_CS_Pin, GPIO_PIN_SET)     //
#define	AD7606_Cs_0 				HAL_GPIO_WritePin(AD7606_CS_GPIO_Port, AD7606_CS_Pin, GPIO_PIN_RESET)

#define AD7606_ConvA_1			HAL_GPIO_WritePin(AD7606_CONVA_GPIO_Port, AD7606_CONVA_Pin, GPIO_PIN_SET)
#define	AD7606_ConvA_0	 		HAL_GPIO_WritePin(AD7606_CONVA_GPIO_Port, AD7606_CONVA_Pin, GPIO_PIN_RESET)

#define	AD7606_Rst_1				HAL_GPIO_WritePin(AD7606_RST_GPIO_Port, AD7606_RST_Pin, GPIO_PIN_SET)
#define AD7606_Rst_0				HAL_GPIO_WritePin(AD7606_RST_GPIO_Port, AD7606_RST_Pin, GPIO_PIN_RESET)
/* 
 * AD7606原始值转电压值：基于双极性模式（±VREF）
 * 核心公式：V = (原始值 × 分辨率系数)，其中分辨率系数 = VREF / 32768（2^15）
 * 若有外部增益/分压，需额外乘以系数K（见注释说明）
 */

//  原始值转电压,配置参数（根据硬件实际情况修改）
#define VREF        2.5f     // 参考电压（单位：V，内部默认2.5V）
#define RESOLUTION  (VREF / 32768.0f)*2 // 分辨率系数/2（V/LSB）：2.5V/32768 ≈ 0.0000762939V/LSB(=0.0763为mv)
#define GAIN_K      1.0f     // 外部电路增益系数（如无放大/分压则为1，若有则根据电路计算）
// 转换公式：电压 = 原始值 × 分辨率系数 × 外部增益系数(分辨率系数=(（2*5000mv）/65536))

//#define relution (float)0.0763


extern uint16_t ADCptFlag;


void 	AD7606_Init(void);
void 	AD7606_Start(void);
void	AD7606_GetADC( void );
void AD7606_GetADCArray(float *array);
void	printfADC( void );
void 	AD7606_Stop(void);
uint16_t MedianFilter( uint16_t *_pStr, uint16_t _len );
#ifdef __cplusplus
}
#endif

#endif
