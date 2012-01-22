
#include "core.h"

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
	if(px != SHEET_LOCATION_NOCHANGE){
		sheet->location.x = px;
	}
	if(py != SHEET_LOCATION_NOCHANGE){
		sheet->location.y = py;
	}

	Sheet_Refresh_Sheet(sheet);

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_Show:[0x%08X] height:%d\n", sheet, i);
	#endif

	return 0;
}

uint Sheet_Refresh_Sheet(UI_Sheet *sheet)
{
	UI_Sheet *now;
	int x, y;
	int apx0, apy0;
	uchar *vram;
	int vxsize, vysize;
	int xsize, ysize;

	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Refresh_Sheet:Null sheet.\n");
		#endif
		return 1;
	}

	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Refresh_Sheet:Null parent.\n");
		#endif
		return 2;
	}

	vram = (uchar *)sheet->parent->vram;
	vxsize = (int)sheet->parent->size.x;
	vysize = (int)sheet->parent->size.y;
	for(now = sheet; now != Null; now = now->next){
		if(now->location.x < vxsize && now->location.y < vysize && now->location.x + (int)now->size.x > 0 && now->location.y + (int)now->size.y > 0 && now->flags.bit.visible){
			if((now->location.x + now->size.x) > vxsize){
				xsize = now->size.x - ((now->location.x + now->size.x) - vxsize);
			} else{
				xsize = now->size.x;
			}
			if((now->location.y + now->size.y) > vysize){
				ysize = now->size.y - ((now->location.y + now->size.y) - vysize);
			} else{
				ysize = now->size.y;
			}
			if(now->location.x < 0){
				xsize += now->location.x;
				apx0 = 0;
			} else{
				apx0 = now->location.x;
			}
			if(now->location.y < 0){
				ysize += now->location.y;
				apy0 = 0;
			} else{
				apy0 = now->location.y;
			}

			#ifdef CHNOSPROJECT_DEBUG_SHEET
				debug("Sheet_Refresh_Sheet:[0x%08X]\n", sheet);
				debug("Sheet_Refresh_Sheet:apx0:%d apy0:%d xsize:%d ysize:%d\n", apx0, apy0, xsize, ysize);
			#endif

			for(y = apy0; y < ysize + apy0; y++){
				for(x = apx0; x < xsize + apx0; x++){
					vram[y * vxsize + x] = ((uchar *)now->vram)[(y - now->location.y) * now->size.x + (x - now->location.x)];
				}
			}
		}
	}

	return 0;
}
