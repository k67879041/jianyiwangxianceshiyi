#include "xpt2046.h"

TouchDevTypeDef Touch_dev;


/**************以下为普通IO模拟SPI通信XPT2046***************/
/****************************************************************************
* 名    称: void XPT2046_Write_Byte(uint8_t num) 
* 功    能：SPI写数据,向触摸屏IC写入1byte数据 
* 入口参数：num:要写入的数据
* 返回参数：无
* 说    明：       
****************************************************************************/
static void XPT2046_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)	T_MOSI_H();  
		else T_MOSI_L();
		num<<=1;    
		T_CLK_L();
		Delay_us(1);
		T_CLK_H();		 //上升沿有效	        
	}		 			    
}

/****************************************************************************
* 名    称: uint16_t XPT2046_Read_AD(uint8_t CMD)
* 功    能：SPI读数据 ,从触摸屏IC读取adc值
* 入口参数：CMD:指令
* 返回参数: 读到的数据	   
* 说    明：       
****************************************************************************/  
static uint16_t XPT2046_Read_AD(uint8_t CMD)	  
{
	uint8_t count=0; 	  
	uint16_t Num=0;
	
	T_CLK_L();		//先拉低时钟 	 
	T_MOSI_L(); 	//拉低数据线
	T_CS_L(); 		//选中触摸屏IC 2046
	XPT2046_Write_Byte(CMD);//发送命令字
	Delay_us(6);
	T_CLK_L();
	Delay_us(1);
	T_CLK_H();
	Delay_us(1);
	T_CLK_L();
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{
		Num<<=1;
		T_CLK_L();	 //下降沿有效
		Delay_us(1);
 		T_CLK_H();
 		if( T_MISO() )	Num++;
	}
	Num>>=4;   	//高12位才有效
	T_CS_H();		  //释放片选
	return(Num);   
}
/**************以上为普通IO模拟SPI通信XPT2046***************/


/****************************************************************************
* 名    称: uint16_t XPT2046_Read_XorY(uint8_t xy)
* 功    能：读取一个坐标值(x或者y)
* 入口参数：xoy:指令(CMD_RDX/CMD_RDY)
* 返回参数: 读到的数据	   
* 说    明：连续读取5次数据,对这些数据升序排列, 然后去掉最低和最1个数,取平均值       
****************************************************************************/ 
static uint16_t XPT2046_Read_XorY(uint8_t xoy)
{
	uint16_t i,temp;
	uint16_t buf[5];
	
	for(i=0;i<5;i++)	buf[i] = XPT2046_Read_AD(xoy);
	
	temp = MedianFilter( buf, 5 );
	
	return temp;
} 

/****************************************************************************
* 名    称: uint8_t XPT2046_Read_XY( CoordinateTypeDef *coordinate )
* 功    能：读取触屏坐标
* 入口参数：*coordinate:读取到的坐标值
* 返回参数: 返回值:0,失败;1,成功   
* 说    明：最小值不能少于50.       
****************************************************************************/ 
static uint8_t XPT2046_Read_XY( CoordinateTypeDef *coordinate )
{
	uint16_t xtemp,ytemp;
	
	xtemp = XPT2046_Read_XorY( Touch_dev.CMD_RDX );
	ytemp = XPT2046_Read_XorY( Touch_dev.CMD_RDY );
	
	if( xtemp<50 || ytemp<50 )	return 0;//读数失败
	
	coordinate->x = xtemp;
	coordinate->y = ytemp;
	
	return 1;//读数成功
}

/****************************************************************************
* 名    称: uint8_t XPT2046_Read_XY2( CoordinateTypeDef *coordinate )
* 功    能：连续2次读取触摸屏坐标
* 入口参数：*coordinate:读取到的坐标值
* 返回参数: 返回值:0,失败;1,成功   
* 说    明：        
****************************************************************************/
static uint8_t XPT2046_Read_XY2( CoordinateTypeDef *coordinate )
{
	CoordinateTypeDef c1,c2;
 	uint8_t 	flag;
	
	// 测量两次
	flag = XPT2046_Read_XY( &c1 );
	if(flag==0)	return(0);
	flag = XPT2046_Read_XY( &c2 );
	if(flag==0)	return(0);
	
	//前后两次采样在+-50内
	if( Abs_Difference(c1.x,c2.x)<=50 && Abs_Difference(c1.y,c2.y)<=50 )
	{
		coordinate->x = (c1.x+c2.x)/2;
		coordinate->y = (c1.y+c2.y)/2;
		return 1;
	}
	else
	{
		coordinate->x = 0;
		coordinate->y = 0;
		return 0;
	}
}  

/****************************************************************************
* 名    称: void XPT2046_Scan( uint8_t type, CoordinateTypeDef *coordinate )
* 功    能：触摸按键扫描	
* 入口参数：type:0,屏幕坐标; 1,物理坐标 
*						*coordinate: 有触摸时返回坐标
* 返回参数: 1: 有触摸,返回坐标有效; 0: 无触摸
* 说    明：
****************************************************************************/
uint8_t XPT2046_Scan( uint8_t type, CoordinateTypeDef *coordinate )
{
	CoordinateTypeDef c1;
	
	if( T_PEN()==0 )// 有触摸
	{
		// 读取到物理坐标?
		if( XPT2046_Read_XY2(&c1) )
		{
			if( type )	// 返回物理坐标
			{
				coordinate->x = c1.x;
				coordinate->y = c1.y;
			}
			else				// 将结果转换为屏幕坐标
			{
				coordinate->x = Touch_dev.dX_X*c1.x + Touch_dev.dX_Y*c1.y + Touch_dev.dX;
				coordinate->y = Touch_dev.dY_X*c1.x + Touch_dev.dY_Y*c1.y + Touch_dev.dY;
			}
			return 1;
		}
		else return 0;
	}
	else  // 无触摸
	{
		return 0;
	}
}


/**
  * @brief  计算 XPT2046 触摸坐标校正系数（注意：只有在LCD和触摸屏间的误差角度非常小时,才能运用下面公式）
  * @param  pDisplayCoordinate ：屏幕人为显示的已知坐标
  * @param  pstrScreenSample ：对已知坐标点触摸时 XPT2046 产生的坐标
  * @param  pCalibrationFactor ：根据人为设定坐标和采样回来的坐标计算出来的屏幕触摸校正系数
  * @retval 计算状态
	*   该返回值为以下值之一：
  *     @arg 1 :计算成功
  *     @arg 0 :计算失败
  */
uint8_t XPT2046_Calculate_CalibrationFactor ( CoordinateTypeDef * pDisplayCoordinate, CoordinateTypeDef * pScreenSample, XPT2046_Calibration * pCalibrationFactor )
{
	uint8_t ucRet = 1;

	
	/* K＝ ( X0－X2 )  ( Y1－Y2 )－ ( X1－X2 )  ( Y0－Y2 ) */
	pCalibrationFactor -> Divider =  ( ( pScreenSample [ 0 ] .x - pScreenSample [ 2 ] .x ) *  ( pScreenSample [ 1 ] .y - pScreenSample [ 2 ] .y ) ) - 
									                 ( ( pScreenSample [ 1 ] .x - pScreenSample [ 2 ] .x ) *  ( pScreenSample [ 0 ] .y - pScreenSample [ 2 ] .y ) ) ;
	
	
	if (  pCalibrationFactor -> Divider == 0  )		ucRet = 0;

	else
	{
		/* A＝ (  ( XD0－XD2 )  ( Y1－Y2 )－ ( XD1－XD2 )  ( Y0－Y2 ) )／K	*/
		pCalibrationFactor -> An =  ( ( pDisplayCoordinate [ 0 ] .x - pDisplayCoordinate [ 2 ] .x ) *  ( pScreenSample [ 1 ] .y - pScreenSample [ 2 ] .y ) ) - 
								                ( ( pDisplayCoordinate [ 1 ] .x - pDisplayCoordinate [ 2 ] .x ) *  ( pScreenSample [ 0 ] .y - pScreenSample [ 2 ] .y ) );
		
		/* B＝ (  ( X0－X2 )  ( XD1－XD2 )－ ( XD0－XD2 )  ( X1－X2 ) )／K	*/
		pCalibrationFactor -> Bn =  ( ( pScreenSample [ 0 ] .x - pScreenSample [ 2 ] .x ) *  ( pDisplayCoordinate [ 1 ] .x - pDisplayCoordinate [ 2 ] .x ) ) - 
								                ( ( pDisplayCoordinate [ 0 ] .x - pDisplayCoordinate [ 2 ] .x ) *  ( pScreenSample [ 1 ] .x - pScreenSample [ 2 ] .x ) );
		
		/* C＝ ( Y0 ( X2XD1－X1XD2 )+Y1 ( X0XD2－X2XD0 )+Y2 ( X1XD0－X0XD1 ) )／K */
		pCalibrationFactor -> Cn =  ( pScreenSample [ 2 ] .x * pDisplayCoordinate [ 1 ] .x - pScreenSample [ 1 ] .x * pDisplayCoordinate [ 2 ] .x ) * pScreenSample [ 0 ] .y +
								                ( pScreenSample [ 0 ] .x * pDisplayCoordinate [ 2 ] .x - pScreenSample [ 2 ] .x * pDisplayCoordinate [ 0 ] .x ) * pScreenSample [ 1 ] .y +
								                ( pScreenSample [ 1 ] .x * pDisplayCoordinate [ 0 ] .x - pScreenSample [ 0 ] .x * pDisplayCoordinate [ 1 ] .x ) * pScreenSample [ 2 ] .y ;
		
		/* D＝ (  ( YD0－YD2 )  ( Y1－Y2 )－ ( YD1－YD2 )  ( Y0－Y2 ) )／K	*/
		pCalibrationFactor -> Dn =  ( ( pDisplayCoordinate [ 0 ] .y - pDisplayCoordinate [ 2 ] .y ) *  ( pScreenSample [ 1 ] .y - pScreenSample [ 2 ] .y ) ) - 
								                ( ( pDisplayCoordinate [ 1 ] .y - pDisplayCoordinate [ 2 ] .y ) *  ( pScreenSample [ 0 ] .y - pScreenSample [ 2 ] .y ) ) ;
		
		/* E＝ (  ( X0－X2 )  ( YD1－YD2 )－ ( YD0－YD2 )  ( X1－X2 ) )／K	*/
		pCalibrationFactor -> En =  ( ( pScreenSample [ 0 ] .x - pScreenSample [ 2 ] .x ) *  ( pDisplayCoordinate [ 1 ] .y - pDisplayCoordinate [ 2 ] .y ) ) - 
								                ( ( pDisplayCoordinate [ 0 ] .y - pDisplayCoordinate [ 2 ] .y ) *  ( pScreenSample [ 1 ] .x - pScreenSample [ 2 ] .x ) ) ;
		
		
		/* F＝ ( Y0 ( X2YD1－X1YD2 )+Y1 ( X0YD2－X2YD0 )+Y2 ( X1YD0－X0YD1 ) )／K */
		pCalibrationFactor -> Fn =  ( pScreenSample [ 2 ] .x * pDisplayCoordinate [ 1 ] .y - pScreenSample [ 1 ] .x * pDisplayCoordinate [ 2 ] .y ) * pScreenSample [ 0 ] .y +
								                ( pScreenSample [ 0 ] .x * pDisplayCoordinate [ 2 ] .y - pScreenSample [ 2 ] .x * pDisplayCoordinate [ 0 ] .y ) * pScreenSample [ 1 ] .y +
								                ( pScreenSample [ 1 ] .x * pDisplayCoordinate [ 0 ] .y - pScreenSample [ 0 ] .x * pDisplayCoordinate [ 1 ] .y ) * pScreenSample [ 2 ] .y;
			
	}
	
	return ucRet;	
}

/****************************************************************************
* 名    称: void XPT2046_Init(uint8_t lcddir)
* 功    能：xpt2046初始化	
* 入口参数：lcddir:0,竖屏; 1,横屏
* 返回参数: 无
* 说    明：
****************************************************************************/    
void XPT2046_Init( uint8_t lcddir )
{
	if(lcddir==1)  // 1 横屏
	{
		Touch_dev.CMD_RDX = 0x90;
		Touch_dev.CMD_RDY = 0xD0;
		Touch_dev.dX_X = -0.090859;
		Touch_dev.dX_Y = 0.000100;
		Touch_dev.dX = 338.834747;
		Touch_dev.dY_X = -0.001083;
		Touch_dev.dY_Y = 0.065755;
		Touch_dev.dY = -12.005668;
	}
	else				    // 0 竖屏
	{
		Touch_dev.CMD_RDX = 0xD0;
		Touch_dev.CMD_RDY = 0x90;
		Touch_dev.dX_X = 0.065826;
		Touch_dev.dX_Y = 0.002512;
		Touch_dev.dX = -18.551832;
		Touch_dev.dY_X = -0.001340;
		Touch_dev.dY_Y = 0.093901;
		Touch_dev.dY = -22.593582;
	}
}



void Task_Touch( void )
{
	// 调节扫描频率, 无触摸式扫描频率 10Hz, 有触摸时 100Hz
	static uint32_t subcnt=0;
	CoordinateTypeDef Coord;
	
	
	if( !SoftTimer_CheckTimer(TMR_TOUCH_ID) )	return;
		
	subcnt++;
	if( subcnt%10!=0 && subcnt>10 )	return;
		
	// 有触摸
	if( XPT2046_Scan(0,&Coord) )
	{
		if(subcnt>10)	// 刚按下
		{
			Touch_dev.Coord_S.x = Coord.x;
			Touch_dev.Coord_S.y = Coord.y;
			Touch_dev.Coord_C.x = Coord.x;
			Touch_dev.Coord_C.y = Coord.y;
			Touch_dev.PressTime = 0;
			
			vPutMsg( MSG_TOUCH_PRESS, (u32)(Touch_dev.Coord_S.x<<16)+Touch_dev.Coord_S.y );
			//按下事件
			#if TOUCH_ACTION==1
				Touch_Action( MSG_TOUCH_PRESS, Touch_dev.Coord_S.x, Touch_dev.Coord_S.y );
			#else			
				vPutMsg( MSG_TOUCH_PRESS, (u32)(Touch_dev.Coord_S.x<<16)+Touch_dev.Coord_S.y );
			#endif
		}
		else	// 长按
		{
			Touch_dev.Coord_C.x = Coord.x;
			Touch_dev.Coord_C.y = Coord.y;
			Touch_dev.PressTime++;
			
			//长按事件
			if( Touch_dev.PressTime>TOUCH_LONG_TIME && (Touch_dev.PressTime-TOUCH_LONG_TIME)%TOUCH_LONG_STEP_TIME==0 )
			{				
				vPutMsg( MSG_TOUCH_LONGHOLD, (u32)(Touch_dev.Coord_C.x<<16)+Touch_dev.Coord_C.y );
				#if TOUCH_ACTION==1
					Touch_Action( MSG_TOUCH_LONGHOLD, Touch_dev.Coord_C.x, Touch_dev.Coord_C.y );
				#else
					vPutMsg( MSG_TOUCH_LONGHOLD, (u32)(Touch_dev.Coord_C.x<<16)+Touch_dev.Coord_C.y );
				#endif
			}
		}
		
		subcnt = 0;	// 保持100Hz扫描频率
	}
	
	else
	{
		if( subcnt==2 && Touch_dev.PressTime>0 )
		{
			//释放事件
			#if TOUCH_ACTION==1
				Touch_Action( MSG_TOUCH_RELEASE, Touch_dev.Coord_S.x, Touch_dev.Coord_S.y );
			#else
				vPutMsg( MSG_TOUCH_RELEASE, (u32)(Touch_dev.Coord_S.x<<16)+Touch_dev.Coord_S.y );
			#endif
		}
	}
}

