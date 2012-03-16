
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

//親の子登録を削除する
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

//子の親登録を削除する
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

	if(sheet->flags.bit.vram_auto_allocated){
		System_Memory_Free(sheet->vram, sheet->vramsize);
	}

	System_Memory_Free(sheet, sizeof(UI_Sheet));

	return 0;
}

//vram==Null:Auto allocate
uint Sheet_SetBuffer(UI_Sheet *sheet, void *vram, uint xsize, uint ysize, uint bpp)
{
	uint retv;

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

	retv = 0;
	if(bpp == 8){
		retv = Sheet08_Internal_SetBuffer(sheet, vram, xsize, ysize, bpp);
	} else if(bpp == 16){
		retv = Sheet16_Internal_SetBuffer(sheet, vram, xsize, ysize, bpp);
	} else if(bpp == 32){
		retv = Sheet32_Internal_SetBuffer(sheet, vram, xsize, ysize, bpp);
	} else{
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Not implemented %d bpp. Abort.\n", bpp);
		#endif
		INT_3();
	}

	if(retv != 0){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_SetBuffer:Function Failed.\n");
		#endif
		return 10 + retv;
	}

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_SetBuffer:[0x%08X]\n", sheet);
		debug("Sheet_SetBuffer:vram:[0x%08X] xsize:%d ysize:%d bpp:%d\n", vram, xsize, ysize, bpp);
	#endif

	return 0;
}

uint Sheet_SetParent(UI_Sheet *sheet, UI_Sheet *parent)
{
	uint retv;

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

	retv = sheet->Config_Functions(sheet);

	if(retv != 0){
		return 10 + retv;
	}

	return 0;
}

uint Sheet_Show(UI_Sheet *sheet, uint height, int px, int py)
{
	UI_Sheet **search;
	uint i;
	bool no_change_height;

	no_change_height = False;

	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Show:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Show:Not buffer_configured sheet.\n");
		#endif
		return 2;
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
		return 3;
	}
	if(sheet->flags.bit.visible == True){
		return 4;
	}

//At First, clear old height link.

	search = &sheet->parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(*search == sheet){
			if(i == height){
				no_change_height = True;
				break;
			}
			*search = sheet->next;
			break;
		}
		search = &(*search)->next;
	}

//Next, set new height link.
	if(!no_change_height){
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
	}

	if(px != SHEET_LOCATION_NOCHANGE){
		sheet->location.x = px;
	}
	if(py != SHEET_LOCATION_NOCHANGE){
		sheet->location.y = py;
	}
	sheet->flags.bit.visible = True;
	Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + sheet->size.x - 1, sheet->location.y + sheet->size.y - 1);

	Sheet_RefreshSheet_All(sheet);

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_Show:[0x%08X] height:%d\n", sheet, i);
	#endif

	return 0;
}

uint Sheet_RefreshSheet_All(UI_Sheet *sheet)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshSheet_All:Null sheet.\n");
		#endif
		return 1;
	}

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
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Absolute:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}
	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Absolute:Null parent.\n");
		#endif
		return 3;
	}
	if(!sheet->flags.bit.visible){
		sheet->location.x = apx;
		sheet->location.y = apy;
		return 0;
	}

	retv = Sheet_Internal_SlideSub(sheet, apx - sheet->location.x, apy - sheet->location.y);

	if(retv != 0){
		return 10 + retv;
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
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Relative:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}
	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Slide_Relative:Null parent.\n");
		#endif
		return 3;
	}
	if(!sheet->flags.bit.visible){
		sheet->location.x += rpx;
		sheet->location.y += rpy;
		return 0;
	}

	retv = Sheet_Internal_SlideSub(sheet, rpx, rpy);

	if(retv != 0){
		return 10 + retv;
	}

	return 0;
}

uint Sheet_RefreshAllInRange(UI_Sheet *parent, int px0, int py0, int px1, int py1)
{
	uint i;
	UI_Sheet *search;

	if(parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshAllInRange:Null parent.\n");
		#endif
		return 1;
	}
	if(!parent->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshAllInRange:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}

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
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshSheet:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshSheet:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}

	return Sheet_Internal_RefreshSheet(sheet, px0 + sheet->location.x, py0 + sheet->location.y, px1 + sheet->location.x, py1 + sheet->location.y);
}

uint Sheet_RefreshMap(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshMap:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshMap:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}

	return Sheet_Internal_MapRefresh(sheet, px0 + sheet->location.x, py0 + sheet->location.y, px1 + sheet->location.x, py1 + sheet->location.y);
}

uint Sheet_RefreshMap_All(UI_Sheet *sheet)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_RefreshMap_All:Null sheet.\n");
		#endif
		return 1;
	}

	return Sheet_RefreshMap(sheet, 0, 0, sheet->size.x - 1, sheet->size.y - 1);
}

uint Sheet_Enable_InvisibleColor(UI_Sheet *sheet, uint invcol)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Enable_InvisibleColor:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Enable_InvisibleColor:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}

	if(sheet->bpp == 8){
		sheet->invcol = RGB_32_To_08(invcol);
		sheet->IsVisiblePixel = &Sheet08_Internal_IsVisiblePixel;
	} else if(sheet->bpp == 16){
		sheet->invcol = RGB_32_To_16(invcol);
		sheet->IsVisiblePixel = &Sheet16_Internal_IsVisiblePixel;
	} else if(sheet->bpp == 32){
		sheet->invcol = invcol;
		sheet->IsVisiblePixel = &Sheet32_Internal_IsVisiblePixel;
	} else{
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Enable_InvisibleColor:Not implemented invisible-color in %d bpp.\n", sheet->bpp);
		#endif
		return 3;
	}
	sheet->flags.bit.using_invcol = True;
	return 0;
}

uint Sheet_Disable_InvisibleColor(UI_Sheet *sheet)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Disable_InvisibleColor:Null sheet.\n");
		#endif
		return 1;
	}
	if(!sheet->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Disable_InvisibleColor:Not buffer_configured sheet.\n");
		#endif
		return 2;
	}
	sheet->flags.bit.using_invcol = False;
	sheet->IsVisiblePixel = &Sheet_Internal_IsVisiblePixel_Invalid;
	return sheet->invcol;
}
