
#include "core.h"

void TextMode_Put_Character(uchar c, col_text col)
{
	uchar *textram;
	ushort location;

	textram = (uchar *)VGA_TEXTMODE_ADR;
	location = TextMode_Get_CursorLocation();
	if(c == '\n'){
		TextMode_Set_CursorLocation(((location + 79) / 80) * 80);
	} else if(c == '\t'){
		TextMode_Set_CursorLocation(((location >> 2) << 2) + 4);
	} else{
		textram[location * 2 + 0] = c;
		textram[location * 2 + 1] = (uchar)col;
		TextMode_Set_CursorLocation(location + 1);
	}
	return;
}

void TextMode_Put_String(const uchar s[], col_text col)
{
	for(; *s != 0x00; s++){
		TextMode_Put_Character(*s, col);
	}
	return;
}

void TextMode_Clear_Screen(void)
{
	uint i;

	TextMode_Set_CursorLocation(0);

	for(i = 0; i < 80 * 25; i ++){
		TextMode_Put_Character(' ', white);
	}

	TextMode_Set_CursorLocation(0);

	return;
}

ushort TextMode_Get_CursorLocation(void)
{
	uint eflags;
	ushort location;

	eflags = IO_Load_EFlags();

	IO_Out8(VGA_CRTC_R_NUMBER, VGA_CRTC_R_CURSOR_LOCATION_HIGH);
	location = IO_In8(VGA_CRTC_R_DATA);

	location = location << 8;

	IO_Out8(VGA_CRTC_R_NUMBER, VGA_CRTC_R_CURSOR_LOCATION_LOW);
	location |= IO_In8(VGA_CRTC_R_DATA);

	IO_Store_EFlags(eflags);
	return location;
}

void TextMode_Set_CursorLocation(ushort location)
{
	uint eflags;

	eflags = IO_Load_EFlags();

	IO_Out8(VGA_CRTC_R_NUMBER, VGA_CRTC_R_CURSOR_LOCATION_HIGH);
	IO_Out8(VGA_CRTC_R_DATA, location >> 8);

	IO_Out8(VGA_CRTC_R_NUMBER, VGA_CRTC_R_CURSOR_LOCATION_LOW);
	IO_Out8(VGA_CRTC_R_DATA, location & 0x00ff);

	IO_Store_EFlags(eflags);
	return;
}
