
#include "core.h"

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

void Mouse_SendCommand(uint data)
{
	KeyboardController_SendCommand(KEYCMD_SENDTO_MOUSE);
	KeyboardController_SendData(data);
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

uint MouseCursor_Move_Relative(UI_MouseCursor *mcursor, int apx, int apy)
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
