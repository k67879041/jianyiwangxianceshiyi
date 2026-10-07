#ifndef __MALLOC_H
#define __MALLOC_H
#include "sys.h" 

 
#ifndef NULL
#define NULL 0
#endif

//In_SRAM内存参数设定,完全处于内部SRAM里面.
#define In_SRAM_BLOCK_SIZE						64  	  						//内存块大小为64字节
#define In_SRAM_MAX_SIZE							100*1024  					//最大管理内存 100K
#define In_SRAM_TABLE_SIZE						In_SRAM_MAX_SIZE/In_SRAM_BLOCK_SIZE 	//内存表大小

//Ext_SRAM 内存参数设定,内存池处于外部SDRAM里面
#define Ext_SRAM_BLOCK_SIZE						64  	  						//内存块大小为64字节
#define Ext_SRAM_MAX_SIZE							960 *1024  					//最大管理内存960k字节
#define Ext_SRAM_TABLE_SIZE						Ext_SRAM_MAX_SIZE/Ext_SRAM_BLOCK_SIZE 	//内存表大小
		 
//In_CCMRAM 内存参数设定,内存池处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!)
#define In_CCMRAM_BLOCK_SIZE					64  	  						//内存块大小为64字节
#define In_CCMRAM_MAX_SIZE						60 *1024  					//最大管理内存60K
#define In_CCMRAM_TABLE_SIZE					In_CCMRAM_MAX_SIZE/In_CCMRAM_BLOCK_SIZE 	//内存表大小

typedef struct{
	uint8_t 	memrdy;			// 内存管理是否就绪
	uint16_t 	preused;		// 使用率
	uint8_t		blocksize;	// 块大小
	uint32_t	memsize;		// 内存池大小
	uint32_t	tabsize;		// 内存管理表大小
	uint8_t		*membase;		// 内存池基地址
	uint32_t 	*memmap;		// 内存管理表地址
}MallocTypeDef;


extern MallocTypeDef In_SRAM, Ext_SRAM, In_CCMRAM;

void MEM_Copy(void *des,void *src,uint32_t n);
void MEM_Set(void *s,uint8_t c,uint32_t count);
void MEM_Init( MallocTypeDef *mem );

uint16_t	MEM_Preused( MallocTypeDef *mem );
void 			*MEM_Malloc( MallocTypeDef *mem, uint32_t size );
uint8_t		MEM_Free( MallocTypeDef *mem, void *ptr );


#endif













