
#include "core.h"

uint kbd_data0;
DATA_FIFO32 *kbd_fifo;

union  STATE_KEYLOCK {
	uchar keylock;
	struct STATE_KEYLOCK_KEY {
		unsigned num : 1;
		unsigned caps : 1;
		unsigned scroll : 1;
	} key;
} state_keylock;

union  STATE_KEYSHIFT {
	uchar keyshift;
	struct STATE_KEYSHIFT_KEY {
		unsigned L : 1;
		unsigned R : 1;
	} key;
} state_keyshift;

union  STATE_KEYCTRL {
	uchar keyctrl;
	struct STATE_KEYCTRL_KEY {
		unsigned L : 1;
		unsigned R : 1;
	} key;
} state_keyctrl;

union  STATE_KEYALT {
	uchar keyalt;
	struct STATE_KEYALT_KEY {
		unsigned L : 1;
		unsigned R : 1;
	} key;
} state_keyalt;

ushort state_shift;

uint key_decode_phase;
uchar key_decode_buf[4];

/*キーコード変換テーブル*/
ushort Keyboard_KeyCodeTable[0x80] = {
	KEYID_MASK_EXTENDED | KEYID_KBD_ERROR,
	KEYID_MASK_EXTENDED | KEYID_ESC,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'-',
	'^',
	KEYID_MASK_EXTENDED | KEYID_BACKSPACE,
	KEYID_MASK_EXTENDED | KEYID_TAB,
/*0x10*/
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'@',
	'[',
	KEYID_MASK_EXTENDED | KEYID_ENTER,
	KEYID_MASK_EXTENDED | KEYID_CTRL_L,
	'A',
	'S',
/*0x20*/
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	';',
	':',
	KEYID_MASK_EXTENDED | KEYID_KANJI,
	KEYID_MASK_EXTENDED | KEYID_SHIFT_L,
	']',
	'Z',
	'X',
	'C',
	'V',
/*0x30*/
	'B',
	'N',
	'M',
	',',
	'.',
	'/',
	KEYID_MASK_EXTENDED | KEYID_SHIFT_R,
	KEYID_MASK_TENKEY | '*',
	KEYID_MASK_EXTENDED | KEYID_ALT_L,
	' ',
	KEYID_MASK_EXTENDED | KEYID_LOCK_CAPS,
	KEYID_MASK_EXTENDED | KEYID_F1,
	KEYID_MASK_EXTENDED | KEYID_F2,
	KEYID_MASK_EXTENDED | KEYID_F3,
	KEYID_MASK_EXTENDED | KEYID_F4,
	KEYID_MASK_EXTENDED | KEYID_F5,
/*0x40*/
	KEYID_MASK_EXTENDED | KEYID_F6,
	KEYID_MASK_EXTENDED | KEYID_F7,
	KEYID_MASK_EXTENDED | KEYID_F8,
	KEYID_MASK_EXTENDED | KEYID_F9,
	KEYID_MASK_EXTENDED | KEYID_F10,
	KEYID_MASK_EXTENDED | KEYID_LOCK_NUM,
	KEYID_MASK_EXTENDED | KEYID_LOCK_SCROOL,
	KEYID_MASK_TENKEY | '7',
	KEYID_MASK_TENKEY | '8',
	KEYID_MASK_TENKEY | '9',
	KEYID_MASK_TENKEY | '-',
	KEYID_MASK_TENKEY | '4',
	KEYID_MASK_TENKEY | '5',
	KEYID_MASK_TENKEY | '6',
	KEYID_MASK_TENKEY | '+',
	KEYID_MASK_TENKEY | '1',
/*0x50*/
	KEYID_MASK_TENKEY | '2',
	KEYID_MASK_TENKEY | '3',
	KEYID_MASK_TENKEY | '0',
	KEYID_MASK_TENKEY | '.',
	KEYID_MASK_EXTENDED | KEYID_SYS_RQ,
	0x0000,
	0x0000,
	KEYID_MASK_EXTENDED | KEYID_F11,
	KEYID_MASK_EXTENDED | KEYID_F12,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
/*0x60*/
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
/*0x70*/
	KEYID_MASK_EXTENDED | KEYID_HIRAGANA,
	0x0000,
	0x0000,
	'_',
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	KEYID_MASK_EXTENDED | KEYID_HENKAN,
	0x0000,
	KEYID_MASK_EXTENDED | KEYID_MUHENKAN,
	0x0000,
	0x5c,	//\ for mikan-trap
	0x0000,
	0x0000
};

//ゼロの所はシフト文字が無いので、テーブル0を参照
uchar Keyboard_KeyCodeTable_Shift[0x80] = {
	0x00,
	0x00,
	'!',
	0x22,	//double quote
	'#',
	'$',
	'%',
	'&',
	0x27,	//single quote
	'(',
	')',
	'~',
	'=',
	'~',
	0x00,
	0x00,
/*0x10*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	'`',
	'{',
	0x00,
	0x00,
	0x00,
	0x00,
/*0x20*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	'+',
	'*',
	0x00,
	0x00,
	'}',
	0x00,
	0x00,
	0x00,
	0x00,
/*0x30*/
	0x00,
	0x00,
	0x00,
	'<',
	'>',
	'?',
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
/*0x40*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
/*0x50*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
/*0x60*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
/*0x70*/
	0x00,
	0x00,
	0x00,
	'_',
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	'|',
	0x00,
	0x00
};

void Initialise_Keyboard(void)
{
	System_GateDescriptor_Set(0x21, (uint)asm_InterruptHandler21, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x01);
	kbd_data0 = 0;
	kbd_fifo = 0;
	state_keylock.keylock = 0;
	state_keyshift.keyshift = 0;
	state_keyctrl.keyctrl = 0;
	state_keyalt.keyalt = 0;
	key_decode_phase = 0;
	return;
}

void InterruptHandler21(uint *esp)
{
	uint data;

	data = IO_In8(KEYB_DATA);

	ProgrammableInterruptController_InterruptRequest_Complete(0x01);

	if(kbd_fifo != 0){
		FIFO32_Put(kbd_fifo, data + kbd_data0);
	}

	return;
}

void Keyboard_Set_ReceiveFIFO(DATA_FIFO32 *fifo, uint data0)
{
	kbd_data0 = data0;
	kbd_fifo = fifo;

	return; 
}

ushort Keyboard_Decode_KeyCode(uchar keycode)
{
	ushort keyid;
	ushort table;
	keyid = 0;
	table = 0;

	key_decode_buf[key_decode_phase] = keycode;

	switch(key_decode_phase){
		case 0:
			if(key_decode_buf[0] == 0xe0){
				key_decode_phase = 1;
				break;
			} else if(key_decode_buf[0] == 0xe1){
				key_decode_phase = 2;
				break;
			} else{
				if(state_keylock.key.caps){
					if(!state_keyshift.keyshift){
						table = 1;
					}
				} else{
					if(state_keyshift.keyshift){
						table = 1;
					}
				}

				if(state_keyshift.keyshift){
					keyid = Keyboard_KeyCodeTable_Shift[(key_decode_buf[0] & KEYID_MASK_ID)];
					if(keyid == 0){
						keyid = Keyboard_KeyCodeTable[(key_decode_buf[0] & KEYID_MASK_ID)];
					}
				} else{
					keyid = Keyboard_KeyCodeTable[(key_decode_buf[0] & KEYID_MASK_ID)];
				}

				if(!table && ('A' <= keyid && keyid <= 'Z')){
					keyid += 0x20;
				}

				if(keyid & KEYID_MASK_EXTENDED){
					if(keyid == (KEYID_MASK_EXTENDED | KEYID_CTRL_L)){
						if((key_decode_buf[0] & KEYID_MASK_BREAK)){
							state_keyctrl.key.L = False;
						} else{
							state_keyctrl.key.L = True;
						}
					} else if(keyid == (KEYID_MASK_EXTENDED | KEYID_SHIFT_L)){
						if((key_decode_buf[0] & KEYID_MASK_BREAK)){
							state_keyshift.key.L = False;
						} else{
							state_keyshift.key.L = True;
						}
					} else if(keyid == (KEYID_MASK_EXTENDED | KEYID_SHIFT_R)){
						if((key_decode_buf[0] & KEYID_MASK_BREAK)){
							state_keyshift.key.R = False;
						} else{
							state_keyshift.key.R = True;
						}
					} else if(keyid == (KEYID_MASK_EXTENDED | KEYID_ALT_L)){
						if((key_decode_buf[0] & KEYID_MASK_BREAK)){
							state_keyalt.key.L = False;
						} else{
							state_keyalt.key.L = True;
						}
					} else if(keyid == (KEYID_MASK_EXTENDED | KEYID_LOCK_CAPS)){
						if(!(key_decode_buf[0] & KEYID_MASK_BREAK)){
							if(state_keylock.key.caps){
								state_keylock.key.caps = False;
							} else{
								state_keylock.key.caps = True;
							}
						}
					} else if(keyid == (KEYID_MASK_EXTENDED | KEYID_LOCK_NUM)){
						if(!(key_decode_buf[0] & KEYID_MASK_BREAK)){
							if(state_keylock.key.num){
								state_keylock.key.num = False;
							} else{
								state_keylock.key.num = True;
							}
						}
					} else if(keyid == (KEYID_MASK_EXTENDED | KEYID_LOCK_SCROOL)){
						if(!(key_decode_buf[0] & KEYID_MASK_BREAK)){
							if(state_keylock.key.scroll){
								state_keylock.key.scroll = False;
							} else{
								state_keylock.key.scroll = True;
							}
						}
					}
				}

				if(keyid != 0 && (key_decode_buf[0] & KEYID_MASK_BREAK)){
					keyid |= KEYID_MASK_BREAK;
				}

			}
			key_decode_phase = 0;
			break;
		case 1:	//E0拡張キーコード
			if((key_decode_buf[1] & KEYID_MASK_ID) == 0x1c){
				keyid |= KEYID_MASK_TENKEY | KEYID_ENTER;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x1d){
				keyid |= KEYID_MASK_EXTENDED | KEYID_CTRL_R;
				if((key_decode_buf[1] & KEYID_MASK_BREAK)){
					state_keyctrl.key.R = False;
				} else{
					state_keyctrl.key.R = True;
				}
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x35){
				keyid |= KEYID_MASK_TENKEY | '/';
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x37){
				keyid |= KEYID_MASK_EXTENDED | KEYID_PRINT_SCREEN;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x38){
				keyid |= KEYID_MASK_EXTENDED | KEYID_ALT_R;
				if((key_decode_buf[1] & KEYID_MASK_BREAK)){
					state_keyalt.key.R = False;
				} else{
					state_keyalt.key.R = True;
				}
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x46){
				keyid |= KEYID_MASK_EXTENDED | KEYID_BREAK;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x47){
				keyid |= KEYID_MASK_EXTENDED | KEYID_HOME;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x48){
				keyid |= KEYID_MASK_EXTENDED | KEYID_CURSOR_U;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x49){
				keyid |= KEYID_MASK_EXTENDED | KEYID_PAGE_UP;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x4b){
				keyid |= KEYID_MASK_EXTENDED | KEYID_CURSOR_L;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x4d){
				keyid |= KEYID_MASK_EXTENDED | KEYID_CURSOR_R;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x4f){
				keyid |= KEYID_MASK_EXTENDED | KEYID_END;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x50){
				keyid |= KEYID_MASK_EXTENDED | KEYID_CURSOR_D;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x51){
				keyid |= KEYID_MASK_EXTENDED | KEYID_PAGE_DOWN;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x52){
				keyid |= KEYID_MASK_EXTENDED | KEYID_INSERT;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x53){
				keyid |= KEYID_MASK_EXTENDED | KEYID_DELETE;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x5b){
				keyid |= KEYID_MASK_EXTENDED | KEYID_ICON_L;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x5c){
				keyid |= KEYID_MASK_EXTENDED | KEYID_ICON_R;
			} else if((key_decode_buf[1] & KEYID_MASK_ID) == 0x5d){
				keyid |= KEYID_MASK_EXTENDED | KEYID_MENU;
			}

			if(keyid != 0 && (key_decode_buf[1] & KEYID_MASK_BREAK)){
				keyid |= KEYID_MASK_BREAK;
			}

			key_decode_phase = 0;
			break;
		case 2:	//E1拡張キーコードフェーズ0
			key_decode_phase = 3;
			break;
		case 3:	//E1拡張キーコードフェーズ1
			if((key_decode_buf[1] == 0x1d) && (key_decode_buf[2] == 0x45)){	//Pause-Key Make
				keyid = KEYID_MASK_EXTENDED | KEYID_PAUSE;
			} else if((key_decode_buf[1] == 0x9d) && (key_decode_buf[2] == 0xc5)){	//Pause-Key Break
				keyid = KEYID_MASK_EXTENDED | KEYID_MASK_BREAK | KEYID_PAUSE;
			}
			key_decode_phase = 0;
			break;
	}

	if(keyid != 0){
		if(state_keyshift.keyshift){
			keyid |= KEYID_MASK_STATE_SHIFT;
		}
		if(state_keyctrl.keyctrl){
			keyid |= KEYID_MASK_STATE_CTRL;
		}
		if(state_keyalt.keyalt){
			keyid |= KEYID_MASK_STATE_ALT;
		}
		keyid |= ((ushort)state_keylock.keylock << 12);
	}

	return keyid;
}
