
#include "core.h"

uint RGB_Table_08[16] = {
	0x00000000,	//黒
	0x00ff0000,	//赤
	0x0000ff00,	//緑
	0x00ffff00,	//黄
	0x000000ff,	//青
	0x0000ffff,	//水
	0x00ffffff,	//白
	0x00c6c6c6,	//明るい灰色
	0x00840000,	//暗い赤
	0x00008400,	//暗い緑
	0x00848400,	//暗い黄
	0x00000084,	//暗い青
	0x00840084,	//紫
	0x00008484,	//暗い水
	0x00848484	//暗い灰色
};

ushort RGB_08_To_16(uchar c8)
{
	RGB32 c32;

	if(c8 < 16){
		c32.c32 = RGB_Table_08[c8];
		return ((c32.bit.r / 7) << 11) | ((c32.bit.g / 7) << (5 + 1) | (c32.bit.b / 7));
	}

	if(c8 >= 16 + 216){
		return 0x0000;
	}

	c8 -= 16;

	c32.bit.b = c8 / 36;
	c8 -= c32.bit.b * 36;
	c32.bit.g = c8 / 6;
	c8 -= c32.bit.g * 6;
	c32.bit.r = c8;
	c32.bit.a = 0x00;

	return ((c32.bit.r * 6) << 11) | ((c32.bit.g * 6) << (5 + 1) | (c32.bit.b * 6));
}

uint RGB_08_To_32(uchar c8)
{
	RGB32 c32;

	if(c8 < 16){
		return RGB_Table_08[c8];
	}

	if(c8 >= 16 + 216){
		return 0x00000000;
	}

	c8 -= 16;

	c32.bit.b = c8 / 36;
	c8 -= c32.bit.b * 36;
	c32.bit.g = c8 / 6;
	c8 -= c32.bit.g * 6;
	c32.bit.r = c8;
	c32.bit.a = 0x00;

	c32.bit.r *= 51;
	c32.bit.g *= 51;
	c32.bit.b *= 51;

	return c32.c32;
}

uchar RGB_16_To_08(ushort c16)
{
	uchar c[3];

	c[0] = c16 >> (5 + 6);
	c[1] = (c16 & 0x7e0) >> 5;
	c[2] = c16 & 0x1f;

	c[0] /= 6;
	c[1] /= 12;
	c[2] /= 6;

	return 16 + c[0] + c[1] * 6 + c[2] * 36;
}

uint RGB_16_To_32(ushort c16)
{
	uchar c[3];

	c[0] = c16 >> (5 + 6);
	c[1] = (c16 & 0x7e0) >> 5;
	c[2] = c16 & 0x1f;

	return (c[0] << (16 + 3)) | (c[1] << (8 + 2)) | (c[2] << 3);
}

uchar RGB_32_To_08(uint c32)
{
	uchar c8;
	uchar c[4];

	c[3] = (c32 << 24) >> 24;
	c[2] = (c32 << 16) >> 24;
	c[1] = (c32 <<  8) >> 24;

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

	c16 = ((c[1] >> 3) << 11 | (c[2] >> 2) << 5 | (c[3] >> 3));
	
	return c16;
}

