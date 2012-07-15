
#include "core.h"

UI_TextBox *TextBox_Initialize(void)
{
	UI_TextBox *textbox;

	textbox = (UI_TextBox *)System_CommonStruct_Allocate(SYSTEM_STRUCTID_TEXTBOX);
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
			debug("TextBox_SetBuffer:Null textbox.\n");
		#endif
		return 1;
	}

	if(xchars < 4 + 1 || ychars < 1){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_SetBuffer:Too small textbox.\n");
		#endif
		return 2;
	}

	textbox->chars.x = xchars;
	textbox->chars.y = ychars;
	textbox->size_text_buf = textbox->chars.x * textbox->chars.y;

	textbox->sheet = Sheet_Initialize();
	Sheet_SetBuffer(textbox->sheet, Null, xchars << 3, ychars << 4, bpp);
	Sheet_SetParent(textbox->sheet, parent);

	Sheet_Drawing_Fill_Rectangle(textbox->sheet, textbox->backcol, 0, 0, (int)textbox->sheet->size.x - 1, (int)textbox->sheet->size.y - 1);

	textbox->text_buf = System_Memory_Allocate(textbox->size_text_buf);
	textbox->text_buf[0] = 0x00;

	textbox->flags.bit.textbuffer_configured = True;

	return 0;
}

uint TextBox_Show(UI_TextBox *textbox, uint height, int px, int py)
{
	if(textbox == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_Show:Null textbox.\n");
		#endif
		return 1;
	}

	if(textbox->sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_Show:Null textbox sheet.\n");
		#endif
		return 2;
	}

	return Sheet_Show(textbox->sheet, height, px, py);
}

//charを渡すことも可能。
//Break時のキーコードは無視して何も入力しない（keyidを直接渡すだけで入力ができるようにするため）。
//タブ文字は、記録がオフの時のみ入力可能。
uint TextBox_Put_Character(UI_TextBox *textbox, ushort keyid)
{
	uchar s[2];
	uint i;

	if(textbox == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_Put_Character:Null textbox.\n");
		#endif
		return 1;
	}

	if(textbox->sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_Put_Character:Null textbox sheet.\n");
		#endif
		return 2;
	}

	if(!(keyid & KEYID_MASK_BREAK)){
		if(keyid & KEYID_MASK_EXTENDED){	/*制御文字*/
			switch(keyid & KEYID_MASK_ID){
				case KEYID_ENTER:
					TextBox_Put_Character(textbox, '\n');
					break;
				case KEYID_BACKSPACE:
					TextBox_Put_Character(textbox, '\b');
					break;
				case KEYID_TAB:
					TextBox_Put_Character(textbox, '\t');
					break;
			}
			return 0;
		} else{	/*ASCII文字*/
			s[0] = keyid & KEYID_MASK_ID;
			s[1] = 0x00;
			if(s[0] == '\n'){
				if(TextBox_Internal_Put_Character_TextBuffer(textbox, '\n')){
					textbox->location_cursor.x = 0;
					textbox->location_cursor.y += 16;
				}
			} else if(s[0] == '\b'){
				if(!(textbox->location_cursor.x <= 0 && textbox->location_cursor.y <= 0)){
					if(TextBox_Internal_Put_Character_TextBuffer(textbox, '\b')){
						textbox->location_cursor.x -= 8;
					}
					if(textbox->location_cursor.x < 0){
						textbox->location_cursor.x = textbox->location_cursor_record_started.x;
						textbox->location_cursor.y -= 16;
						if(textbox->flags.bit.record_input_text){
							for(i = 0; i < textbox->using_text_buf; i++){
								if(textbox->text_buf[textbox->using_text_buf - i - 1] == '\n'){
									break;
								}
								textbox->location_cursor.x += 8;
								if(textbox->location_cursor.x >= (int)textbox->sheet->size.x - (8 - 1)){
									textbox->location_cursor.x = 0;
								}
								if(textbox->using_text_buf - i - 1 == 0){
									break;
								}
							}
						} else{
							textbox->location_cursor.x = (int)textbox->sheet->size.x - 8;
						}
					}
				}
			} else if(s[0] == '\t'){
				if(!textbox->flags.bit.record_input_text && TextBox_Internal_Put_Character_TextBuffer(textbox, '\t')){
					textbox->location_cursor.x += 8 * (4 - ((textbox->location_cursor.x >> 3) & 3));
				}
				if(textbox->location_cursor.x > (int)textbox->sheet->size.x){
					textbox->location_cursor.x = 8 * 4;
					textbox->location_cursor.y += 16;
				} else if(textbox->location_cursor.x == (int)textbox->sheet->size.x){
					textbox->location_cursor.x = 0;
					textbox->location_cursor.y += 16;
				}
			} else if(TextBox_Internal_Put_Character_TextBuffer(textbox, s[0])){
				#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
					debug("TextBox_Put_Character:put [%c].\n", s[0]);
				#endif
				Sheet_Drawing_Put_String(textbox->sheet, textbox->location_cursor.x, textbox->location_cursor.y, textbox->forecol, s);
				Sheet_RefreshSheet(textbox->sheet, textbox->location_cursor.x, textbox->location_cursor.y, textbox->location_cursor.x + 8 - 1, textbox->location_cursor.y + 16 - 1);
				textbox->location_cursor.x += 8;
				if(textbox->location_cursor.x >= (int)textbox->sheet->size.x - (8 - 1)){
					textbox->location_cursor.x = 0;
					textbox->location_cursor.y += 16;
				}
			}
		}
		if(textbox->location_cursor.y >= (int)textbox->sheet->size.y - (16 - 1)){
			Sheet_Drawing_Scroll_Vertical(textbox->sheet, 16);
			Sheet_Drawing_Fill_Rectangle(textbox->sheet, textbox->backcol, 0, (int)textbox->sheet->size.y - 16, (int)textbox->sheet->size.x - 1, (int)textbox->sheet->size.y - 1);
			textbox->location_cursor.y -= 16;
			Sheet_RefreshSheet_All(textbox->sheet);
		}
		Sheet_Drawing_Fill_Rectangle(textbox->sheet, textbox->backcol, textbox->location_cursor.x, textbox->location_cursor.y, textbox->location_cursor.x + 8 - 1, textbox->location_cursor.y + 16 - 1);
		Sheet_RefreshSheet(textbox->sheet, textbox->location_cursor.x, textbox->location_cursor.y, textbox->location_cursor.x + 8 - 1, textbox->location_cursor.y + 16 - 1);
	}

	return 0;
}

uint TextBox_Put_String(UI_TextBox *textbox, const uchar s[])
{
	uint i;

	for(i = 0; s[i] != 0x00; i++){
		TextBox_Put_Character(textbox, s[i]);
	}
	return 0;
}

bool TextBox_Internal_Put_Character_TextBuffer(UI_TextBox *textbox, uchar c)
{
	if(!textbox->flags.bit.record_input_text){
		return True;
	}

	switch(c){
		case '\b':
			if(textbox->using_text_buf > 0){
				textbox->using_text_buf--;
				textbox->text_buf[textbox->using_text_buf] = 0x00;
				return True;
			}
			break;
		default:
			if(textbox->using_text_buf < textbox->size_text_buf - 1){
				textbox->text_buf[textbox->using_text_buf] = c;
				textbox->using_text_buf++;
				textbox->text_buf[textbox->using_text_buf] = 0x00;
				return True;
			}
			break;
	}
	return False;
}

//False->Trueでバッファリセット。
bool TextBox_SetEnable_RecordInputText(UI_TextBox *textbox, bool enable)
{
	bool old;

	if(textbox == Null){
		#ifdef CHNOSPROJECT_DEBUG_TEXTBOX
			debug("TextBox_SetEnable_RecordInputText:Null textbox.\n");
		#endif
		return 1;
	}

	old = textbox->flags.bit.record_input_text;

	if(old != enable){
		if(enable){
			textbox->text_buf[0] = 0x00;
			textbox->using_text_buf = 0;
			textbox->location_cursor_record_started = textbox->location_cursor;
		}
		textbox->flags.bit.record_input_text = enable;
	}

	return old;
}
