#ifndef __DDS_H
#define __DDS_H

#include "sys.h"
#include "tim.h"

#define	DDSTimer	TIM6
#define hDDSTimer	htim6

void DDS_Set( uint8_t wave, uint32_t fre, int16_t amp );
void DDS_Stop( void );
void DDS_Out( void );

#endif  
	 
	 



