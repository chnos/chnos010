
#include "core.h"

#define MAIN_KEYBASE	0x100
void CHNMain(void)
{
	uchar s[128];
	uint data;
	UI_Task *KBCT;
	UI_Task *mytask;
	uint i;
	IO_DisplayControl *disp_ctrl;

	Initialise_System();

	mytask = System_MultiTask_GetNowTask();

	KBCT = System_MultiTask_Task_Initialise(0);
	KBCT->tss->eip = (uint)&KeyboardControlTask;
	KBCT->tss->cs = SYSTEM_CS << 3;
	KBCT->tss->ss = SYSTEM_DS << 3;
	KBCT->tss->ds = SYSTEM_DS << 3;
	KBCT->tss->esp = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	System_MultiTask_Task_Run(KBCT);

	disp_ctrl = System_Display_Get_Controller();

	i = 0;
	data = 0;

	Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0xffffff, 0, 0, VGA08_VRAM_XSIZE - 1, VGA08_VRAM_YSIZE - 1);
	Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	if(disp_ctrl->display_mode == DISPLAYMODE_VBE_LINEAR){
		Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10 + 16 * 1, 0x000000, "Please Select the VideoMode Number.");
		Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10 + 16 * 2, 0x000000, "(Use cursor Up or Down, Select Enter.)");
		Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 5 - 1);
		snprintf(s, sizeof(s), "%d:0x%X %dx%d-%dbits", i, disp_ctrl->VBE.list_vmode[i].mode_number, disp_ctrl->VBE.list_vmode[i].xsize, disp_ctrl->VBE.list_vmode[i].ysize, disp_ctrl->VBE.list_vmode[i].bpp);
		Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10 + 16 * 3, 0x000000, s);

		FIFO32_Put_Arguments(KBCT->fifo, 4, FIFOCMD_KBCT_SET_FOCUS_FIFO, mytask->fifo, MAIN_KEYBASE, 0);

		for(;;){
			if(FIFO32_MyTaskFIFO_Status() == 0){

			} else{
				data = FIFO32_MyTaskFIFO_Get();
				if(MAIN_KEYBASE <= data && data <= (MAIN_KEYBASE + 0xFFFF)){
					data -= MAIN_KEYBASE;
					if(!(data & KEYID_MASK_BREAK) && (data & KEYID_MASK_EXTENDED)){
						if((data & KEYID_MASK_ID) == KEYID_CURSOR_U){
							if(i == 0){
								i = disp_ctrl->VBE.list_vmode_tags - 1;
							} else{
								i--;
							}
							Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 5 - 1);
							snprintf(s, sizeof(s), "%d:0x%X %dx%d-%dbits", i, disp_ctrl->VBE.list_vmode[i].mode_number, disp_ctrl->VBE.list_vmode[i].xsize, disp_ctrl->VBE.list_vmode[i].ysize, disp_ctrl->VBE.list_vmode[i].bpp);
							Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10 + 16 * 3, 0x000000, s);
						} else if((data & KEYID_MASK_ID) == KEYID_CURSOR_D){
							if(i == disp_ctrl->VBE.list_vmode_tags - 1){
								i = 0;
							} else{
								i ++;
							}
							Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 5 - 1);
							snprintf(s, sizeof(s), "%d:0x%X %dx%d-%dbits", i, disp_ctrl->VBE.list_vmode[i].mode_number, disp_ctrl->VBE.list_vmode[i].xsize, disp_ctrl->VBE.list_vmode[i].ysize, disp_ctrl->VBE.list_vmode[i].bpp);
							Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10 + 16 * 3, 0x000000, s);
						} else if((data & KEYID_MASK_ID) == KEYID_ENTER){
							if(!System_Display_VESA_Set_VideoMode(i)){
								break;
							}
							Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0x00ff00, 10, 10 + 16 * 3, VGA08_VRAM_XSIZE - 10 - 1, 10 + 16 * 4 - 1);
							Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10 + 16 * 4, 0x000000, "Function Failed. Try again.");
						}
					}
				}
			}
		}
	}

	Drawing_Fill_Rectangle(disp_ctrl->vram, disp_ctrl->xsize, 0xffffff, 0, 0, disp_ctrl->xsize - 1, disp_ctrl->ysize - 1);
	Drawing_Put_String(disp_ctrl->vram, disp_ctrl->xsize, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	Format_BMP_DrawPicture(disp_ctrl->vram, disp_ctrl->xsize, 10, 26, 0, 0, chnlogo);

	Drawing_Fill_Circle(disp_ctrl->vram, disp_ctrl->xsize, 100, 250, 0x00c600, 45);
	for(i = 0; i < 50; i += 5){
		Drawing_Draw_Circle(disp_ctrl->vram, disp_ctrl->xsize, 100, 250, 0xc6c6c6, i);
	}
	for(;;){

	}
}

void KeyboardControlTask(void)
{
	UI_Task *mytask;
	uchar s[128];
	uint data, offset;
	DATA_FIFO32 *sendto;
	uint args[5];
	uint i;

	sendto = 0;
	data = 0;
	offset = 0;

	mytask = System_MultiTask_GetNowTask();

	snprintf(s, sizeof(s), "KBCT:KeyboardControlTask Start Running.\nKBCT:UI_Task=0x%X\n", mytask);
	TextMode_Put_String(s, white);

	Keyboard_Set_ReceiveFIFO(mytask->fifo, 0x100);

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){

		} else{
			data = FIFO32_MyTaskFIFO_Get();
			if(0x100 <= data && data <= 0x1ff){
				data -= 0x100;
				data = Keyboard_Decode_KeyCode(data);
				if(sendto != 0 && data != 0){
					FIFO32_Put(sendto, data + offset);
				}
			} else if(data == FIFOCMD_KBCT_SET_FOCUS_FIFO){
				args[0] = FIFOCMD_KBCT_SET_FOCUS_FIFO;
				for(i = 1; i < 5; i++){
					args[i] = FIFO32_MyTaskFIFO_Get();
				}
				if(args[4] == SIGNAL_ARGUMENTS_END){
					sendto = (DATA_FIFO32 *)args[1];
					offset = args[2];
				} else{
					#ifdef CHNOSPROJECT_DEBUG_KBCT
						debug("KBCT:Invalid Arguments to Set Focus FIFO.\n");
					#endif
				}
			}
		}
	}
}
