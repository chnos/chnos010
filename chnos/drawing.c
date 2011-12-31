
#include "core.h"

void (*Drawing_Fill_Rectangle)(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void (*Drawing_Put_String)(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
void (*Drawing_Draw_Point)(void *vram, uint xsize, uint x, uint y, uint c);
void (*Drawing_Draw_Line_PQ)(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
//Drawingに含まれる低レベル描画関数は全て、座標は符号なし整数であり、vramの左上の座標を原点(0, 0)として、xは右方向、yは下方向に増加する。
//また、二点の座標をとる関数は、（基本的に）全て引数左側がx成分の小さい側（原点に近い）でなければならない。
//高レベル描画関数では、それらを上手くラップすべきである。

void Initialise_Drawing(void)
{
	IO_DisplayControl *dispctrl;

	dispctrl = System_Display_Get_Controller();

	if(dispctrl->bpp == 8){
		Drawing_Fill_Rectangle	= Drawing08_Fill_Rectangle;
		Drawing_Put_String	= Drawing08_Put_String;
		Drawing_Draw_Point	= Drawing08_Draw_Point;
		Drawing_Draw_Line_PQ	= Drawing08_Draw_Line_PQ;
		Drawing08_Initialise_Palette();
	} else if(dispctrl->bpp == 16){
		Drawing_Fill_Rectangle	= Drawing16_Fill_Rectangle;
		Drawing_Put_String	= Drawing16_Put_String;
		Drawing_Draw_Point	= Drawing16_Draw_Point;
		Drawing_Draw_Line_PQ	= Drawing16_Draw_Line_PQ;
	} else if(dispctrl->bpp == 32){
		Drawing_Fill_Rectangle	= Drawing32_Fill_Rectangle;
		Drawing_Put_String	= Drawing32_Put_String;
		Drawing_Draw_Point	= Drawing32_Draw_Point;
		Drawing_Draw_Line_PQ	= Drawing32_Draw_Line_PQ;
	} else{
		Drawing_Fill_Rectangle	= Drawing_Invalid_Fill_Rectangle;
		Drawing_Put_String	= Drawing_Invalid_Put_String;
		Drawing_Draw_Point	= Drawing_Invalid_Draw_Point;
		Drawing_Draw_Line_PQ	= Drawing_Invalid_Draw_Line_PQ;
		#ifdef CHNOSPROJECT_DEBUG_DRAWING
			debug("Initalise_Drawing:Not implemented %d bpp.\n", dispctrl->bpp);
		#endif
	}
	return;
}

void Drawing_Invalid_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1)
{
	#ifdef CHNOSPROJECT_DEBUG_DRAWING
		debug("Drawing_Invalid_Fill_Rectangle:[0x%X] xsize:%d color:0x%X\nDrawing_Invalid_Fill_Rectangle: (%d, %d) -> (%d, %d)\n", vram, xsize, c, x0, y0, x1, y1);
	#endif
	return;
}

void Drawing_Invalid_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s)
{
	#ifdef CHNOSPROJECT_DEBUG_DRAWING
		debug("Drawing_Invalid_Put_String:[0x%X] xsize:%d (%d, %d) color:0x%X\nDrawing_Invalid_Put_String:>%s\n", vram, xsize, x, y, c, s);
	#endif
	return;
}

void Drawing_Invalid_Draw_Point(void *vram, uint xsize, uint x, uint y, uint c)
{
	#ifdef CHNOSPROJECT_DEBUG_DRAWING
		debug("Drawing_Invalid_Draw_Point:[0x%X] xsize:%d (%d, %d) color:0x%X\n", vram, xsize, x, y, c);
	#endif
	return;
}

void Drawing_Invalid_Draw_Line_PQ(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1)
{
	#ifdef CHNOSPROJECT_DEBUG_DRAWING
		debug("Drawing_Invalid_Draw_Line_PQ:[0x%X] xsize:%d color:0x%X\nDrawing_Invalid_Draw_Line_PQ: (%d, %d) -> (%d, %d)\n", vram, xsize, c, x0, y0, x1, y1);
	#endif
	return;
}

uchar RGB_32_To_08(uint c32)
{
	uchar c8;
	uchar c[4];

	c[3] = (c32 << 24) >> 24;
	c[2] = (c32 << 16) >> 24;
	c[1] = (c32 <<  8) >> 24;
	c[0] = (c32 >> 24);

	c[1] = c[1] / 51;
	c[2] = c[2] / 51;
	c[3] = c[3] / 51;

	c8 = (c[1] + c[2] * 6 + c[3] * 36) + 16;

	return c8;
}

uchar RGB_32_To_08_xy(uint c32, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2 };
	uchar c8;
	uchar c[4];
	int i;

	c[3] = (c32 << 24) >> 24;
	c[2] = (c32 << 16) >> 24;
	c[1] = (c32 <<  8) >> 24;
	c[0] = (c32 >> 24);

	x &= 1; /* 偶数か奇数か */
	y &= 1;

	i = table[x + y * 2];	/* 中間色を作るための定数 */
	c[1] = (c[1] * 21) / 256;	/* これで 0～20 になる */
	c[2] = (c[2] * 21) / 256;
	c[3] = (c[3] * 21) / 256;
	c[1] = (c[1] + i) / 4;	/* これで 0～5 になる */
	c[2] = (c[2] + i) / 4;
	c[3] = (c[3] + i) / 4;

	c8 = 16 + c[1] + c[2] * 6 + c[3] * 36;

	return c8;
}

ushort RGB_32_To_16(uint c32)
{
	ushort c16;
	uchar c[4];

	c[3] = (c32 << 24) >> 24;
	c[2] = (c32 << 16) >> 24;
	c[1] = (c32 <<  8) >> 24;
	c[0] = (c32 >> 24);

	c16 = ((c[1] >> 3) << 11 | (c[2] >> 2) << 5 | (c[3] >> 3));
	
	return c16;
}

