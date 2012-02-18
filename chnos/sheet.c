
#include "core.h"

//各シートの左上をP、左下をQ、右下をR、右上をS（すべて書き込める座標）とする。

UI_Sheet *Sheet_Initialise(void)
{
	UI_Sheet *sheet;

	sheet = System_Memory_Allocate(sizeof(UI_Sheet));

	sheet->flags.bit.initialized = True;

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_Initialise:[0x%08X]\n", sheet);
	#endif

	return sheet;
}

uint Sheet_Free(UI_Sheet *sheet)
{
	UI_Sheet **search;
	uint i;

	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Free:Null sheet.\n");
		#endif
		return 0;
	}
	if(!sheet->flags.bit.initialized){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Free:Not Initialised sheet.\n");
		#endif
		return 1;
	}

//親への子登録を削除する
	if(sheet->parent != Null){
		search = &sheet->parent->child;
		for(i = 0; i < SHEET_MAX_CHILDREN; i++){
			if(*search == sheet){
				*search = sheet->next;
				sheet->parent = Null;
				break;
			}
			search = &(*search)->next;
		}
		if(i == SHEET_MAX_CHILDREN){
			#ifdef CHNOSPROJECT_DEBUG_SHEET
				debug("Sheet_Free:TRAP:Sheet not found in the link for the parent.\n");
			#endif
			INT_3();
		}
	}

//子への親登録を削除する
	if(sheet->child != Null){
		search = &sheet->child;
		for(i = 0; i < SHEET_MAX_CHILDREN; i++){
			if(search == Null){
				break;
			}
			(*search)->parent = Null;
			search = &(*search)->child;
		}
	}

	return 0;
}

uint Sheet_SetBuffer(UI_Sheet *sheet, void *vram, uint xsize, uint ysize, uint bpp)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.initialized){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Not Initialised sheet.\n");
		#endif
		return 2;
	}
	if(vram == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Null VRAM.\n");
		#endif
		return 3;
	}
	if(xsize > SHEET_MAX_XSIZE){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Too large xsize.\n");
		#endif
		return 4;
	}
	if(ysize > SHEET_MAX_YSIZE){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Too large ysize.\n");
		#endif
		return 5;
	}
	if(bpp != 8 && bpp != 16 && bpp != 32){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Not implemented %d bpp.\n", bpp);
		#endif
		return 6;
	}

	sheet->size.x = xsize;
	sheet->size.y = ysize;
	sheet->bpp = bpp;
	sheet->vram = vram;
	sheet->vramsize = xsize * ysize * bpp;

	sheet->flags.bit.buffer_configured = True;

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_SetBuffer:[0x%08X]\n", sheet);
		debug("Sheet_SetBuffer:vram:[0x%08X] xsize:%d ysize:%d bpp:%d\n", vram, xsize, ysize, bpp);
	#endif

	return 0;
}

uint Sheet_SetParent(UI_Sheet *sheet, UI_Sheet *parent)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetParent:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetParent:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}
	if(parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetParent:Null parent.\n");
		#endif
		sheet->parent = Null;
		
		return 3;
	}
	if(!parent->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetParent:Not buffer_configured parent.\n");
		#endif
		return 4;
	}

	sheet->parent = parent;

	if(!parent->flags.bit.using_map){
		Sheet_Internal_MapInitialise(sheet->parent);
	}

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_SetParent:[0x%08X] parent:[0x%08X]\n", sheet, parent);
	#endif

	return 0;
}

uint Sheet_Show(UI_Sheet *sheet, uint height, int px, int py)
{
	UI_Sheet **search;
	uint i;

	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Show:Null sheet.\n");
		#endif
		return 1;
	}
	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Show:Null parent.\n");
		#endif
		sheet->flags.bit.visible = True;
		if(px != SHEET_LOCATION_NOCHANGE){
			sheet->location.x = px;
		}
		if(py != SHEET_LOCATION_NOCHANGE){
			sheet->location.y = py;
		}
		return 2;
	}

//At First, clear old height link.

	search = &sheet->parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(*search == sheet){
			*search = sheet->next;
			break;
		}
		search = &(*search)->next;
	}

//Next, set new height link.
	search = &sheet->parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(i == height){
			#ifdef CHNOSPROJECT_DEBUG_SHEET
				debug("Sheet_Show:Search:Break(height).\n");
			#endif
			break;
		}
		if(*search == Null){
			#ifdef CHNOSPROJECT_DEBUG_SHEET
				debug("Sheet_Show:Search:Break(End of link).\n");
			#endif
			break;
		}
		search = &(*search)->next;
	}
	if(i == SHEET_MAX_CHILDREN){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Show:Number of sheets is over SHEET_MAX_CHILDREN.\n");
		#endif
		return 3;
	}

	sheet->next = *search;
	*search = sheet;

	sheet->flags.bit.visible = True;
	if(px != SHEET_LOCATION_NOCHANGE && py != SHEET_LOCATION_NOCHANGE){
		Sheet_Slide_Absolute(sheet, px, py);
	} else{
		if(px != SHEET_LOCATION_NOCHANGE){
			Sheet_Slide_Absolute(sheet, px, sheet->location.y);
		}
		if(py != SHEET_LOCATION_NOCHANGE){
			Sheet_Slide_Absolute(sheet, sheet->location.x, py);
		}
		if(px == SHEET_LOCATION_NOCHANGE && py == SHEET_LOCATION_NOCHANGE){
			Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + sheet->size.x - 1, sheet->location.y + sheet->size.y - 1, False);
		}
	}

	Sheet_RefreshSheet_All(sheet);

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_Show:[0x%08X] height:%d\n", sheet, i);
	#endif

	return 0;
}

uint Sheet_RefreshSheet_All(UI_Sheet *sheet)
{
	return Sheet_RefreshSheet(sheet, 0, 0, sheet->size.x - 1, sheet->size.y - 1);
}

uint Sheet_Slide_Absolute(UI_Sheet *sheet, int apx, int apy)
{
	uint retv;

	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Absolute:Null sheet.\n");
		#endif
		return 1;
	}

	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Absolute:Null parent.\n");
		#endif
		return 2;
	}

	if(!sheet->flags.bit.visible){
		sheet->location.x = apx;
		sheet->location.y = apy;
		return 0;
	}

	sheet->flags.bit.visible = False;
	Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + (int)sheet->size.x - 1, sheet->location.y + (int)sheet->size.y - 1, True);
	Sheet_RefreshAllInRange(sheet->parent, sheet->location.x, sheet->location.y, sheet->location.x + (int)sheet->size.x - 1, sheet->location.y + (int)sheet->size.y - 1);

	sheet->location.x = apx;
	sheet->location.y = apy;

	sheet->flags.bit.visible = True;
	Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + (int)sheet->size.x - 1, sheet->location.y + (int)sheet->size.y - 1, False);

	retv = Sheet_RefreshSheet_All(sheet);

	if(retv != 0){
		return retv + 2;
	}

	return 0;
}

uint Sheet_Slide_Relative(UI_Sheet *sheet, int rpx, int rpy)
{
	uint retv;

	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Relative:Null sheet.\n");
		#endif
		return 1;
	}

	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Relative:Null parent.\n");
		#endif
		return 2;
	}

	if(!sheet->flags.bit.visible){
		sheet->location.x += rpx;
		sheet->location.y += rpy;
		return 0;
	}

	sheet->flags.bit.visible = False;

	Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + (int)sheet->size.x - 1, sheet->location.y + (int)sheet->size.y - 1, True);
	Sheet_RefreshAllInRange(sheet->parent, sheet->location.x, sheet->location.y, sheet->location.x + (int)sheet->size.x - 1, sheet->location.y + (int)sheet->size.y - 1);

	sheet->location.x += rpx;
	sheet->location.y += rpy;

	sheet->flags.bit.visible = True;
	Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + (int)sheet->size.x - 1, sheet->location.y + (int)sheet->size.y - 1, False);

	retv = Sheet_RefreshSheet_All(sheet);

	if(retv != 0){
		return retv + 2;
	}

	return 0;
}

uint Sheet_RefreshAllInRange(UI_Sheet *parent, int px0, int py0, int px1, int py1)
{
	uint i;
	UI_Sheet *search;

	search = parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(search == Null){
			break;
		}
		if(Sheet_Internal_IsRangeOverlappedWithSheet(search, px0, py0, px1, py1)){
			Sheet_Internal_RefreshSheet(search, px0, py0, px1, py1);
		}
		search = search->next;
	}

	return 0;
}

uint Sheet_RefreshSheet(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	return Sheet_Internal_RefreshSheet(sheet, px0 + sheet->location.x, py0 + sheet->location.y, px1 + sheet->location.x, py1 + sheet->location.y);
}


