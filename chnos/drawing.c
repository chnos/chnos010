
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
	c = RGB_32_To_08(c);
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			((uchar *)vram)[y * xsize + x] = (uchar)c;
		}
	}
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

	x &= 1; /* ãÙêîÇ©äÔêîÇ© */
	y &= 1;

	i = table[x + y * 2];	/* íÜä‘êFÇçÏÇÈÇΩÇﬂÇÃíËêî */
	c[1] = (c[1] * 21) / 256;	/* Ç±ÇÍÇ≈ 0Å`20 Ç…Ç»ÇÈ */
	c[2] = (c[2] * 21) / 256;
	c[3] = (c[3] * 21) / 256;
	c[1] = (c[1] + i) / 4;	/* Ç±ÇÍÇ≈ 0Å`5 Ç…Ç»ÇÈ */
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
