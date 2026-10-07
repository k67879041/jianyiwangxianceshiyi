
#include "TouchKeypad.h"

KeypadTypeDef	KeypadPhone,KeypadCalc,KeypadEng,KeypadEngCAP,KeypadSymbol,DIY_Keypad;
KeypadTypeDef *CurrentKeypad = NULL;

/*
*********************************************************************************************************
*	函 数 名: Keypad_Init
*	功能说明: 键盘初始化
*						根据键盘类型设置键盘中各按键的大小、颜色、显示内容和键值
*	参    数: *keypad:		键盘指针
*						type:				键盘类型
*						(x0,y0):		键盘起始坐标
*						_width,_height:	键盘大小
*	返 回 值: 1成功 0失败
*********************************************************************************************************
*/
uint8_t Keypad_Init( KeypadTypeDef *keypad, KeypadType type, u16 x0, u16 y0, u16 _width, u16 _height )
{
	uint8_t i,j,keynum,dmin;
	float w,h,dw,dh;
	
	// 参数校验
	if( keypad==NULL )	return 0;
	if( x0+_width>lcddev.width )		return 0;
	if( y0+_height>lcddev.height )	return 0;
	
	switch( TOUCHKEY_SIZE )
	{
		case 1:	dmin = 2;	break;
		case 2:	dmin = 3;	break;
		case 3:	dmin = 4;	break;
		case 4:	dmin = 5;	break;
		case 5:	dmin = 7;	break;
		default:	break;
	}
	
	if( type==KEYPAD_PHONE )	// 手机键盘, 3行4列
	{
		dw = _width/16.0f;
		w = _width/4.0f;
		dh = _height/37.0f;
		h = _height*8.0f/37.0f;
		keynum = 12;
	}
	else if( type==KEYPAD_CALC )	// 计算器键盘, 4行4列
	{
		dw = _width/21.0f;
		w = _width*4.0f/21.0f;
		dh = _height/37.0f;
		h = _height*8.0f/37.0f;
		keynum = 16;
	}
	else if( type==KEYPAD_ENG || type==KEYPAD_ENGCAPS )	// 35键英文全键盘
	{
		dw = _width/51.0f;
		w = _width*4.0f/51.0f;
		dh = _height/37.0f;
		h = _height*8.0f/37.0f;
		keynum = 35;
	}
	else if( type==KEYPAD_SYMBOL )	// 符号键盘,未完成
	{
	}
	else if( type==DIY_KEYPAD )	// DIY键盘
	{
	// 计算间隔和按键尺寸（将宽度均分，高度按比例分配）
    dw = 8;      // 水平间隔 = 总宽度 ÷ 10
    w = _width-2*dw;        // 按键宽度 = 总宽度 - 两侧间隔
    dh = 4;     // 垂直间隔 = 总高度 ÷ 21
    h = (_height - 5*dh) / 4; // 按键高度 = (总高度 - 5个间隔) ÷ 4
		keynum = 4;
	}
	// 键盘尺寸不足, 返回
	if( dw<dmin || dh<dmin )	return 0;
	
	keypad->Type = type;
	keypad->sx = x0;
	keypad->sy = y0;
	keypad->width = _width;
	keypad->height = _height;	
	
	if( type==KEYPAD_PHONE )
	{
		for( i=0; i<keynum; i++ )
		{
			keypad->Key[i].T_sta = 0;
			
			keypad->Key[i].width = (uint16_t)w;
			keypad->Key[i].height = (uint16_t)h;
			keypad->Key[i].sx = (uint16_t)(x0+dw+(dw+w)*(i%3));
			keypad->Key[i].sy = (uint16_t)(y0+dh+(dh+h)*(i/3));
			
			keypad->Key[i].ReleaseBackColor = KEYReleaseBackColor;
			keypad->Key[i].ReleasePointColor = KEYReleasePointColor;
			keypad->Key[i].PressBackColor = KEYPressBackColor;
			keypad->Key[i].PressPointColor = KEYPressPointColor;
			
			for(j=0;j<10;j++)	{
				keypad->Key[i].mess0[j] = keypad->Key[i].mess1[j] = '\0';
			}
			
			if( i<9 ){
				keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = i+0x31;
			}
			else if( i==9 ){
				keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '*';
			}
			else if( i==10 ){
				keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '0';
			}
			else if( i==11 ){
				keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '#';
			}
		}
	}
	
	else if( type==KEYPAD_CALC )
	{
		for( i=0; i<keynum; i++ )
		{
			keypad->Key[i].T_sta = 0;
			
			keypad->Key[i].width = (uint16_t)w;
			keypad->Key[i].height = (uint16_t)h;
			keypad->Key[i].sx = (uint16_t)(x0+dw+(dw+w)*(i%4));
			keypad->Key[i].sy = (uint16_t)(y0+dh+(dh+h)*(i/4));
			
			keypad->Key[i].ReleaseBackColor = KEYReleaseBackColor;
			keypad->Key[i].ReleasePointColor = KEYReleasePointColor;
			keypad->Key[i].PressBackColor = KEYPressBackColor;
			keypad->Key[i].PressPointColor = KEYPressPointColor;
			
			for(j=0;j<10;j++)	{
				keypad->Key[i].mess0[j] = keypad->Key[i].mess1[j] = '\0';
			}
			
			switch(i)
			{
				case 0:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '7';
								break;
				case 1:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '8';
								break;
				case 2:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '9';
								break;
				case 3:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '/';
								break;
				case 4:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '4';
								break;
				case 5:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '5';
								break;
				case 6:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '6';
								break;
				case 7:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '*';
								break;
				case 8:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '1';
								break;
				case 9:	
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '2';
								break;
				case 10:
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '3';
								break;
				case 11:
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '-';
								break;
				case 12:
								keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'C';
								keypad->Key[i].T_key = KEY_CLEAR;
								break;
				case 13:
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '0';
								break;
				case 14:
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '.';
								break;
				case 15:
								keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '+';
								break;
				default:break;
			}
		}
	}
	// 35键英文键盘
	else if( type==KEYPAD_ENG || type==KEYPAD_ENGCAPS )
	{
		for( i=0; i<keynum; i++ )
		{
			keypad->Key[i].T_sta = 0;
			
			keypad->Key[i].width = (uint16_t)w;
			keypad->Key[i].height = (uint16_t)h;
			
			keypad->Key[i].ReleaseBackColor = KEYReleaseBackColor;
			keypad->Key[i].ReleasePointColor = KEYReleasePointColor;
			keypad->Key[i].PressBackColor = KEYPressBackColor;
			keypad->Key[i].PressPointColor = KEYPressPointColor;
						
			if( i<10 )	// 第一行
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+(dw+w)*i);
				keypad->Key[i].sy = (uint16_t)(y0+dh);
			}
			else if( i<19 )	// 第二行
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i-10));
				keypad->Key[i].sy = (uint16_t)(y0+2*dh+h);
			}
			else if( i==19 )	// 第三行1 "Shift"
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw);
				keypad->Key[i].sy = (uint16_t)(y0+3*dh+2*h);
				keypad->Key[i].width = w*3/2;
			}
			else if( i<27 )	// 第三行
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i-19));
				keypad->Key[i].sy = (uint16_t)(y0+3*dh+2*h);
			}
			else if( i==27 )	// 第三行9 "Backspace"
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i-19));
				keypad->Key[i].sy = (uint16_t)(y0+3*dh+2*h);
				keypad->Key[i].width = (uint16_t)(w*3/2);
			}
			else if( i==28 )	// 第四行1 "Shift"
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw);
				keypad->Key[i].sy = (uint16_t)(y0+4*dh+3*h);
				keypad->Key[i].width = (uint16_t)(w*3/2);
			}
			else if( i<31 )	// 第四行2,3
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i-28));
				keypad->Key[i].sy = (uint16_t)(y0+4*dh+3*h);
			}
			else if( i==31 )	// 第四行4 "Space"
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i-28));
				keypad->Key[i].sy = (uint16_t)(y0+4*dh+3*h);
				keypad->Key[i].width = (uint16_t)(w*3+dw*2);
			}
			else if( i<34 )	// 第四行5,6
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i+2-28));
				keypad->Key[i].sy = (uint16_t)(y0+4*dh+3*h);
			}
			else if( i==34 )	// 第四行7
			{
				keypad->Key[i].sx = (uint16_t)(x0+dw+w/2+(dw+w)*(i+2-28));
				keypad->Key[i].sy = (uint16_t)(y0+4*dh+3*h);
				keypad->Key[i].width = (uint16_t)(w*3/2);
			}
			for(j=0;j<10;j++)	{
				keypad->Key[i].mess0[j] = keypad->Key[i].mess1[j] = '\0';
			}
			
			switch(i)
			{
				case 0:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'q'-0x20*(type-KEYPAD_ENG);
								break;
				case 1:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'w'-0x20*(type-KEYPAD_ENG);
								break;
				case 2:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'e'-0x20*(type-KEYPAD_ENG);
								break;
				case 3:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'r'-0x20*(type-KEYPAD_ENG);
								break;
				case 4:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 't'-0x20*(type-KEYPAD_ENG);
								break;
				case 5:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'y'-0x20*(type-KEYPAD_ENG);
								break;
				case 6:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'u'-0x20*(type-KEYPAD_ENG);
								break;
				case 7:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'i'-0x20*(type-KEYPAD_ENG);
								break;
				case 8:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'o'-0x20*(type-KEYPAD_ENG);
								break;
				case 9:	keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'p'-0x20*(type-KEYPAD_ENG);
								break;
				case 10:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'a'-0x20*(type-KEYPAD_ENG);
								break;
				case 11:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 's'-0x20*(type-KEYPAD_ENG);
								break;
				case 12:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'd'-0x20*(type-KEYPAD_ENG);
								break;
				case 13:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'f'-0x20*(type-KEYPAD_ENG);
								break;
				case 14:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'g'-0x20*(type-KEYPAD_ENG);
								break;
				case 15:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'h'-0x20*(type-KEYPAD_ENG);
								break;
				case 16:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'j'-0x20*(type-KEYPAD_ENG);
								break;
				case 17:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'k'-0x20*(type-KEYPAD_ENG);
								break;
				case 18:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'l'-0x20*(type-KEYPAD_ENG);
								break;
				case 19:keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'C';
								keypad->Key[i].mess0[1] = keypad->Key[i].mess1[1] = 'A';
								keypad->Key[i].T_key = KEY_CAPS;
								if( type==KEYPAD_ENGCAPS){
									keypad->Key[i].ReleaseBackColor = KEYSecondBackColor;
								}
								break;
				case 20:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'z'-0x20*(type-KEYPAD_ENG);
								break;
				case 21:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'x'-0x20*(type-KEYPAD_ENG);
								break;
				case 22:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'c'-0x20*(type-KEYPAD_ENG);
								break;
				case 23:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'v'-0x20*(type-KEYPAD_ENG);
								break;
				case 24:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'b'-0x20*(type-KEYPAD_ENG);
								break;
				case 25:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'n'-0x20*(type-KEYPAD_ENG);
								break;
				case 26:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = 'm'-0x20*(type-KEYPAD_ENG);
								break;
				case 27:keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = '<';
								keypad->Key[i].mess0[1] = keypad->Key[i].mess1[1] = '-';
								keypad->Key[i].T_key = KEY_BACKSPACE;
								break;
				case 28:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				case 29:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				case 30:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				case 31:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				case 32:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				case 33:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				case 34:keypad->Key[i].T_key = keypad->Key[i].mess0[0] = keypad->Key[i].mess1[0] = ' ';
								break;
				default:break;
			}
		}
	}
	// 符号键盘（未完成）
	else if( type==KEYPAD_SYMBOL )
	{
	}
	else if(type==DIY_KEYPAD)
	{
		for( i=0; i<keynum; i++ )
		{
			keypad->Key[i].T_sta = 0;
			
			keypad->Key[i].width = (uint16_t)w;
			keypad->Key[i].height = (uint16_t)h;
			// 计算按键位置（垂直紧密排列，无间隔）
        keypad->Key[i].sx = (uint16_t)x0+dw;           // 左侧对齐
        keypad->Key[i].sy = (uint16_t)(y0 + h * i+(i+1)*dh); // 垂直紧密排列
			
			keypad->Key[i].ReleaseBackColor = KEYReleaseBackColor;
			keypad->Key[i].ReleasePointColor = KEYReleasePointColor;
			keypad->Key[i].PressBackColor = KEYPressBackColor;
			keypad->Key[i].PressPointColor = KEYPressPointColor;
		
//			// 清空显示内容
//        memset(keypad->Key[i].mess0, 0, 10);
//        memset(keypad->Key[i].mess1, 0, 10);
        
        // 设置每个按键的显示内容和键值
        switch(i)
        {
            case 0:  // 第一个按键
                strcpy((char*)keypad->Key[i].mess0, "AD7606");//释
                strcpy((char*)keypad->Key[i].mess1, "AD7606");//按下
                keypad->Key[i].T_key = 1;
                break;
            case 1:  // 第二个按键
                strcpy((char*)keypad->Key[i].mess0, "dds");
                strcpy((char*)keypad->Key[i].mess1, "dds");
                keypad->Key[i].T_key = 2;
                break;
            case 2:  // 第三个按键
                strcpy((char*)keypad->Key[i].mess0, "左");
                strcpy((char*)keypad->Key[i].mess1, "左");
                keypad->Key[i].T_key = 3;
                break;
            case 3:  // 第四个按键
                strcpy((char*)keypad->Key[i].mess0, "右");
                strcpy((char*)keypad->Key[i].mess1, "右");
                keypad->Key[i].T_key = 4;
                break;
        }
    }
		
	}
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: Keypad_Seek
*	功能说明: 键盘搜索, 由触点坐标搜索是否有效按键, 如有则返回按键编号
*	参    数: *keypad:		键盘指针
*						(T_x,T_y):	触点坐标
*	返 回 值: 255 未找到 其他 按键编号
*********************************************************************************************************
*/
uint8_t	Keypad_Seek( KeypadTypeDef *keypad, uint16_t T_x, uint16_t T_y )
{
	uint8_t i,keynum;
	
	if( keypad->Type==KEYPAD_PHONE )			keynum = 12;
	else if( keypad->Type==KEYPAD_CALC )	keynum = 16;
	else if( keypad->Type==KEYPAD_ENG || keypad->Type==KEYPAD_ENGCAPS )	keynum = 35;
	else if( keypad->Type==KEYPAD_SYMBOL )	keynum = 35;	// 未完成,待定
	else if( keypad->Type==DIY_KEYPAD )	keynum = 4;	//
	for( i=0;i<keynum;i++ )
	{
		if( T_x>=keypad->Key[i].sx && T_x<keypad->Key[i].sx+keypad->Key[i].width && T_y>=keypad->Key[i].sy && T_y<keypad->Key[i].sy+keypad->Key[i].height )
		{
			break;
		}
	}
	
	if( i==keynum ) i=255;
	
	return i;
}

/*
*********************************************************************************************************
*	函 数 名: TouchKey_Draw
*	功能说明: 绘制触摸按键
*	参    数: T_key 按键结构体
*	返 回 值: 无
*********************************************************************************************************
*/
void TouchKey_Draw( TouchkeyTypeDef T_key )
{
	uint8_t	 *pStr;
	uint8_t  len,fsize;
	uint16_t tmpBKColor,tmpPColor,x,y;
	
	// 根据按键大小确定字符尺寸
	switch( TOUCHKEY_SIZE )
	{
		case 1:	fsize = 12;	break;
		case 2:	fsize = 16;	break;
		case 3:	fsize = 24;	break;
		case 4:	fsize = 32;	break;
		case 5:	fsize = 48;	break;
		default:	break;
	}
	
	if( T_key.T_sta==0 )	LCD_Fill( T_key.sx, T_key.sy, T_key.sx+T_key.width-1, T_key.sy+T_key.height-1, T_key.ReleaseBackColor );
	else									LCD_Fill( T_key.sx, T_key.sy, T_key.sx+T_key.width-1, T_key.sy+T_key.height-1, T_key.PressBackColor );
		
	tmpBKColor = lcddev.BACK_COLOR;
	tmpPColor  = lcddev.POINT_COLOR;
	if( T_key.T_sta==0 )			{ pStr = T_key.mess0; lcddev.BACK_COLOR = T_key.ReleaseBackColor; lcddev.POINT_COLOR = T_key.ReleasePointColor; }
	else if( T_key.T_sta==1 )	{ pStr = T_key.mess1; lcddev.BACK_COLOR = T_key.PressBackColor; lcddev.POINT_COLOR = T_key.PressPointColor; }
	else if( T_key.T_sta==2 )	{ pStr = T_key.mess1; lcddev.BACK_COLOR = T_key.PressBackColor; lcddev.POINT_COLOR = T_key.PressPointColor; }
	len = strlen((char*)pStr);
	
	x = T_key.sx + (T_key.width-fsize/2*len)/2;
	y = T_key.sy + (T_key.height-fsize)/2;
	
	LCD_Show_Str( x, y, fsize/2*len, fsize, pStr, fsize, 0 ,BLACK );
	lcddev.BACK_COLOR  = tmpBKColor;
	lcddev.POINT_COLOR = tmpPColor;
}


/*
*********************************************************************************************************
*	函 数 名: Keypad_Draw
*	功能说明: 绘制键盘图形
*	参    数: *keypad 键盘指针
*	返 回 值: 无
*********************************************************************************************************
*/
void Keypad_Draw( KeypadTypeDef *keypad )
{
	uint8_t i,keynum;
	
	if( keypad==NULL )	return;
	
	if( keypad->Type==KEYPAD_PHONE )			keynum = 12;
	else if( keypad->Type==KEYPAD_CALC )	keynum = 16;
	else if( keypad->Type==KEYPAD_ENG || keypad->Type==KEYPAD_ENGCAPS )	keynum = 35;
	else if( keypad->Type==KEYPAD_SYMBOL )	keynum = 35;	// 未完成,待定
	else if( keypad->Type==DIY_KEYPAD)    keynum = 4;
	
	LCD_Fill( keypad->sx, keypad->sy, keypad->sx + keypad->width - 1, keypad->sy + keypad->height - 1, KeypadBackColor );
	LCD_DrawRectangle( keypad->sx, keypad->sy, keypad->sx + keypad->width - 1, keypad->sy + keypad->height - 1, KeypadEdgeColor );
	
	for( i=0;i<keynum;i++ )	TouchKey_Draw( keypad->Key[i] );
}

/*
*********************************************************************************************************
*	函 数 名: Keypad_Clear
*	功能说明: 清除键盘图形
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Keypad_Clear( KeypadTypeDef *keypad )
{
	if( keypad==NULL )	return;
	
	LCD_Fill( keypad->sx, keypad->sy, keypad->sx + keypad->width - 1, keypad->sy + keypad->height - 1, lcddev.BACK_COLOR );
}

/*
*********************************************************************************************************
*	函 数 名: TouchKeypad_Select
*	功能说明: 选择键盘
*	参    数: keypadsel: 选择的当前键盘, 定义见 TouchKeypad.h
*	返 回 值: 无
*********************************************************************************************************
*/
void TouchKeypad_Select( KeypadType keypadsel )
{
	switch(keypadsel)
	{
		case KEYPAD_NONE:
							Keypad_Clear( CurrentKeypad );
							CurrentKeypad = NULL;
							break;
		case KEYPAD_PHONE:
							CurrentKeypad = &KeypadPhone;
							Keypad_Draw( CurrentKeypad );
							break;
		case KEYPAD_CALC:
							CurrentKeypad = &KeypadCalc;
							Keypad_Draw( CurrentKeypad );
							break;
		case KEYPAD_ENG:
							CurrentKeypad = &KeypadEng;
							Keypad_Draw( CurrentKeypad );
							break;
		case KEYPAD_ENGCAPS:
							CurrentKeypad = &KeypadEngCAP;
							Keypad_Draw( CurrentKeypad );
							break;
		case KEYPAD_SYMBOL:
							CurrentKeypad = &KeypadSymbol;
							Keypad_Draw( CurrentKeypad );
							break;
		case DIY_KEYPAD:
							CurrentKeypad = &DIY_Keypad;
							Keypad_Draw( CurrentKeypad );
							break;
		
		
		default:	break;
	}
}


/*
*********************************************************************************************************
*	函 数 名: TouchKeypad_Init
*	功能说明: 触摸键盘初始化
*						配置所有键盘, 设置当前键盘为空
*	参    数: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TouchKeypad_Init( void )
{	
	Keypad_Init( &KeypadPhone, KEYPAD_PHONE, 0, 160, 240, 160 );
	Keypad_Init( &KeypadCalc, KEYPAD_CALC, 0, 160, 240, 160 );
	Keypad_Init( &KeypadEng, KEYPAD_ENG, 0, 160, 240, 160 );
	Keypad_Init( &KeypadEngCAP, KEYPAD_ENGCAPS, 0, 160, 240, 160 );
	Keypad_Init( &DIY_Keypad, DIY_KEYPAD, 220, 0, 100, 240 );
	TouchKeypad_Select( KEYPAD_NONE );//KEYPAD_NONE
}



/*
*********************************************************************************************************
*	函 数 名: Touch_Action
*	功能说明: 触摸事件处理, 搜索当前键盘, 如果是有效按键则执行键动作, 并发送按键消息
*						本函数应在触屏扫描任务函数中被调用
*	参    数: T_msg: 			触摸事件类型, 见 msg.h 
*						(T_x,T_y):	触点坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_Action( MSG_CODE_X T_msg, uint16_t T_x, uint16_t T_y )
{
	uint8_t 	id;
	
	// 非触摸事件, 退出
	if( T_msg!=MSG_TOUCH_RELEASE && T_msg!=MSG_TOUCH_PRESS && T_msg!=MSG_TOUCH_LONGHOLD )	return;
	// 当前无键盘, 退出
	if( CurrentKeypad==NULL )	return;
	
	
	id = Keypad_Seek( CurrentKeypad, T_x, T_y );
	if( id==255 )	return;	
	
	if( T_msg==MSG_TOUCH_RELEASE )				CurrentKeypad->Key[id].T_sta = 0;
	else if( T_msg==MSG_TOUCH_PRESS )			CurrentKeypad->Key[id].T_sta = 1;
	else if( T_msg==MSG_TOUCH_LONGHOLD )	CurrentKeypad->Key[id].T_sta = 2;
	
	TouchKey_Draw( CurrentKeypad->Key[id] );
	
	vPutMsg( T_msg, CurrentKeypad->Key[id].T_key );
}

