#include "string.h"
#include "UI.h"




void UI_Draw_GridWindow( u16 x0, u16 y0, u16 width, u16 height, u8 col, u8 row )
{
	u16 i,cellwidth, cellheight;
	
	if( x0+width>lcddev.width ) return;
	if( y0+height>lcddev.height ) return;
	
	cellwidth = width/col;
	cellheight = height/row;
	
	for( i=0;i<row;i++ )	LCD_DrawLine( x0, y0+i*cellheight, x0+width-1, y0+i*cellheight, LGRAY );
	for( i=0;i<col;i++ )	LCD_DrawLine( x0+i*cellwidth, y0, x0+i*cellwidth, y0+height, LGRAY );
	LCD_DrawRectangle( x0, y0, x0+width-1, y0+height-1, lcddev.POINT_COLOR );
}



void UI1( void )
{

//	UI_Draw_Keypad( &KeypadPhone );
//	UI_Draw_Keypad( &KeypadCalc );
//	UI_Draw_Keypad( &KeypadEng );
	
//	UI_Draw_GridWindow(0,0,640,480,10,8);
//	LCD_DrawLine( 660,  42, 799,  42, BLACK );
//	LCD_DrawLine( 660, 121, 799, 121, BLACK );
//	LCD_DrawLine( 660, 200, 799, 200, BLACK );
//	LCD_DrawLine( 660, 279, 799, 279, BLACK );
//	LCD_DrawLine( 660, 358, 799, 358, BLACK );
//	LCD_DrawLine( 660, 437, 799, 437, BLACK );
//	for( i=0;i<NumofTKey;i++ )	TouchKey_Draw( TouchKey[i] );
}









