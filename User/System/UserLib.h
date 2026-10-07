/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*	文件名称 : UserLib.h
*	版    本 : V1.2
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __USER_LIB_H
#define __USER_LIB_H

#include "sys.h"


int				str_len(char *_str);
void			str_cpy(char *_tar, char *_src);
int				str_cmp(char * s1, char * s2);
void			mem_set(char *_tar, char _data, int _len);

uint16_t	BEBufToUint16(uint8_t *_pBuf);
uint16_t	LEBufToUint16(uint8_t *_pBuf);

uint32_t	BEBufToUint32(uint8_t *_pBuf);
uint32_t	LEBufToUint32(uint8_t *_pBuf);

int32_t		CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

char			BcdToChar(uint8_t _bcd);

uint16_t	Abs_Difference( uint16_t a, uint16_t b);
float			Abs_FloatDifference( float a, float b);
uint16_t	SeekMax( uint16_t *p, uint16_t num );
uint16_t	SeekMin( uint16_t *p, uint16_t num );
void			AvrFilter( uint16_t *_src, uint16_t _len, uint16_t *_tar, uint8_t _depth );
void			AbsFilter( uint16_t *_src, uint16_t _len, uint8_t _percent, uint16_t *_tar );


void 			QuickSort( uint16_t* arr, uint16_t begin, uint16_t end );



uint16_t	MedianFilter( uint16_t *_pStr, uint16_t _len );
uint16_t	MedianFilter2( uint16_t *_pStr, uint16_t _len, uint16_t _n );

#endif

