
#include "core.h"

void Drawing32_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1)
{
	uint x, y;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			((uint *)vram)[y * xsize + x] = c;
		}
	}
	return;
}

void Drawing32_Put_Font(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font)
{
	int i;
	uchar d;
	uint *p;
	for (i = 0; i < 16; i++) {
		p = (uint *)vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = (uint)c; }
		if ((d & 0x40) != 0) { p[1] = (uint)c; }
		if ((d & 0x20) != 0) { p[2] = (uint)c; }
		if ((d & 0x10) != 0) { p[3] = (uint)c; }
		if ((d & 0x08) != 0) { p[4] = (uint)c; }
		if ((d & 0x04) != 0) { p[5] = (uint)c; }
		if ((d & 0x02) != 0) { p[6] = (uint)c; }
		if ((d & 0x01) != 0) { p[7] = (uint)c; }
	}
	return;
}

void Drawing32_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar s[])
{
	for(; *s != 0x00; s++){
		if(x > xsize - 8){
			break;
		}
		Drawing32_Put_Font(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}
