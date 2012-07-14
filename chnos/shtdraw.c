
#include "core.h"

uint Sheet_Drawing_Fill_Rectangle(UI_Sheet *sheet, uint c, int px0, int py0, int px1, int py1)
{
	if(sheet == Null){
		return 1;
	}

	if(sheet->vram == Null){
		return 2;
	}

	return sheet->Drawing.Fill_Rectangle(sheet, c, px0, py0, px1, py1);
}

uint Sheet_Drawing_Put_String(UI_Sheet *sheet, int x, int y, uint fc, const uchar s[])
{
	if(sheet == Null){
		return 1;
	}

	if(sheet->vram == Null){
		return 2;
	}

	return sheet->Drawing.Put_String(sheet, x, y, fc, s);
}

uint Sheet_Drawing_Draw_Point(UI_Sheet *sheet, int x, int y, uint c)
{
	if(sheet == Null){
		return 1;
	}

	if(sheet->vram == Null){
		return 2;
	}

	return sheet->Drawing.Draw_Point(sheet, x, y, c);
}

uint Sheet_Drawing_Scroll_Vertical(UI_Sheet *sheet, int vpx)
{
	if(sheet == Null){
		return 1;
	}

	if(sheet->vram == Null){
		return 2;
	}

	if(vpx > 0){
		if((uint)vpx >= sheet->size.y){
			return 3;
		}
		CFunction_MemoryMove(sheet->vram, sheet->vramsize, (void *)((uint)sheet->vram + (sheet->size.x * vpx * (sheet->bpp >> 3))), sheet->vramsize - (sheet->size.x * vpx * (sheet->bpp >> 3)));
	} else if(vpx < 0){
		vpx = -vpx;
		if((uint)vpx >= sheet->size.y){
			return 4;
		}
		CFunction_MemoryMove((void *)((uint)sheet->vram + (sheet->size.x * vpx * (sheet->bpp >> 3))), sheet->vramsize - (sheet->size.x * vpx * (sheet->bpp >> 3)), sheet->vram, sheet->vramsize);
	}

	return 0;
}

//invalid
uint Sheet_Drawing_Fill_Rectangle_Invalid(UI_Sheet *sheet, uint c, int px0, int py0, int px1, int py1)
{
	return 0;
}

uint Sheet_Drawing_Put_String_Invalid(UI_Sheet *sheet, int x, int y, uint fc, const uchar s[])
{
	return 0;
}

uint Sheet_Drawing_Draw_Point_Invalid(UI_Sheet *sheet, int x, int y, uint c)
{
	return 0;
}


//8-bit
uint Sheet08_Drawing_Fill_Rectangle(UI_Sheet *sheet, uint c, int px0, int py0, int px1, int py1)
{
	Drawing08_Fill_Rectangle(sheet->vram, sheet->size.x, c, (uint)px0, (uint)py0, (uint)px1, (uint)py1);
	return 0;
}

uint Sheet08_Drawing_Put_String(UI_Sheet *sheet, int x, int y, uint fc, const uchar s[])
{
	Drawing08_Put_String(sheet->vram, sheet->size.x, (uint)x, (uint)y, fc, s);
	return 0;
}

uint Sheet08_Drawing_Draw_Point(UI_Sheet *sheet, int x, int y, uint c)
{
	Drawing08_Draw_Point(sheet->vram, sheet->size.x, (uint)x, (uint)y, c);
	return 0;
}

//16-bit
uint Sheet16_Drawing_Fill_Rectangle(UI_Sheet *sheet, uint c, int px0, int py0, int px1, int py1)
{
	Drawing16_Fill_Rectangle(sheet->vram, sheet->size.x, c, (uint)px0, (uint)py0, (uint)px1, (uint)py1);
	return 0;
}

uint Sheet16_Drawing_Put_String(UI_Sheet *sheet, int x, int y, uint fc, const uchar s[])
{
	Drawing16_Put_String(sheet->vram, sheet->size.x, (uint)x, (uint)y, fc, s);
	return 0;
}

uint Sheet16_Drawing_Draw_Point(UI_Sheet *sheet, int x, int y, uint c)
{
	Drawing16_Draw_Point(sheet->vram, sheet->size.x, (uint)x, (uint)y, c);
	return 0;
}

//32-bit
uint Sheet32_Drawing_Fill_Rectangle(UI_Sheet *sheet, uint c, int px0, int py0, int px1, int py1)
{
	Drawing32_Fill_Rectangle(sheet->vram, sheet->size.x, c, (uint)px0, (uint)py0, (uint)px1, (uint)py1);
	return 0;
}

uint Sheet32_Drawing_Put_String(UI_Sheet *sheet, int x, int y, uint fc, const uchar s[])
{
	Drawing32_Put_String(sheet->vram, sheet->size.x, (uint)x, (uint)y, fc, s);
	return 0;
}

uint Sheet32_Drawing_Draw_Point(UI_Sheet *sheet, int x, int y, uint c)
{
	Drawing32_Draw_Point(sheet->vram, sheet->size.x, (uint)x, (uint)y, c);
	return 0;
}
