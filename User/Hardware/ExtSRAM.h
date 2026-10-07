#ifndef __EXTSRAM_H
#define __EXTSRAM_H

#include "sys.h"
#include "main.h"


// µÿ÷∑
#define EXT_SRAM_ADDR  	    ((uint32_t)0x68000000)
// »›¡ø	1M Byte
#define EXT_SRAM_SIZE				(1 * 1024 * 1024)

uint32_t ExtSRAM_Test(void);

#endif
