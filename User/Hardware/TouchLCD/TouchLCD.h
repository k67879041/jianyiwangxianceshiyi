#ifndef __TOUCHLCD_H
#define __TOUCHLCD_H

#include "sys.h"
#include "LCD_Hard.h"
#include "Function.h"

#define		LCD_XPT2046_ENABLE		1    // 是否使用触摸屏

#if LCD_XPT2046_ENABLE
	#include "xpt2046.h"
#endif

/********************* 颜色定义 ************************/
#define WHITE					0xFFFF	// 白色
#define BLACK					0x0000	// 黑色
#define BLUE					0x001F  // 蓝色
#define BRED					0xF81F	// 
#define GRED					0xFFE0	// 
#define GBLUE					0x07FF	// 
#define RED						0xF800	// 
#define MAGENTA				0xF81F	// 
#define GREEN					0x07E0	// 
#define CYAN					0x7FFF	// 
#define YELLOW				0xFFE0	// 
#define BROWN					0xBC40	// 棕色
#define BRRED					0xFC07	// 棕红色
#define GRAY					0x8430	// 灰色
//GUI颜色
#define DARKBLUE			0x01CF	// 深蓝色
#define LIGHTBLUE			0x7D7C	// 浅蓝色  
#define GRAYBLUE			0x5458	// 灰蓝色
#define LIGHTGREEN		0x841F	// 浅绿色
#define LIGHTGRAY			0xEF5B	// 浅灰色(PANNEL)
#define LGRAY					0xC618	// 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE			0xA651	// 浅灰蓝色(中间层颜色)
#define LBBLUE				0x2B12	// 浅棕蓝色(选择条目的反色)
/****************** end of 颜色定义 ********************/


void LCD_Clear(u16 color);
void LCD_Clear_text(u16 sx,u16 sy,u16 ex,u16 ey);//清屏指定区域为背景色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color );
void LCD_DrawDashedLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color, u16 dash, u16 solid);
void LCD_DrawRectangle( u16 x1, u16 y1, u16 x2, u16 y2, u16 color );
void LCD_DrawCircle( u16 x0, u16 y0, u8 r, u16 color );
void LCD_DrawFillCircle(u16 x0, u16 y0, u8 r, u16 color);
void LCD_DrawCross ( uint16_t usX, uint16_t usY, u16 color );
void LCD_ChangeBackArea(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);


void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_xnum_all(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color, uint16_t black_color);
void LCD_ShowChar(u16 x,u16 y,u8 ch,u8 size,u8 mode,u16 color);
void LCD_ShowChar_all(u16 x,u16 y,u8 ch,u8 size,u8 mode,u16 color,u16 black_color);
void LCD_ShowCharact( uint16_t x, uint16_t y, uint8_t *font, uint8_t size, u8 mode,u16 color );
void LCD_ShowCharact_all( uint16_t x, uint16_t y, uint8_t *font, uint8_t size, u8 mode,u16 color,u16 back_color );
void LCD_Show_Str(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 size, u8 mode,u16 color);
void LCD_Show_Str_all(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 size, u8 mode,u16 color,u16 back_color);

void LCD_Show1bitPic( u16 x, u16 y, const unsigned char *gImage,u16 color );
void LCD_ShowPic(u16 x, u16 y, const unsigned char *gImage,u16 color_1,u16 color_black_1);
void LCD_ShowPicFromFlash(u16 x, u16 y, uint32_t flashAddr);

void LCD_ShowPicFromFlash_all(u16 x, u16 y, uint32_t flashAddr, unsigned char need_rotate);
void LCD_ClearNonBackground_Fast_WithRotate(u16 sx, u16 sy, u16 ex, u16 ey, unsigned char need_rotate);




// 绘制模式枚举
typedef enum {
    DRAW_POINT = 0,  // 画点模式
    DRAW_LINE = 1    // 画线模式
} DrawMode;


uint32_t DrawWaveformGrid(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, 
                     int32_t *xTicks, int32_t *yTicks, uint8_t xTickCount, uint8_t yTickCount, 
                     char *xLabel, char *yLabel, uint16_t originX, uint16_t originY,
                     uint16_t gridColor, uint16_t axisColor, uint16_t tickTextColor, uint16_t labelTextColor);//要返回原点坐标因为它会智能调整，x高位，y低位
void DrawAdWaveform(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                   int16_t *adData, uint16_t dataLen,  // 修改为int16_t类型
                   uint16_t originX, uint16_t originY,
                   int16_t yMin, int16_t yMax,          // 修改为int16_t类型
                   DrawMode mode, uint16_t waveColor);
									
//	back_value_1=DrawWaveformGrid(0,0,300,240,xValues,yValues,10,8,"f/mhz","dB",0,240,LIGHTGRAY,BLUE,BLACK,RED);//按x轴刻度，扫频步进为 150,000hz
//	DrawAdWaveform((uint16_t )(back_value_1>>16),0,300,(uint16_t )back_value_1,DrawWave_buf,266,(uint16_t )(back_value_1>>16),(uint16_t )back_value_1,0,4095,0,RED);									
									
void LCD_ShowProgressBar(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                        uint8_t percent, uint16_t bgColor, uint16_t barColor,
                        uint8_t showText, uint16_t textColor, uint8_t fontSize);									
void TouchLCD_Init( uint8_t lcddir );
#if LCD_XPT2046_ENABLE
	uint8_t TouchLCD_Calibrate( void );
#endif


#endif  
	 
	 



