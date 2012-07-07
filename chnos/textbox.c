
#include "core.h"

UI_TextBox *TextBox_Initialise(void)
{
	UI_TextBox *textbox;

	textbox = System_Memory_Allocate(sizeof(UI_TextBox));
	textbox->flags.bit.initialized = True;

	textbox->forecol = 0x000000;
	textbox->backcol = 0xffffff;

	return textbox;
}

//bpp==0:自動選択(現在のシステムシートの設定に合わせる)
uint TextBox_SetBuffer(UI_TextBox *textbox, uint xchars, uint ychars, uint bpp, UI_Sheet *parent)
{
	if(textbox == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_SetBuffer:Null textbox.");
			return 1;
		#endif
	}

	textbox->chars.x = xchars;
	textbox->chars.y = ychars;
	textbox->size_text_buf = textbox->chars.x * textbox->chars.y;

	textbox->sheet = Sheet_Initialise();
	Sheet_SetBuffer(textbox->sheet, Null, xchars << 3, ychars << 4, bpp);
	Sheet_SetParent(textbox->sheet, parent);

	textbox->sheet->Drawing.Fill_Rectangle(textbox->sheet, textbox->backcol, 0, 0, (int)textbox->sheet->size.x - 1, (int)textbox->sheet->size.y - 1);

	textbox->text_buf = System_Memory_Allocate(textbox->size_text_buf);
	textbox->text_buf[0] = Null;

	textbox->flags.bit.textbuffer_configured = True;

	return 0;
}

uint TextBox_Show(UI_TextBox *textbox, uint height, int px, int py)
{
	if(textbox == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_Show:Null textbox.");
			return 1;
		#endif
	}

	return Sheet_Show(textbox->sheet, height, px, py);
}

uint TextBox_Put_Character(UI_TextBox *textbox, uchar c)
{
	uchar s[2];

	s[0] = c;
	s[1] = 0x00;

	#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
		debug("TextBox_Put_Character:put [%c].\n", c);
	#endif

	textbox->sheet->Drawing.Put_String(textbox->sheet, textbox->location_cursor.x, textbox->location_cursor.y, textbox->forecol, s);
	Sheet_RefreshSheet(textbox->sheet, textbox->location_cursor.x, textbox->location_cursor.y, textbox->location_cursor.x + 8 - 1, textbox->location_cursor.y + 16 - 1);

	textbox->location_cursor.x += 8;
	return 0;
}



