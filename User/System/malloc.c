#include "malloc.h"	   


/****************************  内存池(32字节对齐)  ****************************/
__align(32) uint8_t In_SRAM_Pool[ In_SRAM_MAX_SIZE ];																					//内部SRAM内存池
__align(32) uint8_t Ext_SRAM_Pool[ Ext_SRAM_MAX_SIZE ] 			__attribute__((at(0x68000000)));	//外部SRAM内存池
__align(32) uint8_t In_CCMRAM_Pool[ In_CCMRAM_MAX_SIZE ] 		__attribute__((at(0x10000000)));	//内部CCM内存池

/****************************  内存管理表 ****************************/
uint32_t In_SRAM_Map[ In_SRAM_TABLE_SIZE ];																														//内部SRAM内存池MAP
uint32_t Ext_SRAM_Map[ Ext_SRAM_TABLE_SIZE ] 			__attribute__((at(0x68000000+Ext_SRAM_MAX_SIZE)));	//外部SRAM内存池MAP
uint32_t In_CCMRAM_Map[ In_CCMRAM_TABLE_SIZE ] 		__attribute__((at(0x10000000+In_CCMRAM_MAX_SIZE)));	//内部CCM内存池MAP


/****************************  内存设备定义 ****************************/
//MallocTypeDef	In_SRAM = {
//	0, 0, In_SRAM_BLOCK_SIZE, In_SRAM_MAX_SIZE, In_SRAM_TABLE_SIZE,
//	In_SRAM_Pool,
//	In_SRAM_Map,
//};

MallocTypeDef	Ext_SRAM = {
	0, 0, Ext_SRAM_BLOCK_SIZE, Ext_SRAM_MAX_SIZE, Ext_SRAM_TABLE_SIZE,
	Ext_SRAM_Pool,
	Ext_SRAM_Map,
};


// CCM内存, 64k, 不属于总线矩阵, 只能通过 CPU 进行访问, DMA 等外设不能访问
MallocTypeDef	In_CCMRAM = {
	0, 0, In_CCMRAM_BLOCK_SIZE, In_CCMRAM_MAX_SIZE, In_CCMRAM_TABLE_SIZE,
	In_CCMRAM_Pool,
	In_CCMRAM_Map,
};

/*
*********************************************************************************************************
*	函 数 名: MEM_Copy
*	功能说明: 内存复制
*	参    数: *des 目的地址; *src 源地址; n 需要复制的内存长度(字节为单位)
*	返 回 值: 无
*********************************************************************************************************
*/
void MEM_Copy(void *des,void *src,uint32_t n)
{
    uint8_t *xdes=des;
		uint8_t *xsrc=src;
    while(n--)	*xdes++=*xsrc++;
}


/*
*********************************************************************************************************
*	函 数 名: MEM_Set
*	功能说明: 内存复制
*	参    数: *s 内存首地址; c 要设置的值; count 需要设置的内存大小(字节为单位)
*	返 回 值: 无
*********************************************************************************************************
*/
void MEM_Set(void *s,uint8_t c,uint32_t count)
{
    uint8_t *xs = s;
    while(count--) *xs++=c;
}


/*
*********************************************************************************************************
*	函 数 名: MEM_Init
*	功能说明: 内存管理表初始化
*	参    数: *mem 内存设备
*	返 回 值: 无
*********************************************************************************************************
*/
void MEM_Init( MallocTypeDef *mem )
{
		//内存状态表数据清零
		MEM_Set( mem->memmap, 0, mem->tabsize*4 );
		mem->memrdy = 1;	//内存管理初始化OK
}

/*
*********************************************************************************************************
*	函 数 名: MEM_Preused
*	功能说明: 获取内存使用率
*	参    数: *mem 内存设备
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t MEM_Preused( MallocTypeDef *mem )
{
    uint32_t used=0;
    uint32_t i;
	
    for(i=0;i<mem->tabsize;i++)
    {
			if( mem->memmap[i] )	used++;
    }
    return used*1000/mem->tabsize;
}


/*
*********************************************************************************************************
*	函 数 名: MEM_Malloc
*	功能说明: 分配内存
*	参    数: *mem 内存设备; size 要分配的内存大小(字节)
*	返 回 值: 内存偏移地址; 0xFFFFFFFF,代表错误;
*********************************************************************************************************
*/
void *MEM_Malloc( MallocTypeDef *mem, uint32_t size )
{
		signed long block_index=0;
		uint32_t blocks;			//需要的内存块数
		uint32_t cblocks=0;		//连续空内存块数
		uint32_t offset = 0xFFFFFFFF;	//找到连续空内存块的偏移地址
		uint32_t i;
	
		if( !mem->memrdy )	MEM_Init( mem );		//未初始化,先执行初始化
		if(size==0)					return NULL;				//不需要分配
	
		//获取需要分配的连续内存块数
		blocks = size/mem->blocksize;
		if( size%mem->blocksize )	blocks++;
	
		//搜索整个内存控制区
		for( block_index=mem->tabsize-1; block_index>=0; block_index-- )
		{
			if( !mem->memmap[block_index] )	cblocks++;//连续空内存块数增加
			else cblocks=0;							//连续内存块清零
			
			if( cblocks==blocks )				//找到了连续blocks个空内存块
			{
				for( i=0;i<blocks;i++ ){
					mem->memmap[block_index+i] = blocks;  
				}
				offset = block_index * mem->blocksize;
				break;
			}
		}
		
		if( offset==0xFFFFFFFF )
			return NULL;
		else
			return (void*)((uint32_t)mem->membase+offset);
}


/*
*********************************************************************************************************
*	函 数 名: MEM_Free
*	功能说明: 释放内存
*	参    数: *mem 内存设备; ptr 内存首地址
*	返 回 值: 1,释放成功; 0,释放失败;
*********************************************************************************************************
*/
uint8_t MEM_Free( MallocTypeDef *mem, void *ptr )
{
	uint16_t i, index, blocks;
	uint32_t offset;
	
	if( !mem->memrdy ){
		MEM_Init( mem );		//未初始化,先执行初始化
		return 0;
	}
	if( ptr==NULL )	return 0;
	
	offset = (u32)ptr-(u32)mem->membase;
	
	if( offset<mem->memsize )//偏移地址在内存池内. 
	{  
			index = offset/mem->blocksize;			//偏移地址所在内存块
			blocks = mem->memmap[index];				//内存块数量
			for(i=0;i<blocks;i++)  							//内存块清零
			{  
					mem->memmap[index+i]=0;  
			}  
			return 1;
	}
	
	else return 0;//偏移超区了
}


