
#include "core.h"

uint mouse_data0;
DATA_FIFO32 *mouse_fifo;
uint mouse_retv;

uchar Mouse_Pattern_Standard[24][24] = {
	".**.....................",
	"*O**....................",
	"*OO**...................",
	"*OOO**..................",
	"*OOOO**.................",
	"*OOOOO**................",
	"*OOOOOO**...............",
	"*OOOOOOO**..............",
	"*OOOOOOOO**.............",
	"*OOOOOOOOO**............",
	"*OOOOOOOOOO**...........",
	"*OOOOOOOOOOO**..........",
	"*OOOOOOOOOOOO**.........",
	"*OOOOOOOOOOOOO**........",
	"*OOOOOOOOOOOOOO**.......",
	"*OOOOOOOOOOOOOOO**......",
	"*OOOOOOOOOOOOOOOO**.....",
	"*OOOOOO************.....",
	"*OOOOO**................",
	"*OOOO**.................",
	"*OOO**..................",
	"*OO**...................",
	"*O**....................",
	"***.....................",
};

void Initialise_Mouse(void)
{
	mouse_data0 = 0;
	mouse_fifo = Null;

	IO_CLI();
	System_GateDescriptor_Set(0x2c, (uint)asm_InterruptHandler2c, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x0c);
	IO_STI();

	Mouse_SendCommand(MOUSECMD_ENABLE);

	return;
}

void InterruptHandler2c(uint *esp)
{
	uint data;

	data = IO_In8(PORT_KEYDATA);

	ProgrammableInterruptController_InterruptRequest_Complete(0x0c);

	if(mouse_fifo != 0){
		FIFO32_Put(mouse_fifo, data + mouse_data0);
	}

	if(data == KEYDATA_ACK || data == KEYDATA_RESEND){
		mouse_retv = data;
	}

	return;
}

void Mouse_Set_ReceiveFIFO(DATA_FIFO32 *fifo, uint data0)
{
	mouse_data0 = data0;
	mouse_fifo = fifo;

	return; 
}

void Mouse_SendCommand(uint cmd)
{
	KeyboardController_Wait_SendReady();
	IO_Out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);

	for(;;){
		mouse_retv = 0;
		KeyboardController_Wait_SendReady();
		IO_Out8(PORT_KEYDATA, cmd);
		for(;;){
			if(mouse_retv == KEYDATA_ACK){
				return;
			}
			if(mouse_retv == KEYDATA_RESEND){
				break;
			}
		}
	}
	return;
}

UI_MouseCursor *MouseCursor_Initialise(UI_Sheet *parent)
{
	UI_MouseCursor *mcursor;
	uint x, y;

	if(parent == Null){
		return Null;
	}

	mcursor = System_Memory_Allocate(sizeof(UI_MouseCursor));
	mcursor->cursor_sheet = Sheet_Initialise();

	mcursor->cursors[0] = System_Memory_Allocate(24 * 24 * (8 >> 3));
	for(y = 0; y < 24; y++){
		for(x = 0; x < 24; x++){
			if(Mouse_Pattern_Standard[y][x] == '.'){
				(mcursor->cursors[0])[y * 24 + x] = RGB_32_To_08(0x00ff00);
			} else if(Mouse_Pattern_Standard[y][x] == 'O'){
				(mcursor->cursors[0])[y * 24 + x] = RGB_32_To_08(0xffffff);
			} else if(Mouse_Pattern_Standard[y][x] == '*'){
				(mcursor->cursors[0])[y * 24 + x] = RGB_32_To_08(0x000000);
			}
		}
	}

	Sheet_SetBuffer(mcursor->cursor_sheet, mcursor->cursors[0], 24, 24, 8);
	Sheet_SetParent(mcursor->cursor_sheet, parent);
	Sheet_Enable_InvisibleColor(mcursor->cursor_sheet, 0x00ff00);
	Sheet_SetTopmost(mcursor->cursor_sheet, True);
	Sheet_Slide_Absolute(mcursor->cursor_sheet, mcursor->cursor_sheet->parent->size.x >> 1, mcursor->cursor_sheet->parent->size.y >> 1);

	return mcursor;
}

uint MouseCursor_Show(UI_MouseCursor *mcursor)
{
	if(mcursor == Null){
		return 1;
	}

	if(mcursor->cursor_sheet->location.x < 0){
		mcursor->cursor_sheet->location.x = 0;
	} else if((uint)mcursor->cursor_sheet->location.x > mcursor->cursor_sheet->parent->size.x - 1){
		mcursor->cursor_sheet->location.x = mcursor->cursor_sheet->parent->size.x - 1;
	}

	if(mcursor->cursor_sheet->location.y < 0){
		mcursor->cursor_sheet->location.y = 0;
	} else if((uint)mcursor->cursor_sheet->location.y > mcursor->cursor_sheet->parent->size.y - 1){
		mcursor->cursor_sheet->location.y = mcursor->cursor_sheet->parent->size.y - 1;
	}

	Sheet_Show(mcursor->cursor_sheet, SHEET_MAX_CHILDREN, SHEET_LOCATION_NOCHANGE, SHEET_LOCATION_NOCHANGE);

	return 0;
}

uint MouseCursor_Move_Relative(UI_MouseCursor *mcursor, int rpx, int rpy)
{
	if(mcursor == Null){
		return 1;
	}

	rpx += mcursor->cursor_sheet->location.x;
	rpy += mcursor->cursor_sheet->location.y;

	if(rpx < 0){
		rpx = 0;
	} else if((uint)rpx > mcursor->cursor_sheet->parent->size.x - 1){
		rpx = mcursor->cursor_sheet->parent->size.x - 1;
	}

	if(rpy < 0){
		rpy = 0;
	} else if((uint)rpy > mcursor->cursor_sheet->parent->size.y - 1){
		rpy = mcursor->cursor_sheet->parent->size.y - 1;
	}

	Sheet_Slide_Absolute(mcursor->cursor_sheet, rpx, rpy);

	return 0;
}

uint MouseCursor_Move_Absolute(UI_MouseCursor *mcursor, int apx, int apy)
{
	if(mcursor == Null){
		return 1;
	}

	if(apx < 0){
		apx = 0;
	} else if((uint)apx > mcursor->cursor_sheet->parent->size.x - 1){
		apx = mcursor->cursor_sheet->parent->size.x - 1;
	}

	if(apy < 0){
		apy = 0;
	} else if((uint)apy > mcursor->cursor_sheet->parent->size.y - 1){
		apy = mcursor->cursor_sheet->parent->size.y - 1;
	}

	Sheet_Slide_Absolute(mcursor->cursor_sheet, apx, apy);

	return 0;
}
