
#include "core.h"

uchar VGA_CRTController_ReadRegister(uchar regno)
{
	IO_Out8(VGA_CRTC_R_NUMBER, regno);
	return IO_In8(VGA_CRTC_R_DATA);
}

void VGA_CRTController_WriteRegister(uchar regno, uchar data)
{
	IO_Out8(VGA_CRTC_R_NUMBER, regno);
	IO_Out8(VGA_CRTC_R_DATA, data);
	return;
}

void TextMode_Write_TextRAM(ushort index, uchar data)
{
	uchar *textram;

	textram = (uchar *)VGA_TEXTMODE_ADR;

	if(index < 80 * 25 * 2){
		textram[index] = data;
	}
	return;
}

void TextMode_Put_Character_Absolute(uchar c, col_text col, ushort location)
{
	TextMode_Write_TextRAM(location * 2 + 0, c);
	TextMode_Write_TextRAM(location * 2 + 1, (uchar)col);
	return;
}

void TextMode_Put_String_Absolute(const uchar s[], col_text col, uint x, uint y)
{
	ushort location;
	uint i;

	for(i = 0; s[i] != 0x00; i++){

	}

	if((x + i - 1) >= 80 || y >= 25){
		return;
	}

	location = (y * 80) + x;

	for(i = 0; s[i] != 0x00; i++){
		TextMode_Put_Character_Absolute(s[i], col, location + i);
	}
	return;
}

void TextMode_Clear_Screen(void)
{
	uint i;

	for(i = 0; i < 80 * 25; i++){
		TextMode_Put_Character_Absolute(' ', white, i);
	}

	TextMode_Set_CursorLocation(0);

	return;
}

ushort TextMode_Get_CursorLocation(void)
{
	ushort location;

	location = VGA_CRTController_ReadRegister(VGA_CRTC_R_CURSOR_LOCATION_HIGH);

	location = location << 8;

	location |= VGA_CRTController_ReadRegister(VGA_CRTC_R_CURSOR_LOCATION_LOW);

	return location;
}

void TextMode_Set_CursorLocation(ushort location)
{
	VGA_CRTController_WriteRegister(VGA_CRTC_R_CURSOR_LOCATION_HIGH, location >> 8);
	VGA_CRTController_WriteRegister(VGA_CRTC_R_CURSOR_LOCATION_LOW, location & 0x00ff);

	return;
}

void TextMode_Put_Character(uchar c, col_text col)
{
	ushort location;

	location = TextMode_Get_CursorLocation();
	if(c == '\n'){
		if(80 * 24 <= location && location < 80 * 25){
			TextMode_Newline();
			TextMode_Set_CursorLocation(24 * 80);
		} else{
			TextMode_Set_CursorLocation(((location / 80) + 1) * 80);
		}
	} else if(c == '\t'){
		TextMode_Set_CursorLocation(((location >> 2) << 2) + 4);
	} else{
		TextMode_Put_Character_Absolute(c, col, location);
		TextMode_Set_CursorLocation(location + 1);
	}
	return;
}

void TextMode_Newline(void)
{
	uint *textram;
	uint i;

	textram = (uint *)VGA_TEXTMODE_ADR;

	for(i = 0; i < 80 * 24 * 2 / 4; i++){
		textram[i] = textram[i + 80 * 2 / 4];
	}

	for(i = 0; i < 80; i++){
		TextMode_Put_Character_Absolute(' ', white, (80 * 24) + i);
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
