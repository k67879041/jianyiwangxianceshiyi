#include <string.h>
#include "TouchLCD.h"
#include "ASCII.h"
#include "w25qxx.h"

/****************************************************************************
* 名    称: void LCD_Clear(u16 color)
* 功    能：清屏函数
* 入口参数：color: 要清屏的填充色
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_Clear(u16 color)
{
	u32 index=0;
	u32 totalpoint=lcddev.width;
	
	totalpoint *= lcddev.height; 		//得到总点数
	LCD_SetCursor(0x00,0x0000);			//设置光标位置
	
	LCD_WriteRAM_Prepare();     		//开始写入GRAM
	for(index=0;index<totalpoint;index++){
		LCD->LCD_RAM=color;
	}
}


/****************************************************************************
* 名    称: void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
* 功    能：单色矩形填充
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
*						color: 要填充的填充色
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{          
	u32 i;
	u32 size;
	
	size = (ex-sx) * (ey-sy);
	
	LCD_Set_Window(sx,sy,ex-sx,ey-sy);
	LCD_WriteRAM_Prepare();
	for( i=0;i<size;i++ )	LCD_WR_DATA ( color );
	 LCD_Set_Window(0, 0, lcddev.width, lcddev.height);// 恢复窗口
}

////在指定区域内填充指定颜色块			 
////(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
////color:要填充的颜色
//void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
//{  
//	u16 height,width;
//	u16 i,j; 
//	width=ex-sx+1; 			//得到填充的宽度
//	height=ey-sy+1;			//高度
//	for(i=0;i<height;i++)
//	{
//		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
//		LCD_WriteRAM_Prepare();     //开始写入GRAM
//		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];//写入数据 
//	} 
//}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color )
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol;
	
	delta_x = x2-x1; //计算坐标增量 
	delta_y = y2-y1; 
	uRow = x1;
	uCol = y1;
	
	if( delta_x>0 )	incx = 1; //设置单步方向 
	else if( delta_x==0 )	incx=0;//垂直线 
	else{
		incx = -1;
		delta_x = 0-delta_x;
	}
	
	if( delta_y>0 )	incy = 1; 
	else if( delta_y==0 )	incy = 0;//水平线 
	else{
		incy = -1;
		delta_y = 0-delta_y;
	}
	
	if( delta_x>delta_y )	distance = delta_x; //选取基本增量坐标轴 
	else distance = delta_y;
	
	for( t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint( uRow, uCol, color ); //画点
		xerr += delta_x ; 
		yerr += delta_y ; 
		if( xerr>distance )
		{ 
			xerr -= distance;
			uRow += incx; 
		} 
		if( yerr>distance ) 
		{ 
			yerr -= distance; 
			uCol += incy; 
		}
	}
}
/**
  * @brief  简易虚线绘制
  * @param  x1,y1,x2,y2: 线段端点坐标
  * @param  color: 虚线颜色
  * @param  dash: 虚线周期总长（实线长度+空白长度，单位：像素）
  * @param  solid: 周期内的实线部分长度（单位：像素）
  * @note   例如：dash=8, solid=5 表示5像素实线+3像素空白的重复模式
  */
void LCD_DrawDashedLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color, u16 dash, u16 solid)
{
    // 计算X、Y方向的距离差
    int dx = x2 - x1, dy = y2 - y1;
    
    // 确定X、Y方向的步进方向（1=向右/下，-1=向左/上，0=水平/垂直）
    int stepX = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
    int stepY = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
    
    // 取距离差的绝对值（用于计算总步数）
    dx = dx > 0 ? dx : -dx;
    dy = dy > 0 ? dy : -dy;
    
    // 计算总步数（取X、Y方向中距离较大的值，保证线条完整）
    int len = dx > dy ? dx : dy;
    
    // 当前绘制点的坐标
    int x = x1, y = y1;
    // 误差累计变量（Bresenham算法核心，用于控制像素绘制位置）
    int err = 0;

    // 遍历所有像素点
    for (int i = 0; i <= len; i++) {
        // 虚线绘制判断：仅在周期内的实线部分绘制像素
        // i % dash 计算当前位置在周期中的偏移，小于solid则绘制
        if (i % dash < solid) {
            LCD_DrawPoint(x, y, color);  // 绘制单个像素
        }
        
        // Bresenham算法：计算下一个像素点的坐标
        if (dx > dy) {
            // X方向为长轴，优先X方向步进
            x += stepX;
            err += dy;  // 累计Y方向误差
            // 当误差超过X方向长度时，Y方向步进一次并修正误差
            if (err > dx) { err -= dx; y += stepY; }
        } else {
            // Y方向为长轴，优先Y方向步进
            y += stepY;
            err += dx;  // 累计X方向误差
            // 当误差超过Y方向长度时，X方向步进一次并修正误差
            if (err > dy) { err -= dy; x += stepX; }
        }
    }
}
    
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle( u16 x1, u16 y1, u16 x2, u16 y2, u16 color )
{
	LCD_DrawLine( x1, y1, x2, y1, color );
	LCD_DrawLine( x1, y1, x1, y2, color );
	LCD_DrawLine( x1, y2, x2, y2, color );
	LCD_DrawLine( x2, y1, x2, y2, color );
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_DrawCircle( u16 x0, u16 y0, u8 r, u16 color )
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint( x0+a, y0-b, color );             //5
 		LCD_DrawPoint( x0+b, y0-a, color );             //0           
		LCD_DrawPoint( x0+b, y0+a, color );             //4               
		LCD_DrawPoint( x0+a, y0+b, color );             //6 
		LCD_DrawPoint( x0-a, y0+b, color );             //1       
 		LCD_DrawPoint( x0-b, y0+a, color );             
		LCD_DrawPoint( x0-a, y0-b, color );             //2             
  	LCD_DrawPoint( x0-b, y0-a, color );             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

/**
 * 在指定位置画一个填充圆（逐行扫描版本）
 * (x,y): 中心点
 * r: 半径
 * color: 填充颜色
 */
void LCD_DrawFillCircle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    int di;
    
    a = 0;
    b = r;
    di = 3 - (r << 1);  // 判断下个点位置的标志
    
    while (a <= b) {
        // 逐行填充顶部和底部区域
        for (int dy = -a; dy <= a; dy++) {
            LCD_DrawPoint(x0 + b, y0 + dy, color);
            LCD_DrawPoint(x0 - b, y0 + dy, color);
        }
        
        // 逐行填充左侧和右侧区域
        for (int dy = -b; dy <= b; dy++) {
            LCD_DrawPoint(x0 + a, y0 + dy, color);
            LCD_DrawPoint(x0 - a, y0 + dy, color);
        }
        
        a++;
        // 使用Bresenham算法计算下一个圆弧点
        if (di < 0)
            di += 4 * a + 6;
        else {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

//在指定位置画一个十字,圆
//(usX,usY):中心点
//color    :颜色
void LCD_DrawCross ( uint16_t usX, uint16_t usY, u16 color )
{
  LCD_DrawLine ( usX-10, usY, usX+10, usY, color);
  LCD_DrawLine ( usX, usY-10, usX, usY+10, color);
	LCD_DrawCircle( usX, usY, 6, color );
}


/****************************************************************************
* 名    称: void LCD_ShowChar(u16 x,u16 y,u8 ch,u8 size,u16 backcolor,u16 pointcolor,u8 mode)
* 功    能：在指定位置显示一个字符,颜色为显示器默认颜色
* 入口参数：(x,y)起始坐标
*						ch: 要显示的字符:" "--->"~"
*						size:字体大小 12/16/24/32
*						mode:叠加方式(1)还是非叠加方式(0)
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 ch,u8 size,u8 mode,u16 color)
{
	uint8_t i, ucTemp, row, col, colbyte, colbyterange;
	const unsigned char *p;
	
	if( size!=12 && size!=16 && size!=24 && size!=32 && size!=48 )	return;
	
	switch( size )
	{
		case 12:	p = ucAscii_1206[ ch - ' ' ];	colbyterange = 1;		break;
		case 16:	p = ucAscii_1608[ ch - ' ' ];	colbyterange = 1;		break;
		case 24:	p = ucAscii_2412[ ch - ' ' ];	colbyterange = 2;		break;
		case 32:	p = ucAscii_3216[ ch - ' ' ];	colbyterange = 2;		break;
		case 48:	p = ucAscii_4824[ ch - ' ' ];	colbyterange = 3;	break;
		default:	break;
	}
		
	for( row=0; row<size; row++ )
	{
		col = 0;
		for( colbyte=0; colbyte<colbyterange; colbyte++ )
		{
			ucTemp = p[ row*colbyterange + colbyte ];
			
			for( i=0; i<8; i++ )
			{
				if ( ucTemp & 0x01 )
					LCD_DrawPoint( x+col, y+row, color );
				
				else if(mode==0)
					LCD_DrawPoint( x+col, y+row, lcddev.BACK_COLOR );
				
				ucTemp >>= 1;
				
				if( ++col>=(size/2) ) break;
			}
		}
	}
}
void LCD_ShowChar_all(u16 x,u16 y,u8 ch,u8 size,u8 mode,u16 color,u16 black_color)
{
	uint8_t i, ucTemp, row, col, colbyte, colbyterange;
	const unsigned char *p;
	
	if( size!=12 && size!=16 && size!=24 && size!=32 && size!=48 )	return;
	
	switch( size )
	{
		case 12:	p = ucAscii_1206[ ch - ' ' ];	colbyterange = 1;		break;
		case 16:	p = ucAscii_1608[ ch - ' ' ];	colbyterange = 1;		break;
		case 24:	p = ucAscii_2412[ ch - ' ' ];	colbyterange = 2;		break;
		case 32:	p = ucAscii_3216[ ch - ' ' ];	colbyterange = 2;		break;
		case 48:	p = ucAscii_4824[ ch - ' ' ];	colbyterange = 3;	break;
		default:	break;
	}
		
	for( row=0; row<size; row++ )
	{
		col = 0;
		for( colbyte=0; colbyte<colbyterange; colbyte++ )
		{
			ucTemp = p[ row*colbyterange + colbyte ];
			
			for( i=0; i<8; i++ )
			{
				if ( ucTemp & 0x01 )
					LCD_DrawPoint( x+col, y+row, color );
				
				else if(mode==0)
					LCD_DrawPoint( x+col, y+row, black_color );
				
				ucTemp >>= 1;
				
				if( ++col>=(size/2) ) break;
			}
		}
	}
}



/****************************************************************************
* 名    称: void Get_HzMat(uint8_t *code,uint8_t *mat,uint8_t size)
* 功    能：从字库中查找出字模
* 入口参数：code 字符串的开始地址,GBK码
*						mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
*						size:字体大小 12/16/24/32
* 返回参数：无
* 说    明：
****************************************************************************/
void Get_HzMat(uint8_t *code,uint8_t *mat,uint8_t size)
{		    
	uint8_t 	qh,ql;
	uint8_t 	i;					  
	uint32_t 	foffset; 
	uint8_t		csize;
	
	qh = *code;
	ql = *(++code);
	csize = (size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
	
	if( qh<0x81 || ql<0x40 || ql==0xff || qh==0xff )//非常用汉字
	{   		    
	   for(i=0;i<csize;i++) *mat++=0x00; //填充满格
	   return;
	}
	
	if( ql<0x7f ) ql-=0x40;//注意!
	else ql -= 0x41;
	
	qh -= 0x81;
	
	foffset=((uint32_t)190*qh+ql)*csize;	//得到字库中的字节偏移量
	
	switch(size)
	{
		case 12:
			W25QXX_Read(foffset+W25X_GBK12ADDR,mat,csize);
			break;
		case 16:
			W25QXX_Read(foffset+W25X_GBK16ADDR,mat,csize);
			break;
		case 24:
			W25QXX_Read(foffset+W25X_GBK24ADDR,mat,csize);
			break;
		case 32:
			W25QXX_Read(foffset+W25X_GBK32ADDR,mat,csize);
			break;
	}
}


/****************************************************************************
* 名    称: void LCD_ShowCharact(u16 x,u16 y,u8 *font,u8 size,u16 backcolor,u16 pointcolor,u8 mode)
* 功    能：在指定位置显示一个汉字
* 入口参数：(x,y)起始坐标
*						*font: 要显示的汉字, GBK编码
*						size:字体大小 12/16/24/32
*						backcolor,pointcolor: 背景色、字体颜色
*						mode:叠加方式(1)还是非叠加方式(0)
* 返回参数：无
* 说    明：字库取模方式为逐行、顺向、阴码
****************************************************************************/
void LCD_ShowCharact( uint16_t x, uint16_t y, uint8_t *font, uint8_t size, u8 mode,u16 color )
{
	uint8_t i, ucTemp, row, col, colbyte, colbyterange;
	uint8_t	dzk[144];
	
	if( size!=12 && size!=16 && size!=24 && size!=32 )	return;
	
	
	Get_HzMat( font, dzk, size );	//得到相应大小的点阵数据 
	
	switch( size )
	{
		case 12:	colbyterange = 2;		break;
		case 16:	colbyterange = 2;		break;
		case 24:	colbyterange = 3;		break;
		case 32:	colbyterange = 4;		break;
		case 48:	colbyterange = 6;		break;
		default:	break;
	}
		
	for( row=0; row<size; row++ )
	{
		col = 0;
		for( colbyte=0; colbyte<colbyterange; colbyte++ )
		{
			ucTemp = dzk[ row*colbyterange + colbyte ];
			
			for( i=0; i<8; i++ )
			{
				if ( ucTemp & 0x80 )
					LCD_DrawPoint( x+col, y+row, color);
				
				else if(mode==0)
					LCD_DrawPoint( x+col, y+row, lcddev.BACK_COLOR );
				
				ucTemp <<= 1;
				
				if( ++col>=size ) break;
			}
		}
	}	
}
void LCD_ShowCharact_all( uint16_t x, uint16_t y, uint8_t *font, uint8_t size, u8 mode,u16 color,u16 back_color )
{
	uint8_t i, ucTemp, row, col, colbyte, colbyterange;
	uint8_t	dzk[144];
	
	if( size!=12 && size!=16 && size!=24 && size!=32 )	return;

	
	Get_HzMat( font, dzk, size );	//得到相应大小的点阵数据 
	
	switch( size )
	{
		case 12:	colbyterange = 2;		break;
		case 16:	colbyterange = 2;		break;
		case 24:	colbyterange = 3;		break;
		case 32:	colbyterange = 4;		break;
		case 48:	colbyterange = 6;		break;
		default:	break;
	}
		
	for( row=0; row<size; row++ )
	{
		col = 0;
		for( colbyte=0; colbyte<colbyterange; colbyte++ )
		{
			ucTemp = dzk[ row*colbyterange + colbyte ];
			
			for( i=0; i<8; i++ )
			{
				if ( ucTemp & 0x80 )
					LCD_DrawPoint( x+col, y+row, color );
				
				else if(mode==0)
					LCD_DrawPoint( x+col, y+row,back_color );
				
				ucTemp <<= 1;
				
				if( ++col>=size ) break;
			}
		}
	}	
}


//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void LCD_Show_Str(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 size, u8 mode,u16 color)
{					
	u16 x0=x;
	u16 y0=y;							  	  
	u8 	bHz=0;     //字符或者中文
	
	if( size!=12 && size!=16 && size!=24 && size!=32 && size!=48 )	return;
	
	while(*str!=0)//数据未结束
	{
		if(!bHz)
		{
			if(*str>0x80) bHz=1;//中文 
			else               //字符
			{
				if(x>(x0+width-size/2))//换行
				{
					y+=size;
					x=x0;	   
				}							    
				if(y>(y0+height-size))break;//越界返回
				if(*str==13)//换行符号
				{
					y+=size;
					x=x0;
					str++;
				}
				else LCD_ShowChar(x,y,*str,size,mode, color);//有效部分写入 
				str++;
				x+=size/2; //字符,为全字的一半 
			}
		}
		else//中文 
		{
			bHz=0;
			if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
			if(y>(y0+height-size))break;//越界返回  						     
			LCD_ShowCharact(x,y,str,size,mode, color);
			str+=2; 
			x+=size;//下一个汉字偏移
		}
	}   
}

void LCD_Show_Str_all(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 size, u8 mode,u16 color,u16 back_color)
{					
	u16 x0=x;
	u16 y0=y;							  	  
	u8 	bHz=0;     //字符或者中文
	
	if( size!=12 && size!=16 && size!=24 && size!=32 && size!=48 )	return;
	
	
	while(*str!=0)//数据未结束
	{
		if(!bHz)
		{
			if(*str>0x80) bHz=1;//中文 
			else               //字符
			{
				if(x>(x0+width-size/2))//换行
				{
					y+=size;
					x=x0;	   
				}							    
				if(y>(y0+height-size))break;//越界返回
				if(*str==13)//换行符号
				{
					y+=size;
					x=x0;
					str++;
				}
				else LCD_ShowChar_all(x,y,*str,size,mode, color,back_color);//有效部分写入 
				str++;
				x+=size/2; //字符,为全字的一半 
			}
		}
		else//中文 
		{
			bHz=0;
			if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
			if(y>(y0+height-size))break;//越界返回  						     
			LCD_ShowCharact_all(x,y,str,size,mode,color,back_color);
			str+=2; 
			x+=size;//下一个汉字偏移
		}
	}   
}
/**
 * @brief       平方函数, m^n
 * @param       m: 底数
 * @param       n: 指数
 * @retval      m的n次方
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       扩展显示len个数字(高位是0也显示)
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @param       mode: 显示模式
 *              [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 * @param       color : 数字的颜色;
 * @retval      无
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)       /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                if (mode & 0x80)    /* 高位需要填充0 */
                {
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0x01, color);    /* 用0占位 */
                }
                else
                {
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);    /* 用空格占位 */
                }

                continue;
            }
            else
            {
                enshow = 1;         /* 使能显示 */
            }

        }

        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

void lcd_show_xnum_all(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color, uint16_t black_color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)       /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                if (mode & 0x80)    /* 高位需要填充0 */
                {
                    LCD_ShowChar_all(x + (size / 2) * t, y, '0', size, mode & 0x01, color,black_color);    /* 用0占位 */
                }
                else
                {
                    LCD_ShowChar_all(x + (size / 2) * t, y, ' ', size, mode & 0x01, color,black_color);    /* 用空格占位 */
                }

                continue;
            }
            else
            {
                enshow = 1;         /* 使能显示 */
            }

        }
        LCD_ShowChar_all(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color,black_color);
    }
}

void LCD_Show1bitPic( u16 x, u16 y, const unsigned char *gImage,u16 color )
{
	u16 col,row,bytepr,width,heigh;
	u16 x0=x;
	u8  i,temp;
	
	width = gImage[2]*256+gImage[3];
	heigh = gImage[4]*256+gImage[5];
	bytepr = width/8;
	if(width%8) bytepr++;
	
	for(row=0;row<heigh;row++)
	{
		for(col=0;col<bytepr;col++)
		{
			temp = gImage[row*bytepr+col+6];
			for(i=0;i<8;i++)
			{
				if(temp&0x80)		LCD_DrawPoint(x,y,color);
//				else 						LCD_DrawPoint(x,y);
				temp<<=1;
				x++;
				if((x-x0)==width)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}
}



/*image2lcd图片头文件信息：
********************************************************************************
 
第一个字节是扫描方式，低四位为0水平扫描，为1垂直扫描
第二字节代表输出灰度：
0x04--16灰，0x10--16位真彩色，0x12--18位真彩色，0x18--24位真彩色，0x20--32位真彩色；
第3~6个字节表示宽和高，假设图片数组BMP_BUF[],高位在前时
bmp_withd=(BMP_BUF[2]<<8)|BMP_BUF[3]
bmp_lenth=(BMP_BUF[4]<<8)|BMP_BUF[5]
数组大小 =  bmp_withd x bmp_lenth x (位数/8) + [ 8(包含头信息时)]
第七个字节代表颜色数据的排列方式： 0 --- 565； 1 – 555; (16位真色彩)
第八个字节代表？*/
/**
 * 基于image2lcd图片显示图片 (支持RGB565和单色)
 * @param x 起始X坐标
 * @param y 起始Y坐标
 * @param gImage 图片数据指针
 *               格式: 8字节头 + 像素数据
 *               头格式: [0-1]保留, [2-3]宽度, [4-5]高度, [6]颜色模式(0=单色, 1=RGB565), [7]保留
 * @param  color_1,color_black_1单色模式的笔色和底色
 */
 void LCD_ShowPic(u16 x, u16 y, const unsigned char *gImage,u16 color_1,u16 color_black_1)
{
    uint16_t width, height, color;
    uint32_t i, pixelCount;
    uint8_t colorMode;
    
    // 从图像头中提取信息
    width = (gImage[2] << 8) | gImage[3];
    height = (gImage[4] << 8) | gImage[5];
    colorMode = gImage[1]; // 颜色模式: 0x01=单色, 1=RGB565
    
	
    // 检查图像尺寸是否超出LCD范围
    if (x + width > lcddev.width || y + height > lcddev.height) {
        return; // 超出范围不显示
    }
	
    pixelCount = width * height;
    
    // 设置显示窗口
    LCD_Set_Window(x, y, width, height);
    
    // 准备写入显存
    LCD_WriteRAM_Prepare();
    
    if (colorMode == 0x10) { // RGB565模式
        for (i = 0; i < pixelCount; i++) {
            // 提取16位颜色值 (高字节在前)
            color = (gImage[i*2+8] << 8) | gImage[i*2+9];
            LCD_WR_DATA(color);
        }
    }
		else if(colorMode == 0x01)
		{ 
			int32_t  j, bytesPerLine;
			const unsigned char *pixelData;
			const unsigned char *lineData;
			pixelData=&gImage[6];
      // 单色模式
        bytesPerLine = (width + 7) / 8; // 每行的字节数，向上取整到字节边界
        
        for (j = 0; j < height; j++) {
            lineData = pixelData + j * bytesPerLine;
            
            for (i = 0; i < width; i++) {
                uint8_t byteValue = lineData[i / 8];
                uint8_t bitPos = 7 - (i % 8);
                
                color = (byteValue & (1 << bitPos)) ? color_1 : color_black_1;
                LCD_WR_DATA(color);
            }
        }
    }
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // 恢复窗口
}


#if LCD_XPT2046_ENABLE

uint8_t TouchLCD_Calibrate( void )
{
	uint8_t i;
	uint16_t usTest_x = 0, usTest_y = 0, usGap_x = 0, usGap_y = 0;
	uint16_t usScreenWidth, usScreenHeigth;
	char cStr[10];
	char *pStr = 0;	
	CoordinateTypeDef 	CrossCoordinate[4], ScreenSample[4];		// 校准点坐标, 测量坐标
	XPT2046_Calibration CalibrationFactor;
	
	usScreenWidth = lcddev.width;
	usScreenHeigth = lcddev.height;
	
	/* 设定“十”字交叉点的坐标 */ 
	CrossCoordinate[0].x = usScreenWidth >> 2;
	CrossCoordinate[0].y = usScreenHeigth >> 2;
	CrossCoordinate[1].x = CrossCoordinate[0] .x;
	CrossCoordinate[1].y = (usScreenHeigth*3) >> 2;
	CrossCoordinate[2].x = (usScreenWidth*3) >> 2;
	CrossCoordinate[2].y = CrossCoordinate[1].y;
	CrossCoordinate[3].x = CrossCoordinate[2].x;
	CrossCoordinate[3].y = CrossCoordinate[0].y;
	
	LCD_Clear( WHITE );
	LCD_Show_Str( 10, 10, 220, 16, (uint8_t *)"触屏Calibrate......", 16, 0 ,BLACK);

	for ( i = 0; i < 4; i++ )
	{
		sprintf( cStr, "%d", i+1 );
		LCD_Show_Str( usScreenWidth>>1, (usScreenHeigth>>1)-16, strlen(cStr)*8, 16, (uint8_t *)cStr, 16, 0 ,BLACK);		
		LCD_DrawCross( CrossCoordinate[i].x, CrossCoordinate[i].y, RED );  		//显示校正用的“十”字

		Delay_ms( 200 );		                                                   //适当的延时很有必要
		while ( !XPT2046_Scan( 1, &ScreenSample[i] ) )	IWDG_Feed();
	}
		
	
	XPT2046_Calculate_CalibrationFactor ( CrossCoordinate, ScreenSample, &CalibrationFactor ) ;  	 //用原始参数计算出 原始参数与坐标的转换系数
	
	if ( CalibrationFactor.Divider == 0 )	goto Failure; 

	usTest_x = ( ( CalibrationFactor.An * ScreenSample[3].x ) + ( CalibrationFactor.Bn * ScreenSample[3].y ) + CalibrationFactor.Cn ) / CalibrationFactor.Divider;		//取一个点计算X值	 
	usTest_y = ( ( CalibrationFactor.Dn * ScreenSample[3].x ) + ( CalibrationFactor.En * ScreenSample[3].y ) + CalibrationFactor.Fn ) / CalibrationFactor.Divider;    //取一个点计算Y值
	
	usGap_x = ( usTest_x > CrossCoordinate [ 3 ] .x ) ? ( usTest_x - CrossCoordinate [ 3 ] .x ) : ( CrossCoordinate [ 3 ] .x - usTest_x );   //实际X坐标与计算坐标的绝对差
	usGap_y = ( usTest_y > CrossCoordinate [ 3 ] .y ) ? ( usTest_y - CrossCoordinate [ 3 ] .y ) : ( CrossCoordinate [ 3 ] .y - usTest_y );   //实际Y坐标与计算坐标的绝对差
	
	if ( ( usGap_x > 10 ) || ( usGap_y > 10 ) ) goto Failure;       //可以通过修改这两个值的大小来调整精度    
	

	// 更新校准系数
	Touch_dev.dX_X = ( CalibrationFactor .An * 1.0 ) / CalibrationFactor .Divider;
	Touch_dev.dX_Y = ( CalibrationFactor .Bn * 1.0 ) / CalibrationFactor .Divider;
	Touch_dev.dX   = ( CalibrationFactor .Cn * 1.0 ) / CalibrationFactor .Divider;
	
	Touch_dev.dY_X = ( CalibrationFactor .Dn * 1.0 ) / CalibrationFactor .Divider;
	Touch_dev.dY_Y = ( CalibrationFactor .En * 1.0 ) / CalibrationFactor .Divider;
	Touch_dev.dY   = ( CalibrationFactor .Fn * 1.0 ) / CalibrationFactor .Divider;

//	/* 打印校校准系数 */ 
//	DEBUGOUT( "\r\nCalibrationPara:" );
//	DEBUGOUT( "\r\ndX_X = %f",  Touch_dev.dX_X);
//	DEBUGOUT( "\r\ndX_Y = %f",  Touch_dev.dX_Y);
//	DEBUGOUT( "\r\ndX = %f",  Touch_dev.dX);
//	DEBUGOUT( "\r\ndY_X = %f",  Touch_dev.dY_X);
//	DEBUGOUT( "\r\ndY_Y = %f",  Touch_dev.dY_Y);
//	DEBUGOUT( "\r\ndY = %f",  Touch_dev.dY);
	
	LCD_Clear( WHITE );
	pStr = "Calibrate Succed";
	LCD_Show_Str( (usScreenWidth-strlen(pStr)*8)>>1, usScreenHeigth>>1, strlen(pStr)*8, 16, (uint8_t *)pStr, 16, 0 ,BLACK);

	Delay_ms( 200 );

	return 1;    


Failure:	
	LCD_Clear( WHITE );
	pStr = "Calibrate fail";
	LCD_Show_Str( (usScreenWidth-strlen(pStr)*8)>>1, usScreenHeigth>>1, strlen(pStr)*8, 16, (uint8_t *)pStr, 16, 0,BLACK);
	
	Delay_ms( 200 );	
	return 0;
}
#endif




/****************************************************************************
* 名    称: void TouchLCD_Init(uint8_t lcddir)
* 功    能：触摸屏初始化	
* 入口参数：lcddir:0,竖屏; 1,横屏
* 返回参数: 无
* 说    明：
****************************************************************************/  
void TouchLCD_Init( uint8_t lcddir )
{
	LCD_Hard_Init();	
	LCD_Display_Dir( lcddir );		//竖屏
	LCD_Clear( WHITE );
	
	#if LCD_XPT2046_ENABLE
		XPT2046_Init(lcddir);
	#endif
	
	lcddev.BACK_COLOR  = WHITE;
	lcddev.POINT_COLOR = BLACK;
}



void LCD_ChangeBackArea(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{
    u16 x, y,height,width,pixel;
		u16 target_color = LCD_BGR2RGB(lcddev.BACK_COLOR);  // 确保颜色格式一致
    
		width=ex-sx+1; 			//宽度
		height=ey-sy+1;			//高度
	
   // 遍历整个屏幕
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // 读取当前像素
            pixel = LCD_ReadPoint(x, y);
            
            // 如果是背景色（通过颜色比较判断）
            if (pixel == lcddev.BACK_COLOR) {
                // 修改为新背景色
                LCD_DrawPoint(x, y, color);
            }
        }
    }
    
    // 更新背景色记录
    lcddev.BACK_COLOR = color;
    
}

void LCD_Clear_text(u16 sx,u16 sy,u16 ex,u16 ey)
{
	 u16 x, y,height,width,pixel;
		
		width=ex-sx+1; 			//宽度
		height=ey-sy+1;			//高度
	
   // 遍历整个屏幕
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // 读取当前像素
            pixel = LCD_ReadPoint(x, y);
            
            // 如果不是背景色（通过颜色比较判断）
            if (pixel != lcddev.BACK_COLOR) {
                // 修改为背景色
                LCD_DrawPoint(x, y, lcddev.BACK_COLOR);
            }
        }
			}
}

/**
  * @brief  在LCD上绘制三角形
  * @param  x1,y1: 第一个顶点坐标
  * @param  x2,y2: 第二个顶点坐标
  * @param  x3,y3: 第三个顶点坐标
  * @param  color: 线条颜色
  * @retval None
  */
void LCD_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, 
                     uint16_t x3, uint16_t y3, uint16_t color)
{
    LCD_DrawLine(x1, y1, x2, y2, color);
    LCD_DrawLine(x2, y2, x3, y3, color);
    LCD_DrawLine(x3, y3, x1, y1, color);
}

///**
//  * @brief  在LCD上绘制波形显示网格
//  * @param  x0, y0: 网格左上角坐标
//  * @param  width, height: 网格整体宽度和高度
//  * @param  gridSize: 网格大小(像素)
//  * @retval None
//  */
//void DrawWaveformGrid(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t gridSize)
//{
//    uint16_t i;
//    
//    // 绘制网格背景
//    LCD_Fill(x0, y0, x0+width, y0+height, WHITE);
//    
//    // 绘制垂直网格线
//    for(i = 0; i <= width; i += gridSize) {
//        if(i == width/2) {
//            LCD_DrawLine(x0+i, y0, x0+i, y0+height, BLACK);  // 中轴线加粗
//        } else {
//            LCD_DrawLine(x0+i, y0, x0+i, y0+height, LIGHTGRAY);
//        }
//    }
//    
//    // 绘制水平网格线
//    for(i = 0; i <= height; i += gridSize) {
//        if(i == height/2) {
//            LCD_DrawLine(x0, y0+i, x0+width, y0+i, BLACK);  // 中轴线加粗
//        } else {
//            LCD_DrawLine(x0, y0+i, x0+width, y0+i, LIGHTGRAY);
//        }
//    }
//    
//    // 绘制坐标轴箭头
//    LCD_DrawTriangle(x0+width, y0+height/2, x0+width-10, y0+height/2-5, x0+width-10, y0+height/2+5, BLACK);  // X轴箭头
//    LCD_DrawTriangle(x0+width/2, y0, x0+width/2-5, y0+10, x0+width/2+5, y0+10, BLACK);  // Y轴箭头
//    
//    // 添加坐标标签
//    LCD_Show_Str(x0+width/2-10, y0+height+10,320,240, (uint8_t *) "时间",12,0 ,BLACK);
//    LCD_Show_Str(x0-40, y0+height/2,320,240,(uint8_t *) "幅度",12,0 ,BLACK);
//}


/**
  * @brief  在LCD上绘制波形显示网格（支持自定义刻度、标签、原点位置及所有元素颜色）
  * @param  x1, y1: 网格左上角坐标
  * @param  x2, y2: 网格右下角坐标
  * @param  xTicks: X轴刻度值数组（从左到右)
  * @param  yTicks: Y轴刻度值数组（从上到下）
  * @param  xTickCount: X轴刻度数量
  * @param  yTickCount: Y轴刻度数量
  * @param  xLabel: X轴标签文本
  * @param  yLabel: Y轴标签文本
  * @param  originX: 原点X坐标（相对于屏幕左上角）
  * @param  originY: 原点Y坐标（相对于屏幕左上角）
  * @param  gridColor: 网格线颜色
  * @param  axisColor: 坐标轴（含箭头）颜色
  * @param  tickTextColor: 刻度文本颜色
  * @param  labelTextColor: 标签文本颜色
  * @retval None
  */
// 辅助宏定义（可放在头文件）
#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define IS_IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))

// 计算整数的位数，用于确定文本显示宽度
static inline uint8_t get_digits(int32_t value) {
    uint8_t digits = 1;
    if(value < 0) {
        digits++; // 负号占一个位置
        value = -value;
    }
    while(value >= 10) {
        value /= 10;
        digits++;
    }
    return digits;
}

uint32_t DrawWaveformGrid(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, 
                     int32_t *xTicks, int32_t *yTicks, uint8_t xTickCount, uint8_t yTickCount, 
                     char *xLabel, char *yLabel, uint16_t originX, uint16_t originY,
                     uint16_t gridColor, uint16_t axisColor, uint16_t tickTextColor, uint16_t labelTextColor)
{
    // 常量定义 - 文本显示参数和箭头尺寸
    const uint8_t TEXT_HEIGHT = 12;  // 文本高度（像素）
    const uint8_t TEXT_WIDTH = 6;    // 单个字符宽度（像素）
    const uint8_t ARROW_LEN = 10;    // 箭头长度（像素）
    const uint8_t ARROW_WIDTH = 5;   // 箭头宽度（像素）
    
    // 计算网格尺寸
    uint16_t width = x2 - x1;
    uint16_t height = y2 - y1;
    
    // 参数验证 - 确保输入参数有效
    if(width <= 0 || height <= 0 || xTickCount < 3 || yTickCount < 3 || 
       !xTicks || !yTicks || !xLabel || !yLabel)
        return 0;
    
    // 计算刻度间距（相邻刻度线之间的像素距离）
    uint16_t xTickSpacing = width / (xTickCount - 1);
    uint16_t yTickSpacing = height / (yTickCount - 1);
    
    // 找到最接近指定坐标的刻度索引
    uint8_t originXIndex = 0;
    uint8_t originYIndex = 0;
    
    // 寻找最接近的X轴刻度索引
    for(uint8_t i = 0; i < xTickCount; i++) {
        uint16_t tickPos = x1 + i * xTickSpacing;
        if(abs(tickPos - originX) < abs(x1 + originXIndex * xTickSpacing - originX)) {
            originXIndex = i;
        }
    }
    
    // 寻找最接近的Y轴刻度索引
    for(uint8_t i = 0; i < yTickCount; i++) {
        uint16_t tickPos = y1 + i * yTickSpacing;
        if(abs(tickPos - originY) < abs(y1 + originYIndex * yTickSpacing - originY)) {
            originYIndex = i;
        }
    }
    
    // 确保原点周围有足够空间（至少留一个网格间距）
    if(originXIndex < 1) originXIndex = 1;
    if(originXIndex > xTickCount - 2) originXIndex = xTickCount - 2;
    if(originYIndex < 1) originYIndex = 1;
    if(originYIndex > yTickCount - 2) originYIndex = yTickCount - 2;
    
    // 更新原点坐标为对应刻度位置
    originX = x1 + originXIndex * xTickSpacing;
    originY = y1 + originYIndex * yTickSpacing;
    
    // 绘制背景（白色填充）
    LCD_Fill(x1, y1, x2, y2, WHITE);
    
    // 绘制坐标轴（原点处的水平和垂直线）
    LCD_DrawLine(originX, y1, originX, y2, axisColor);  // Y轴
    LCD_DrawLine(x1, originY, x2, originY, axisColor);  // X轴
    
    // 绘制垂直网格线和刻度值
    for(uint8_t i = 0; i < xTickCount; i++) {
        uint16_t xPos = x1 + i * xTickSpacing;
        uint16_t color = (xPos == originX) ? axisColor : gridColor;
        
        // 绘制垂直网格线
        LCD_DrawLine(xPos, y1, xPos, y2, color);
        
        // 绘制刻度值文本（跳过原点，原点处不显示刻度值）
        if(xPos != originX && IS_IN_RANGE(xPos, x1 + TEXT_WIDTH, x2 - TEXT_WIDTH)) {
            char label[16];
            snprintf(label, sizeof(label), "%d", xTicks[i]);
            uint8_t digits = get_digits(xTicks[i]);
            
            // 刻度文本显示在X轴下方
            uint16_t yTextPos = originY + 2;
            uint16_t xTextPos = xPos - (TEXT_WIDTH * digits) / 2;  // 文本居中显示
            
            // 确保文本不会超出网格边界
            if(IS_IN_RANGE(yTextPos, y1, y2 - TEXT_HEIGHT))
                LCD_Show_Str(xTextPos, yTextPos, 320, 240, (uint8_t*)label, 12, 0, tickTextColor);
        }
    }
    
    // 绘制水平网格线和刻度值
    for(uint8_t i = 0; i < yTickCount; i++) {
        uint16_t yPos = y1 + i * yTickSpacing;
        uint16_t color = (yPos == originY) ? axisColor : gridColor;
        
        // 绘制水平网格线
        LCD_DrawLine(x1, yPos, x2, yPos, color);
        
        // 绘制刻度值文本（跳过原点，原点处不显示刻度值）
        if(yPos != originY && IS_IN_RANGE(yPos, y1 + TEXT_HEIGHT/2, y2 - TEXT_HEIGHT/2)) {
            char label[16];
            snprintf(label, sizeof(label), "%d", yTicks[i]);
            uint8_t digits = get_digits(yTicks[i]);
            
            // 刻度文本显示在Y轴左侧
            uint16_t xTextPos = originX - TEXT_WIDTH * digits - 2;
            uint16_t yTextPos = yPos - TEXT_HEIGHT/2;  // 文本垂直居中
            
            // 确保文本不会超出网格边界
            if(IS_IN_RANGE(xTextPos, x1, x2 - TEXT_WIDTH * digits))
                LCD_Show_Str(xTextPos, yTextPos, 320, 240, (uint8_t*)label, 12, 0, tickTextColor);
        }
    }
    
    // 绘制X轴正方向箭头（向右）
    LCD_DrawTriangle(x2, originY, x2 - ARROW_LEN, originY - ARROW_WIDTH, 
                    x2 - ARROW_LEN, originY + ARROW_WIDTH, axisColor);
    
    // 绘制Y轴正方向箭头（向上）
    LCD_DrawTriangle(originX, y1, originX - ARROW_WIDTH, y1 + ARROW_LEN, 
                    originX + ARROW_WIDTH, y1 + ARROW_LEN, axisColor);
    
    // 绘制轴标签（X轴和Y轴名称）
    uint16_t xLabelPos = x2 - TEXT_WIDTH * strlen(xLabel) - ARROW_LEN;
    uint16_t yLabelPos = y1 + ARROW_LEN;
    
    LCD_Show_Str(xLabelPos, originY + 12, 320, 240, (uint8_t*)xLabel, 12, 0, labelTextColor);
    LCD_Show_Str(originX + 6, yLabelPos, 320, 240, (uint8_t*)yLabel, 12, 0, labelTextColor);
    
    // 在原点位置显示"0"
    LCD_Show_Str(originX - 6, originY+1, 320, 240, (uint8_t*)"0", 12, 0, tickTextColor);
		
		
		// 返回原点坐标（高16位为X坐标，低16位为Y坐标）
    return ((uint32_t)originX << 16) | originY;
}


/**
  * @brief  绘制AD采样波形（以传入原点为基准，X轴向右递增）
  * @param  x1, y1: 波形区域左上角坐标
  * @param  x2, y2: 波形区域右下角坐标
  * @param  adData: AD采样数据数组（有符号值，支持正负）
  * @param  dataLen: 数据长度
  * @param  originX, originY: 原点坐标（相对于屏幕左上角，不做自动调整）
  * @param  yMin, yMax: Y轴显示范围（有符号值，如-2048~2048）
  * @param  mode: 绘制模式（DRAW_POINT=画点，DRAW_LINE=画线）
  * @param  waveColor: 波形颜色
  */
void DrawAdWaveform(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                   int16_t *adData, uint16_t dataLen,
                   uint16_t originX, uint16_t originY,
                   int16_t yMin, int16_t yMax,
                   DrawMode mode, uint16_t waveColor)
{
    // 参数验证
    if(x1 >= x2 || y1 >= y2 || !adData || dataLen == 0 || yMin >= yMax)
        return;
    
		
    // 计算Y轴缩放因子
    float yScale = (float)(y2 - y1) / (yMax - yMin);
    
    // 计算可显示的数据点数量（从原点开始向右）
    uint16_t displayPoints = x2 - originX + 1;
    if(displayPoints > dataLen)
        displayPoints = dataLen;
    
    // 画点模式
    if(mode == DRAW_POINT) {
        for(uint16_t i = 0; i < displayPoints; i++) {
            uint16_t x = originX + i;  // 从原点开始向右递增
            
            // 确保X坐标在显示区域内
            if(x > x2) break;
            
            int16_t adValue = adData[i];
            
            // 限幅并映射Y坐标
            adValue = CLAMP(adValue, yMin, yMax);
            
            // 修正的Y轴映射公式
            uint16_t y = originY - adValue * yScale;
            
            // 绘制点
            LCD_DrawPoint(x, y, waveColor);
        }
    }
    // 画线模式
    else if(mode == DRAW_LINE) {
        for(uint16_t i = 0; i < displayPoints - 1; i++) {
            uint16_t x0 = originX + i;
            uint16_t x1_pos = originX + i + 1;
            
            // 确保线段起点和终点在显示区域内
            if(x0 > x2 || x1_pos > x2) break;
            
            // 修正：从数组读取数据并修复限幅
            int16_t ad0 = CLAMP(adData[i], yMin, yMax);
            int16_t ad1 = CLAMP(adData[i + 1], yMin, yMax);
            
            // 修正的Y轴映射公式
            uint16_t y0 = originY - ad0 * yScale;
            uint16_t y1_pos = originY - ad1 * yScale;
            
            // 绘制线段
            LCD_DrawLine(x0, y0, x1_pos, y1_pos, waveColor);
        }
    }
}
/**
  * @brief  绘制无框进度条（参数化字体版）
  * @param  x1,y1: 左上角坐标 | x2,y2: 右下角坐标
  * @param  percent: 进度百分比(0-100)，注意计算使用的类型，最好float
  * @param  bgColor: 背景色 | barColor: 进度条颜色
  * @param  showText: 是否显示文本 | textColor: 文本颜色
  * @param  fontSize: 字体大小(8,12,16等)
  */
void LCD_ShowProgressBar(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                        uint8_t percent, uint16_t bgColor, uint16_t barColor,
                        uint8_t showText, uint16_t textColor, uint8_t fontSize)
{
    if(x1 >= x2 || y1 >= y2 || percent > 100) return;
    
    uint16_t w = x2 - x1, h = y2 - y1;
    uint16_t barEnd = x1 + (percent == 100 ? w : w * percent / 100);
    
    static uint8_t lastP = 255;    // 记录上次进度值
    static uint16_t lastTextX = 0;  // 记录上次文本起始位置
    static uint8_t lastFontSize = 0; // 记录上次字体大小
    
    // 快速路径：进度、字体未变且无需文本更新时直接返回
    if(!showText && lastP == percent && lastFontSize == fontSize) return;
    
    // 计算文本区域位置（根据字体大小调整）
    uint8_t charWidth = fontSize >= 16 ? 8 : 6;  // 字体宽度估计
    uint16_t textX = x1 + (w - (percent == 100 ? 3*charWidth : 2*charWidth+charWidth/2)) / 2;
    uint16_t textY = y1 + (h - fontSize) / 2;
    
    // 智能清除上次文本（处理与进度条的重叠）
    if(lastP != percent || lastFontSize != fontSize) {
        if(lastP != 255) {
            uint8_t lastCharWidth = lastFontSize >= 16 ? 8 : 6;
            uint16_t textW = lastP == 100 ? 3*lastCharWidth : 2*lastCharWidth+lastCharWidth/2;
            uint16_t textEnd = lastTextX + textW;
            
            // 处理进度条覆盖的文本区域
            if(lastTextX < barEnd) {
                uint16_t s = lastTextX > x1 ? lastTextX : x1;
                uint16_t e = textEnd < barEnd ? textEnd : barEnd;
                LCD_Fill(s, textY, e, textY + fontSize, barColor);
            }
            
            // 处理超出进度条的文本区域
            if(textEnd > barEnd)
                LCD_Fill(barEnd, textY, textEnd, textY + fontSize, bgColor);
        }
    }
    
    // 背景和进度条更新
    if(lastP == 255 || percent < lastP)  // 首次绘制或进度减少时重绘背景
        LCD_Fill(x1, y1, x2, y2, bgColor);
    
    if(percent > lastP)  // 仅更新新增的进度条区域
        LCD_Fill(x1 + (lastP == 255 ? 0 : w * lastP / 100), y1, barEnd, y2, barColor);
    
    // 显示新文本（百分比或"完成!"）
    if(showText) {
        if(percent == 100)
            LCD_Show_Str(textX, textY, w, h, (uint8_t*)"完成!", fontSize, 1, textColor);
        else {
            lcd_show_xnum(textX, textY, percent, 2, fontSize, 1, textColor);
            LCD_Show_Str(textX + 2*charWidth, textY, w, h, (uint8_t*)"%", fontSize, 1, textColor);
        }
    }
    
    // 更新记录供下次使用
    lastP = percent;
    lastTextX = textX;
    lastFontSize = fontSize;
}
/***************************w25显示背景图片及背景上的指定区域清除***********************************
*LCD_ShowPicFromFlash
*使用效果
*背景显示：可根据图片数组是横的图或竖的图，改need_rotate，0：竖 1:横
*在竖屏 LCD 上：图片按原始水平扫描方向显示（宽为水平方向，高为垂直方向）。
*在横屏 LCD 上：图片自动旋转 90 度，但保持水平显示（例如，竖屏图片在横屏 LCD 上仍显示为竖的，但占据横屏的宽度）。
*这种方式确保了图片始终以其原始水平扫描方向显示，不受 LCD 横竖屏设置的影响
*竖屏图片横向显示
*
*LCD_ClearNonBackground_Fast_WithRotate
*使用效果
*背景上指定区域清除，指定区域坐标按LCD横竖为基准看
****************************************************************************************/
#define BACKGROUND_ADDR_IN_FLASH    W25X_IMAGE_3_START  // 背景图片在W25Q中的起始地址
#define MAX_BLOCK_SIZE  512         // 单次处理的最大像素数（可根据RAM调整）
#define MIN_1(a, b) ((a) < (b) ? (a) : (b))
// 使用同静态缓冲区，节省RAM
static uint8_t pixel_buffer[MAX_BLOCK_SIZE * 2];  // 像素缓冲区（2字节/像素）

// 从W25Q读取背景图片信息（宽高）
void LCD_ReadBackgroundInfo(uint16_t *width, uint16_t *height)
{
    uint8_t header[8];
    W25QXX_Read(BACKGROUND_ADDR_IN_FLASH, header, 8);
    *width = (header[2] << 8) | header[3];    // 解析宽度
    *height = (header[4] << 8) | header[5];   // 解析高度
}

// 直接覆盖指定区域的背景（支持旋转参数）
void LCD_ClearNonBackground_Fast_WithRotate(u16 sx, u16 sy, u16 ex, u16 ey, unsigned char need_rotate)
{
    uint16_t bg_width, bg_height;       // 背景图片宽高
    uint16_t disp_width, disp_height;   // 显示区域宽高
    uint32_t bg_addr;                   // 背景图片在W25Q中的地址
    unsigned char mirror_horizontal = (lcddev.id == 0x9341); // 是否需要水平镜像
    
    // 获取背景图片尺寸
    LCD_ReadBackgroundInfo(&bg_width, &bg_height);
    
    // 设置显示窗口宽高（考虑旋转）
    if (need_rotate) {
        disp_width = bg_height;
        disp_height = bg_width;
    } else {
        disp_width = bg_width;
        disp_height = bg_height;
    }
    
    // 调整清除区域，确保不超出背景范围
    if (sx >= disp_width || sy >= disp_height) return;  // 区域完全超出背景范围
    if (ex >= disp_width) ex = disp_width - 1;          // 裁剪X边界
    if (ey >= disp_height) ey = disp_height - 1;        // 裁剪Y边界


    // 设置LCD显示窗口
    LCD_Set_Window(sx, sy, ex - sx + 1, ey - sy + 1);
    LCD_WriteRAM_Prepare();  // 准备写入GRAM

    

    // 根据旋转和镜像需求处理数据
    if (need_rotate) {
        // 需要旋转显示
        for (uint16_t y = 0; y < (ey - sy + 1); y++) {
            for (uint16_t x = 0; x < (ex - sx + 1); x++) {
                uint16_t src_x, src_y;
                
                // 将目标坐标转换为源图片坐标
                if (mirror_horizontal) {
                    src_x = bg_width - 1 - (sy + y);
                    src_y = sx + x;
                } else {
                    src_x = sy + y;
                    src_y = sx + x;
                }
                
                // 计算当前像素在背景中的地址
                bg_addr = BACKGROUND_ADDR_IN_FLASH + 8  // 跳过8字节头
                         + (uint32_t)src_y * bg_width * 2  // 前面所有行的像素
                         + src_x * 2;  // 当前行的偏移
                
                // 从W25Q读取背景像素
                W25QXX_Read(bg_addr, pixel_buffer, 2);  // 读取1个像素(2字节)
                
                // 写入像素数据
                uint16_t color = (pixel_buffer[0] << 8) | pixel_buffer[1];
                LCD_WR_DATA(color);
            }
        }
    } else {
        // 不需要旋转，直接按行读取
        for (uint16_t y = 0; y < (ey - sy + 1); y++) {
            for (uint16_t x = 0; x < (ex - sx + 1); x++) {
                // 计算当前像素在背景中的地址
                bg_addr = BACKGROUND_ADDR_IN_FLASH + 8  // 跳过8字节头
                         + (uint32_t)(sy + y) * bg_width * 2  // 前面所有行的像素
                         + (sx + x) * 2;  // 当前行的偏移
                
                // 从W25Q读取背景像素
                W25QXX_Read(bg_addr, pixel_buffer, 2);  // 读取1个像素(2字节)
                
                // 写入像素数据
                uint16_t color = (pixel_buffer[0] << 8) | pixel_buffer[1];
                LCD_WR_DATA(color);
            }
        }
    }

    // 恢复LCD全屏窗口
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}
// 优化后的图片显示函数（支持旋转）
void LCD_ShowPicFromFlash_all(u16 x, u16 y, uint32_t flashAddr, unsigned char need_rotate)
{
    uint8_t header[8];
    uint16_t img_width, img_height;  // 图片原始宽高
    uint16_t disp_width, disp_height; // 显示窗口宽高
    uint32_t pixel_addr;             // 像素在Flash中的地址
    uint16_t color, count, i;        // 临时变量
    unsigned char mirror_horizontal = (lcddev.id == 0x9341);  // 是否需要水平镜像
    
    // 读取图片头，解析原始宽高
    W25QXX_Read(flashAddr, header, 8);
    img_width = (header[2] << 8) | header[3];
    img_height = (header[4] << 8) | header[5];

    // 设置显示窗口宽高
    if (need_rotate) {
        disp_width = img_height;
        disp_height = img_width;
    } else {
        disp_width = img_width;
        disp_height = img_height;
    }

    // 检查显示范围
    if (x + disp_width > lcddev.width || y + disp_height > lcddev.height) {
        return;
    }

    // 设置LCD显示窗口
    LCD_Set_Window(x, y, disp_width, disp_height);
    LCD_WriteRAM_Prepare();  // 准备写入GRAM

    // 根据旋转和镜像需求读取数据
    if (need_rotate) {
        // 需要旋转显示
        if (mirror_horizontal) {
            // 水平镜像：从右到左读取列
            for (int16_t col = img_width - 1; col >= 0; col--) {
                for (uint16_t row = 0; row < img_height; row += MAX_BLOCK_SIZE) {
                    count = MIN_1(img_height - row, MAX_BLOCK_SIZE);
                    for (i = 0; i < count; i++) {
                        pixel_addr = flashAddr + 8 + (uint32_t)(row + i) * img_width * 2 + col * 2;
                        W25QXX_Read(pixel_addr, &pixel_buffer[i * 2], 2);
                    }
                    for (i = 0; i < count; i++) {
                        color = (pixel_buffer[i * 2] << 8) | pixel_buffer[i * 2 + 1];
                        LCD_WR_DATA(color);
                    }
                }
            }
        } else {
            // 正常旋转：从左到右读取列
            for (uint16_t col = 0; col < img_width; col++) {
                for (uint16_t row = 0; row < img_height; row += MAX_BLOCK_SIZE) {
                    count = MIN_1(img_height - row, MAX_BLOCK_SIZE);
                    for (i = 0; i < count; i++) {
                        pixel_addr = flashAddr + 8 + (uint32_t)(row + i) * img_width * 2 + col * 2;
                        W25QXX_Read(pixel_addr, &pixel_buffer[i * 2], 2);
                    }
                    for (i = 0; i < count; i++) {
                        color = (pixel_buffer[i * 2] << 8) | pixel_buffer[i * 2 + 1];
                        LCD_WR_DATA(color);
                    }
                }
            }
        }
    } else {
        // 不需要旋转，直接按行读取
        for (uint16_t row = 0; row < img_height; row++) {
            uint32_t row_start_addr = flashAddr + 8 + (uint32_t)row * img_width * 2;
            for (uint16_t col = 0; col < img_width; col += MAX_BLOCK_SIZE) {
                count = MIN_1(img_width - col, MAX_BLOCK_SIZE);
                W25QXX_Read(row_start_addr + col * 2, pixel_buffer, count * 2);
                for (i = 0; i < count; i++) {
                    color = (pixel_buffer[i * 2] << 8) | pixel_buffer[i * 2 + 1];
                    LCD_WR_DATA(color);
                }
            }
        }
    }

    // 恢复LCD全屏窗口
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}



/**
 * 从W25QXX读取图片并显示（修复错位版）
 * @param x 起始X坐标
 * @param y 起始Y坐标
 * @param flashAddr 图片在Flash中的起始地址
 *  图片数据：RGB565格式，带8字节头（含宽高），水平扫描（逐行从左到右）
 */
#define PIXEL_BUF_SIZE 512  // 缓冲区大小（字节），需为2的倍数（每个像素2字节）
void LCD_ShowPicFromFlash(u16 x, u16 y, uint32_t flashAddr)
{
    uint8_t header[8];
    uint16_t width, height;       // 图片宽高
    uint16_t row, col;            // 行、列索引
    uint16_t row_pixel_count;     // 每行像素数（即width）
    uint32_t row_start_addr;      // 当前行在Flash中的起始地址
    uint8_t pixelBuffer[PIXEL_BUF_SIZE];  // 行内分块缓冲区
    uint16_t color, count, i;     // 临时变量

    // 1. 读取图片头，解析宽高
    W25QXX_Read(flashAddr, header, 8);
    width = (header[2] << 8) | header[3];    // 解析宽度（确保与图片头格式一致）
    height = (header[4] << 8) | header[5];   // 解析高度

    // 检查显示范围（超出LCD则不显示）
    if (x + width > lcddev.width || y + height > lcddev.height) {
        return;
    }

    // 2. 设置LCD显示窗口（起始坐标+宽高，适配LCD_Set_Window逻辑）
    LCD_Set_Window(x, y, width, height);  // 内部会自动计算右下角坐标
    LCD_WriteRAM_Prepare();  // 准备写入GRAM

    row_pixel_count = width;  // 每行像素数 = 图片宽度

    // 3. 按行读取并显示（核心优化：避免跨行列分块）
    for (row = 0; row < height; row++) {
        // 计算当前行在Flash中的起始地址（跳过8字节头 + 前面所有行的像素）
        row_start_addr = flashAddr + 8 + (uint32_t)row * row_pixel_count * 2;

        // 4. 行内分块读取（确保在当前行内完成，不跨到下一行）
        for (col = 0; col < row_pixel_count; col += PIXEL_BUF_SIZE / 2) {
            // 计算当前块的像素数（不超过当前行剩余像素）
            count = MIN_1(row_pixel_count - col, PIXEL_BUF_SIZE / 2);

            // 从Flash读取当前行的一块像素数据
            W25QXX_Read(row_start_addr + col * 2, pixelBuffer, count * 2);

            // 将当前块像素写入LCD（按顺序逐点显示，匹配LCD行扫描方向）
            for (i = 0; i < count; i++) {
                // 解析RGB565颜色（大端模式：高字节在前）
                color = (pixelBuffer[i * 2] << 8) | pixelBuffer[i * 2 + 1];
                LCD_WR_DATA(color);  // 写入LCDGRAM，窗口模式下自动按顺序填充
            }
        }
    }

    // 5. 恢复窗口为全屏（避免影响后续显示）
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}
