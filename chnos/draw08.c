
#include "core.h"

void Drawing08_Initialise_Palette(void)
{
	static uchar table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,
		0xff, 0x00, 0x00,
		0x00, 0xff, 0x00,
		0xff, 0xff, 0x00,
		0x00, 0x00, 0xff,
		0xff, 0x00, 0xff,
		0x00, 0xff, 0xff,
		0xff, 0xff, 0xff,
		0xc6, 0xc6, 0xc6,
		0x84, 0x00, 0x00,
		0x00, 0x84, 0x00,
		0x84, 0x84, 0x00,
		0x00, 0x00, 0x84,
		0x84, 0x00, 0x84,
		0x00, 0x84, 0x84,
		0x84, 0x84, 0x84
	};
	uchar table2[216 * 3];
	uint r, g, b;

	Drawing08_Set_Palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	Drawing08_Set_Palette(16, 231, table2);

	return;
}

void Drawing08_Set_Palette(uint start, uint end, uchar *rgb)
{
	uint i, eflags;

	eflags = IO_Load_EFlags();
	IO_CLI(); 
	IO_Out8(VGA_VDAC_WRITE_ADR, start);
	for (i = start; i <= end; i++) {
		IO_Out8(VGA_VDAC_DATA, rgb[0] >> 2);
		IO_Out8(VGA_VDAC_DATA, rgb[1] >> 2);
		IO_Out8(VGA_VDAC_DATA, rgb[2] >> 2);
		rgb += 3;
	}
	IO_Store_EFlags(eflags);
	return;
}

void Drawing08_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1)
{
	uint x, y;

//if negative location
	if((x0 & 0x80000000) != 0 || (y0 & 0x80000000) != 0 || (x1 & 0x80000000) != 0 || (y1 & 0x80000000) != 0){
		return;
	}

	c = RGB_32_To_08(c);
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			((uchar *)vram)[y * xsize + x] = (uchar)c;
		}
	}
	return;
}

void Drawing08_Put_Font(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font)
{
	int i;
	uchar d;
	uchar *p;

//if negative location
	if((x & 0x80000000) != 0 || (y & 0x80000000) != 0){
		return;
	}

	for (i = 0; i < 16; i++) {
		p = (uchar *)(vram + (y + i) * xsize + x);
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = (uchar)c; }
		if ((d & 0x40) != 0) { p[1] = (uchar)c; }
		if ((d & 0x20) != 0) { p[2] = (uchar)c; }
		if ((d & 0x10) != 0) { p[3] = (uchar)c; }
		if ((d & 0x08) != 0) { p[4] = (uchar)c; }
		if ((d & 0x04) != 0) { p[5] = (uchar)c; }
		if ((d & 0x02) != 0) { p[6] = (uchar)c; }
		if ((d & 0x01) != 0) { p[7] = (uchar)c; }
	}
	return;
}

void Drawing08_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar s[])
{
//if negative location
	if((x & 0x80000000) != 0 || (y & 0x80000000) != 0){
		return;
	}

	c = RGB_32_To_08(c);
	for(; *s != 0x00; s++){
		if(x > xsize - 8){
			break;
		}
		Drawing08_Put_Font(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void Drawing08_Draw_Point(void *vram, uint xsize, uint x, uint y, uint c)
{
//if negative location
	if((x & 0x80000000) != 0 || (y & 0x80000000) != 0){
		return;
	}

	((uchar *)vram)[y * xsize + x] = RGB_32_To_08_xy(c, x, y);
	return;
}
