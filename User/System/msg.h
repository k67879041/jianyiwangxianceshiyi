/*
*********************************************************************************************************
*
*	模块名称 : 消息处理缓存队列
*	文件名称 : msg.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/
#ifndef __MSG_H
#define __MSG_H

#include "sys.h"


//用于程序间通信的事件消息缓冲区
#define MSG_FIFO_SIZE      50

typedef enum
{
	MSG_NONE              = 0,
	MSG_KEY_PRESS,
	MSG_KEY_SHORTRELEASE,
	MSG_KEY_LONGHOLD,
	MSG_KEY_LONGRELEASE,
	MSG_KEY_DOBLECLICK,
	MSG_TOUCH_PRESS,
	MSG_TOUCH_LONGHOLD,
	MSG_TOUCH_RELEASE,
	
	MSG_ADC_GET,
	MSG_ADC_GET_HALF,
	MSG_ADC_AD7606_GET,

}MSG_CODE_X;


typedef struct
{
  MSG_CODE_X   ucMsgCode;	//消息类型
  uint32_t     ulParam;   //消息数据
}stMsgUnit;

//一个消息队列中压入的数据包括一个消息类型和一个数据 
typedef struct
{
  stMsgUnit xBuf[ MSG_FIFO_SIZE ];
  uint8_t ucRead;
  uint8_t ucWrite;
}stMsgQueue;

/******************************************************************************/
void    vClearMsg( void );
uint8_t ucGetMsg( stMsgUnit* _pxMsg );
void    vPutMsg( MSG_CODE_X _ucMsgCode, uint32_t _ulMsgParam );
void    vInitMsgQueue( void );
/******************************************************************************/










#endif
/***********************************************(END OF FILE)*******************************************/
