
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

	i = 0;
	data = 0;

	Initialise_System();

	mytask = System_MultiTask_GetNowTask();
	disp_ctrl = System_Display_Get_Controller();

	vramsheet = disp_ctrl->vramsheet;

	Sheet_Drawing_Fill_Rectangle(vramsheet, 0xffffff, 0, 0, vramsheet->size.x - 1, vramsheet->size.y - 1);
	Sheet_Drawing_Put_String(vramsheet, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	if(disp_ctrl->display_mode == DISPLAYMODE_VBE_LINEAR){
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

	Sheet_Drawing_Fill_Rectangle(vramsheet, 0xffffff, 0, 0, disp_ctrl->xsize - 1, disp_ctrl->ysize - 1);
	Sheet_Drawing_Put_String(vramsheet, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	Format_BMP_DrawPicture(disp_ctrl->vram, disp_ctrl->xsize, 10, 26, 0, 0, chnlogo);

	Drawing_Fill_Circle(disp_ctrl->vram, disp_ctrl->xsize, 100, 250, 0x00c600, 45);
	for(i = 0; i < 50; i += 5){
		Drawing_Draw_Circle(disp_ctrl->vram, disp_ctrl->xsize, 100, 250, 0xc6c6c6, i);
	}

	testsheet = Sheet_Initialise();
	sheet_desktop = Sheet_Initialise();
	sheet08 = Sheet_Initialise();
	sheet16 = Sheet_Initialise();
	sheet32 = Sheet_Initialise();

	Sheet_SetBuffer(testsheet, Null, 256, 128, 8);
	Sheet_SetBuffer(sheet_desktop, Null, disp_ctrl->xsize, disp_ctrl->ysize, disp_ctrl->bpp);
	Sheet_SetBuffer(sheet08, Null, 128, 128, 8);
	Sheet_SetBuffer(sheet16, Null, 128, 128, 16);
	Sheet_SetBuffer(sheet32, Null, 128, 128, 32);

	for(i = 0; i < sheet_desktop->vramsize; i++){
		((uchar *)sheet_desktop->vram)[i] = ((uchar *)disp_ctrl->vram)[i];
	}

	for(y = 0; y < testsheet->size.y; y++){
		for(x = 0; x < testsheet->size.x; x++){
			((uchar *)testsheet->vram)[y * testsheet->size.x + x] = x * 2;
		}
	}

	Drawing08_Fill_Rectangle(testsheet->vram, testsheet->size.x, 0xc6c6c6, 4, 24, testsheet->size.x - 4 - 1, testsheet->size.y - 4 - 1);

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
	Sheet_Show(testsheet, 6, 10, 10);

	timer1 = Timer_Initialise();
	Timer_Config(timer1, 50, mytask->fifo, 11, True);
	counter1 = 0;
	Timer_Run(timer1);

	timer2 = Timer_Initialise();
	Timer_Config(timer2, 50, mytask->fifo, 12, False);
	counter2 = 0;
	Timer_Run(timer2);

	timer3 = Timer_Initialise();
	Timer_Config(timer3, 200, mytask->fifo, 13, True);
	counter3 = 0;
	Timer_Run(timer3);

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			System_MultiTask_Task_Sleep(mytask);
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			if(SIGNAL_KEY_OFFSET <= data && data <= (SIGNAL_KEY_OFFSET + 0xFFFF)){
				data -= SIGNAL_KEY_OFFSET;
				if(!(data & KEYID_MASK_BREAK) && (data & KEYID_MASK_EXTENDED)){
					if((data & KEYID_MASK_ID) == KEYID_CURSOR_U){
						if(data & KEYID_MASK_STATE_SHIFT){
							Sheet_Slide_Relative(testsheet, 5, -5);
						} else{
							Sheet_Slide_Relative(testsheet, 0, -5);
						}
					} else if((data & KEYID_MASK_ID) == KEYID_CURSOR_D){
						if(data & KEYID_MASK_STATE_SHIFT){
							Sheet_Slide_Relative(testsheet, -5, 5);
						} else{
							Sheet_Slide_Relative(testsheet, 0, 5);
						}
					} else if((data & KEYID_MASK_ID) == KEYID_CURSOR_L){
						if(data & KEYID_MASK_STATE_SHIFT){
							Sheet_Slide_Relative(testsheet, -5, -5);
						} else{
							Sheet_Slide_Relative(testsheet, -5, 0);
						}
					} else if((data & KEYID_MASK_ID) == KEYID_CURSOR_R){
						if(data & KEYID_MASK_STATE_SHIFT){
							Sheet_Slide_Relative(testsheet, 5, 5);
						} else{
							Sheet_Slide_Relative(testsheet, 5, 0);
						}
					} else if((data & KEYID_MASK_ID) == KEYID_ENTER){
						Sheet_Slide_Absolute(testsheet, disp_ctrl->xsize >> 1, disp_ctrl->ysize >> 1);
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
	UI_Sheet *mouseinfosheet;
	uchar s[16];
	int scroll;

	data = 0;
	scroll = 0;

	mytask = System_MultiTask_GetNowTask();

	#ifdef CHNOSPROJECT_DEBUG
		debug("MCT:MouseControlTask Start Running.\nMCT:UI_Task=0x%X\n", mytask);
	#endif

	mouseinfosheet = Sheet_Initialise();
	Sheet_SetBuffer(mouseinfosheet, Null, (4 * 2) + (8 * 16), 4 + 16 + (4 * 2) + (16 * 4), 8);
	System_Sheet_SetParentToVRAM(mouseinfosheet);
	Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0x99cc33, 0, 0, mouseinfosheet->size.x - 1, mouseinfosheet->size.y - 1);
	Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24, mouseinfosheet->size.x - 1 - 4, mouseinfosheet->size.y - 1 - 4);
	Sheet_Drawing_Put_String(mouseinfosheet, 4, 4, 0xffffff, "MouseInfo");

	mctrl = Initialise_Mouse();

	#ifdef CHNOSPROJECT_DEBUG_MCT
		debug("MCT:Mouse Initialized.\n");
	#endif

	MouseCursor_Show(mcursor);
	Mouse_Set_ReceiveFIFO(mytask->fifo, 0x100);

	Mouse_Decode(mctrl, 0x00);	//Decode start.

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			System_MultiTask_Task_Sleep(mytask);
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			#ifdef CHNOSPROJECT_DEBUG_MCT
				debug("MCT:Receive data from FIFO(data:0x%X).\n", data);
			#endif
			if(0x100 <= data && data <= 0x1ff){
				if(Mouse_Decode(mctrl, data - 0x100)){
					MouseCursor_Move_Relative(mcursor, mctrl->move.x, mctrl->move.y);
					Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 0), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 0) + 15);
					snprintf(s, sizeof(s), "X:%d", mcursor->cursor_sheet->location.x);
					Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 0), 0x000000, s);
					Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 1), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 1) + 15);
					snprintf(s, sizeof(s), "Y:%d", mcursor->cursor_sheet->location.y);
					Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 1), 0x000000, s);
					Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 2), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 2) + 15);
					snprintf(s, sizeof(s), "Button:lrc");
					if(mctrl->button.bit.L){
						s[7] -= 0x20;
					}
					if(mctrl->button.bit.R){
						s[8] -= 0x20;
					}
					if(mctrl->button.bit.C){
						s[9] -= 0x20;
					}
					Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 2), 0x000000, s);
					if(mctrl->flags.scroll){
						scroll += mctrl->scroll;
						Sheet_Drawing_Fill_Rectangle(mouseinfosheet, 0xccffff, 4, 24 + (16 * 3), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 3) + 15);
						snprintf(s, sizeof(s), "Scroll:%d", scroll);
						Sheet_Drawing_Put_String(mouseinfosheet, 4, 24 + (16 * 3), 0x000000, s);

						Sheet_RefreshSheet(mouseinfosheet, 4, 24 + (16 * 0), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 3) + 15);
					} else{
						Sheet_RefreshSheet(mouseinfosheet, 4, 24 + (16 * 0), mouseinfosheet->size.x - 1 - 4, 24 + (16 * 2) + 15);
					}
				}
			} else if(data == TCM_INFO_DISPLAY_UPDATE_RESOLUTION){
				MouseCursor_Move_Absolute(mcursor, mcursor->cursor_sheet->parent->size.x >> 1, mcursor->cursor_sheet->parent->size.y >> 1);
				Sheet_Show(mouseinfosheet, 2, 200, 200);
			}
		}
	}
}


