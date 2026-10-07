#ifndef _SOFT_IICs_H_
#define	_SOFT_IICs_H_

#include "sys.h"
#include "main.h"


/***** IIC引脚定义 *****/
//#define	IICs_SDA					PIout(3)
//#define IICs_SCL					PHout(6)
//#define IICs_SDAin				PIin(3)


#define I2Cs_SCL_1()  		HAL_GPIO_WritePin(IICs_SCL_GPIO_Port, IICs_SCL_Pin,GPIO_PIN_SET)			/* SCL = 1 */
#define I2Cs_SCL_0()  		HAL_GPIO_WritePin(IICs_SCL_GPIO_Port, IICs_SCL_Pin,GPIO_PIN_RESET)		/* SCL = 0 */

#define I2Cs_SDA_1()  		HAL_GPIO_WritePin(IICs_SDA_GPIO_Port, IICs_SDA_Pin,GPIO_PIN_SET)			/* SDA = 1 */
#define I2Cs_SDA_0()  		HAL_GPIO_WritePin(IICs_SDA_GPIO_Port, IICs_SDA_Pin,GPIO_PIN_RESET)		/* SDA = 0 */

#define I2Cs_SDA_READ()		HAL_GPIO_ReadPin(IICs_SDA_GPIO_Port, IICs_SDA_Pin)										/* 读SDA口线状态 */


/***** IIC应答定义 *****/
typedef	enum{
	ACK,			//应答
	NACK,			//不应答
}IICAckTypedef;

void					IICs_Init(void);
uint8_t	IICs_Read( uint8_t SlaveAddr, uint8_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode );
uint8_t	IICs_Write( uint8_t SlaveAddr, uint8_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode );
uint8_t	IICs_Read2( uint8_t SlaveAddr, uint16_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode );
uint8_t	IICs_Write2( uint8_t SlaveAddr, uint16_t cReg, uint8_t *pbuf, uint8_t cnt, IICAckTypedef ackmode );

void		IICs_Scan( void );

#endif
