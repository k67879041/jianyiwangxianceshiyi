

#include "msg.h"



static stMsgQueue g_xMsg;

/*******************************************************************************
//函 数 名: vInitMsgQueue
//功能说明: 初始化消息缓冲区
//形    参: 无
//返 回 值: 无
*******************************************************************************/
void vInitMsgQueue( void )
{
	g_xMsg.ucWrite = g_xMsg.ucRead = 0; 
}

/*******************************************************************************
* 函 数 名: vPutMsg
* 功能说明: 将1个消息压入消息FIFO缓冲区。
* 形    参:  _MsgCode : 消息代码
*	    			 _pMsgParam : 消息参数，一般指向某个特定的结构体. 或者是0
* 返 回 值: 无
*******************************************************************************/
void vPutMsg( MSG_CODE_X _ucMsgCode, uint32_t _ulMsgParam )
{
  g_xMsg.xBuf[ g_xMsg.ucWrite ].ucMsgCode = _ucMsgCode;
  g_xMsg.xBuf[ g_xMsg.ucWrite ].ulParam = _ulMsgParam;
  
	g_xMsg.ucWrite++;
  if( g_xMsg.ucWrite >= MSG_FIFO_SIZE )
    g_xMsg.ucWrite = 0;
}

/*******************************************************************************
* 函 数 名: ucGetMsg
* 功能说明: 将1个消息取出消息FIFO缓冲区。
* 形    参:  _pxMsg : 消息结构体
* 返 回 值: 0:失败 1:成功
*******************************************************************************/
uint8_t ucGetMsg( stMsgUnit* _pxMsg )
{
  if( g_xMsg.ucWrite == g_xMsg.ucRead )    /* 防止消息队列已空操作 */
  { return 0; }

  _pxMsg->ucMsgCode = g_xMsg.xBuf[ g_xMsg.ucRead ].ucMsgCode;
  _pxMsg->ulParam = g_xMsg.xBuf[ g_xMsg.ucRead ].ulParam;

	g_xMsg.ucRead++;
  if( g_xMsg.ucRead >= MSG_FIFO_SIZE )
    g_xMsg.ucRead = 0;

  return 1;
}

/*******************************************************************************
* 函 数 名: prvClearMsg
* 功能说明: 清空消息缓冲区 
* 形    参:  None	  
* 返 回 值: None
*******************************************************************************/
void vClearMsg( void )
{ g_xMsg.ucWrite = g_xMsg.ucRead; }








/***************************************************(END OF FILE)****************************************/
