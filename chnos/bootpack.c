
#include "core.h"

void CHNMain(void)
{
	uchar s[128];
	uint data;
	UI_Task *mytask;
	uint i;
	IO_DisplayControl *disp_ctrl;
	UI_Sheet *vramsheet, *testsheet, *sheet_desktop;
	UI_Sheet *sheet08, *sheet16, *sheet32;
	int x, y;
	UI_Timer *timer1, *timer2, *timer3;
	uint counter1, counter2, counter3;
	UI_TextBox *textbox;
	UI_Console *console;

	i = 0;
	data = 0;

	Initialize_System();

	mytask = System_MultiTask_GetNowTask();
	disp_ctrl = System_Display_Get_Controller();

	vramsheet = disp_ctrl->vramsheet;

	Sheet_Drawing_Fill_Rectangle(vramsheet, 0xffffff, 0, 0, vramsheet->size.x - 1, vramsheet->size.y - 1);
	Sheet_Drawing_Put_String(vramsheet, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	if(disp_ctrl->display_mode == DISPLAYMODE_VBE_LINEAR){
		i = 0;
		Sheet_Drawing_Put_String(vramsheet, 10, 10 + 16 * 1, 0x000000, "Please Select the VideoMode Number.");
		Sheet_Drawing_Put_String(vramsheet, 10, 10 + 16 * 2, 0x000000, "(Use cursor Up or Down, Select Enter.)");
		Sheet_Drawing_Fill_Rectangle(vramsheet, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 5 - 1);
		snprintf(s, sizeof(s), "%d:0x%X %dx%d-%dbits", i, disp_ctrl->VBE.list_vmode[i].mode_number, disp_ctrl->VBE.list_vmode[i].xsize, disp_ctrl->VBE.list_vmode[i].ysize, disp_ctrl->VBE.list_vmode[i].bpp);
		Sheet_Drawing_Put_String(vramsheet, 10, 10 + 16 * 3, 0x000000, s);

		for(;;){
			if(FIFO32_MyTaskFIFO_Status() == 0){
				System_MultiTask_Task_Sleep(mytask);
			} else{
				data = FIFO32_MyTaskFIFO_Get();
				if(SIGNAL_KEY_OFFSET <= data && data <= (SIGNAL_KEY_OFFSET + 0xFFFF)){
					data -= SIGNAL_KEY_OFFSET;
					if(!(data & KEYID_MASK_BREAK) && (data & KEYID_MASK_EXTENDED)){
						if((data & KEYID_MASK_ID) == KEYID_CURSOR_U){
							if(i == 0){
								i = disp_ctrl->VBE.list_vmode_tags - 1;
							} else{
								i--;
							}
							Sheet_Drawing_Fill_Rectangle(vramsheet, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 5 - 1);
							snprintf(s, sizeof(s), "%d:0x%X %dx%d-%dbits", i, disp_ctrl->VBE.list_vmode[i].mode_number, disp_ctrl->VBE.list_vmode[i].xsize, disp_ctrl->VBE.list_vmode[i].ysize, disp_ctrl->VBE.list_vmode[i].bpp);
							Sheet_Drawing_Put_String(vramsheet, 10, 10 + 16 * 3, 0x000000, s);
						} else if((data & KEYID_MASK_ID) == KEYID_CURSOR_D){
							if(i == disp_ctrl->VBE.list_vmode_tags - 1){
								i = 0;
							} else{
								i ++;
							}
							Sheet_Drawing_Fill_Rectangle(vramsheet, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 5 - 1);
							snprintf(s, sizeof(s), "%d:0x%X %dx%d-%dbits", i, disp_ctrl->VBE.list_vmode[i].mode_number, disp_ctrl->VBE.list_vmode[i].xsize, disp_ctrl->VBE.list_vmode[i].ysize, disp_ctrl->VBE.list_vmode[i].bpp);
							Sheet_Drawing_Put_String(vramsheet, 10, 10 + 16 * 3, 0x000000, s);
						} else if((data & KEYID_MASK_ID) == KEYID_ENTER){
							if(System_Display_VESA_Set_VideoMode(i) == 0){
								Sheet_SetBuffer(vramsheet, disp_ctrl->vram, disp_ctrl->xsize, disp_ctrl->ysize, disp_ctrl->bpp);
								break;
							}
							Sheet_Drawing_Fill_Rectangle(vramsheet, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 4 - 1);
							Sheet_Drawing_Put_String(vramsheet, 10, 10 + 16 * 4, 0x000000, "Function Failed. Try again.");
						}
					}
				}
			}
		}
	}

	sheet_desktop = Sheet_Initialize();
	Sheet_SetBuffer(sheet_desktop, Null, disp_ctrl->xsize, disp_ctrl->ysize, disp_ctrl->bpp);

	Sheet_Drawing_Fill_Rectangle(sheet_desktop, 0xffffff, 0, 0, disp_ctrl->xsize - 1, disp_ctrl->ysize - 1);
	Sheet_Drawing_Put_String(sheet_desktop, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	Format_BMP_DrawPicture(sheet_desktop->vram, sheet_desktop->size.x, 10, 26, 0, 0, chnlogo);

	Drawing_Fill_Circle(disp_ctrl->vram, disp_ctrl->xsize, 100, 250, 0x00c600, 45);
	for(i = 0; i < 50; i += 5){
		Drawing_Draw_Circle(disp_ctrl->vram, disp_ctrl->xsize, 100, 250, 0xc6c6c6, i);
	}

	testsheet = Sheet_Initialize();

	sheet08 = Sheet_Initialize();
	sheet16 = Sheet_Initialize();
	sheet32 = Sheet_Initialize();

	Sheet_SetBuffer(testsheet, Null, 256, 128, 8);
	Sheet_SetBuffer(sheet08, Null, 128, 128, 8);
	Sheet_SetBuffer(sheet16, Null, 128, 128, 16);
	Sheet_SetBuffer(sheet32, Null, 128, 128, 32);

	for(y = 0; y < testsheet->size.y; y++){
		for(x = 0; x < testsheet->size.x; x++){
			((uchar *)testsheet->vram)[y * testsheet->size.x + x] = x * 2;
		}
	}

	Drawing08_Fill_Rectangle(testsheet->vram, testsheet->size.x, 0xc6c6c6, 4, 24, testsheet->size.x - 4 - 1, testsheet->size.y - 4 - 1);

	textbox = TextBox_Initialize();
	TextBox_SetBuffer(textbox, 20, 6, 8, testsheet);
	TextBox_Show(textbox, 0, 4, 24);
	TextBox_Put_Character(textbox, '>');
	TextBox_SetEnable_RecordInputText(textbox, True);


	Drawing08_Put_String(testsheet->vram, testsheet->size.x, 4, 4, 0xffffff, "TestSheet");
	snprintf(s, sizeof(s), "Memory:%d Bytes", System_Get_PhisycalMemorySize());
	Drawing08_Put_String(testsheet->vram, testsheet->size.x, 8, 24 + 16 * 4, 0xffffff, s);

	for(y = 0; y < sheet08->size.y; y++){
		for(x = 0; x < sheet08->size.x; x++){
			((uchar *)sheet08->vram)[y * sheet08->size.x + x] = RGB_32_To_08(((x * 2) << 16) | ((y * 2) << 8) | (x + y));
		}
	}

	for(y = 0; y < sheet16->size.y; y++){
		for(x = 0; x < sheet16->size.x; x++){
			((ushort *)sheet16->vram)[y * sheet16->size.x + x] = RGB_32_To_16(((x * 2) << 16) | ((y * 2) << 8) | (x + y));
		}
	}

	for(y = 0; y < sheet32->size.y; y++){
		for(x = 0; x < sheet32->size.x; x++){
			((uint *)sheet32->vram)[y * sheet32->size.x + x] = ((x * 2) << 16) | ((y * 2) << 8) | (x + y);
		}
	}

	Sheet_SetParent(sheet_desktop, vramsheet);
	Sheet_Show(sheet_desktop, 0, 0, 0);

	Sheet_SetParent(sheet08, vramsheet);
	Sheet_Show(sheet08, 1, 20, vramsheet->size.y >> 1);

	Sheet_SetParent(sheet16, vramsheet);
	Sheet_Show(sheet16, 1, 220, vramsheet->size.y >> 1);

	Sheet_SetParent(sheet32, vramsheet);
	Sheet_Show(sheet32, 1, 420, vramsheet->size.y >> 1);

	Sheet_SetParent(testsheet, vramsheet);
	Sheet_SetMovable(testsheet, True);
	Sheet_SetInputFIFO(testsheet, mytask->fifo);
	Sheet_Show(testsheet, 6, 10, 10);

	timer1 = Timer_Initialize();
	Timer_Config(timer1, 50, mytask->fifo, 11, True);
	counter1 = 0;
	//Timer_Run(timer1);

	timer2 = Timer_Initialize();
	Timer_Config(timer2, 50, mytask->fifo, 12, False);
	counter2 = 0;
	//Timer_Run(timer2);

	timer3 = Timer_Initialize();
	Timer_Config(timer3, 200, mytask->fifo, 13, True);
	counter3 = 0;
	//Timer_Run(timer3);

	console = Console_Initialize();
	Console_SetSize(console, vramsheet->size.x >> 4, vramsheet->size.y >> 5);
	Console_Run(console);

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			System_MultiTask_Task_Sleep(mytask);
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			if(SIGNAL_KEY_OFFSET <= data && data <= (SIGNAL_KEY_OFFSET + 0xFFFF)){
				data -= SIGNAL_KEY_OFFSET;
				TextBox_Put_Key(textbox, data);
				if(!(data & KEYID_MASK_BREAK) && (data & KEYID_MASK_EXTENDED)){
					if((data & KEYID_MASK_ID) == KEYID_ENTER){
						TextBox_SetEnable_RecordInputText(textbox, False);
						TextBox_Put_String(textbox, textbox->text_buf);
						TextBox_Put_Character(textbox, '>');
						TextBox_SetEnable_RecordInputText(textbox, True);
					}
				}
			} else if(data == 11){
				Drawing08_Fill_Rectangle(testsheet->vram, testsheet->size.x, 0xc6c6c6, 8, 24, 8 + (20 * 8) - 1, 24 + (16 * 2) - 1);
				snprintf(s, sizeof(s), "Tick :%d", Timer_GetTick());
				Drawing08_Put_String(testsheet->vram, testsheet->size.x, 8, 24, 0xffffff, s);
				snprintf(s, sizeof(s), "Tick1:%d", counter1);
				Drawing08_Put_String(testsheet->vram, testsheet->size.x, 8, 24 + 16, 0xffffff, s);
				Sheet_RefreshSheet(testsheet, 8, 24, 8 + (20 * 8) - 1, 24 + (16 * 2) - 1);
				counter1++;
			} else if(data == 12){
				Drawing08_Fill_Rectangle(testsheet->vram, testsheet->size.x, 0xc6c6c6, 8, 24 + 16 * 2, 8 + (20 * 8) - 1, 24 + (16 * 3) - 1);
				snprintf(s, sizeof(s), "Tick2:%d", counter2);
				Drawing08_Put_String(testsheet->vram, testsheet->size.x, 8, 24 + 16 * 2, 0xffffff, s);
				Sheet_RefreshSheet(testsheet, 8, 24 + 16 * 2, 8 + (20 * 8) - 1, 24 + (16 * 3) - 1);
				Timer_Run(timer2);
				counter2++;
			} else if(data == 13){
				Drawing08_Fill_Rectangle(testsheet->vram, testsheet->size.x, 0xc6c6c6, 8, 24 + 16 * 3, 8 + (20 * 8) - 1, 24 + (16 * 4) - 1);
				snprintf(s, sizeof(s), "Tick3:%d", counter3);
				Drawing08_Put_String(testsheet->vram, testsheet->size.x, 8, 24 + 16 * 3, 0xffffff, s);
				Sheet_RefreshSheet(testsheet, 8, 24 + 16 * 3, 8 + (20 * 8) - 1, 24 + (16 * 4) - 1);
				counter3++;

				Drawing08_Fill_Rectangle(testsheet->vram, testsheet->size.x, 0xc6c6c6, 8, 24 + 16 * 5, 8 + (20 * 8) - 1, 24 + (16 * 6) - 1);
				snprintf(s, sizeof(s), "Free  :%d Bytes", System_Memory_Get_FreeSize());
				Drawing08_Put_String(testsheet->vram, testsheet->size.x, 8, 24 + 16 * 5, 0xffffff, s);
				Sheet_RefreshSheet(testsheet, 8, 24 + 16 * 5, 8 + (20 * 8) - 1, 24 + (16 * 6) - 1);
			}
		}
	}
}

void KeyboardControlTask(DATA_FIFO32 **InputFocus)
{
	UI_Task *mytask;
	uint data;

	data = 0;

	mytask = System_MultiTask_GetNowTask();

	#ifdef CHNOSPROJECT_DEBUG
		debug("KBCT:KeyboardControlTask Start Running.\nKBCT:UI_Task=0x%X\n", mytask);
	#endif

	Keyboard_Set_ReceiveFIFO(mytask->fifo, 0x100);

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			System_MultiTask_Task_Sleep(mytask);
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			#ifdef CHNOSPROJECT_DEBUG_KBCT
				debug("KBCT:Receive data from FIFO(data:0x%X).\n", data);
			#endif
			if(0x100 <= data && data <= 0x1ff){
				data -= 0x100;
				data = Keyboard_Decode_KeyCode(data);
				if(InputFocus != 0 && data != 0){
					FIFO32_Put(*InputFocus, data + SIGNAL_KEY_OFFSET);
				}
			}
		}
	}
}

void MouseControlTask(DATA_FIFO32 **InputFocus, UI_MouseCursor *mcursor)
{
	UI_Task *mytask;
	uint data;
	IO_MouseControl *mctrl;
	#ifdef CHNOSPROJECT_DEBUG_MCT
		UI_Sheet *mouseinfosheet;
		uchar s[16];
	#endif
	int scroll;

	DATA_Location2D moveorg_mfocus;
	UI_Sheet *mfocus;
	bool old_mouse_buttonL;

	data = 0;
	scroll = 0;
	mfocus = Null;
	old_mouse_buttonL = False;

	mytask = System_MultiTask_GetNowTask();

	#ifdef CHNOSPROJECT_DEBUG
		debug("MCT:MouseControlTask Start Running.\nMCT:UI_Task=0x%X\n", mytask);
	#endif

	#ifdef CHNOSPROJECT_DEBUG_MCT
		mouseinfosheet = Sheet_Initialize();
		Sheet_SetBuffer(mouseinfosheet, Null, (4 * 2) + (8 * 16), 4 + 16 + (4 * 2) + (16 * 4), 8);
		System_Sheet_SetParentToVRAM(mouseinfosheet);
		Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0x99cc33, 0, 0, mouseinfosheet->size.x - 1, mouseinfosheet->size.y - 1);
		Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24, mouseinfosheet->size.x - 1 - 4, mouseinfosheet->size.y - 1 - 4);
		Sheet_Drawing_Put_String(mouseinfosheet, 4, 4, 0xffffff, "MouseInfo");
	#endif

	mctrl = Initialize_Mouse();

	#ifdef CHNOSPROJECT_DEBUG_MCT
		debug("MCT:Mouse Initialized.\n");
	#endif

	Mouse_Set_ReceiveFIFO(mytask->fifo, 0x100);

	Mouse_Decode(mctrl, 0x00);	//Decode start.

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			if(mfocus != Null && ((mcursor->cursor_sheet->location.x - moveorg_mfocus.x) != 0 || (mcursor->cursor_sheet->location.y - moveorg_mfocus.y) != 0)){
				Sheet_Slide_Relative(mfocus, mcursor->cursor_sheet->location.x - moveorg_mfocus.x, mcursor->cursor_sheet->location.y - moveorg_mfocus.y);
				moveorg_mfocus.x = mcursor->cursor_sheet->location.x;
				moveorg_mfocus.y = mcursor->cursor_sheet->location.y;
			} else{
				System_MultiTask_Task_Sleep(mytask);
			}
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			#ifdef CHNOSPROJECT_DEBUG_MCT
				debug("MCT:Receive data from FIFO(data:0x%X).\n", data);
			#endif
			if(0x100 <= data && data <= 0x1ff){
				if(Mouse_Decode(mctrl, data - 0x100)){
					MouseCursor_Move_Relative(mcursor, mctrl->move.x, mctrl->move.y);
					#ifdef CHNOSPROJECT_DEBUG_MCT
						Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 0), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 0) + 15);
						snprintf(s, sizeof(s), "X:%d", mcursor->cursor_sheet->location.x);
						Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 0), 0x000000, s);
						Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 1), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 1) + 15);
						snprintf(s, sizeof(s), "Y:%d", mcursor->cursor_sheet->location.y);
						Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 1), 0x000000, s);
						Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 2), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 2) + 15);
						snprintf(s, sizeof(s), "Button:lrc");
					#endif

					if(old_mouse_buttonL != mctrl->button.bit.L){
						if(old_mouse_buttonL){	//up
							if(mfocus != Null){
								Sheet_Slide_Relative(mfocus, mcursor->cursor_sheet->location.x - moveorg_mfocus.x, mcursor->cursor_sheet->location.y - moveorg_mfocus.y);
							}
							mfocus = Null;
						} else{	//down
							moveorg_mfocus.x = mcursor->cursor_sheet->location.x;
							moveorg_mfocus.y = mcursor->cursor_sheet->location.y;
							mfocus = Sheet_GetSheetFromLocation(mcursor->cursor_sheet->parent, mcursor->cursor_sheet->location.x, mcursor->cursor_sheet->location.y);
							System_InputFocus_Change(mfocus->input_fifo);
							if(mfocus != Null && mfocus->flags.bit.movable == False){
								mfocus = Null;
							}
						}
						old_mouse_buttonL = mctrl->button.bit.L;
					}
					#ifdef CHNOSPROJECT_DEBUG_MCT
						if(mctrl->button.bit.L){
							#ifdef CHNOSPROJECT_DEBUG_MCT
								s[7] -= 0x20;
							#endif
						}
						if(mctrl->button.bit.R){
							#ifdef CHNOSPROJECT_DEBUG_MCT
								s[8] -= 0x20;
							#endif
						}
						if(mctrl->button.bit.C){
							#ifdef CHNOSPROJECT_DEBUG_MCT
								s[9] -= 0x20;
							#endif
						}
					#endif
					#ifdef CHNOSPROJECT_DEBUG_MCT
						Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 2), 0x000000, s);
					#endif
					if(mctrl->flags.scroll){
						scroll += mctrl->scroll;
						#ifdef CHNOSPROJECT_DEBUG_MCT
							Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 3), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 3) + 15);
							snprintf(s, sizeof(s), "Scroll:%d", scroll);
							Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 3), 0x000000, s);
							Sheet_RefreshSheet(mouseinfosheet, 4, 24 + (16 * 0), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 3) + 15);
						#endif
					} else{
						#ifdef CHNOSPROJECT_DEBUG_MCT
							Sheet_RefreshSheet(mouseinfosheet, 4, 24 + (16 * 0), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 2) + 15);
						#endif
					}
				}
			} else if(data == TCM_OFFSET + TCM_INFO_DISPLAY_UPDATE_RESOLUTION){
				MouseCursor_Move_Absolute(mcursor, mcursor->cursor_sheet->parent->size.x >> 1, mcursor->cursor_sheet->parent->size.y >> 1);
				MouseCursor_Show(mcursor);
				#ifdef CHNOSPROJECT_DEBUG_MCT
					Sheet_Show(mouseinfosheet, 2, 200, 200);
				#endif
			}
		}
	}
}


