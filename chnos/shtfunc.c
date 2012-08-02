
#include "core.h"

//シート制御用内部関数。sheet.c以外からは呼び出し禁止。
//その代わり、シート有効性チェックはある程度省く。
//呼び出し側の関数で、シートが有効であることを確認してから渡すこと。

//uint Sheet_Internal_GetLocation[P,Q,R,S](UI_Sheet *sheet, DATA_Location2D *dest);
//書き込み可能なもっとも端の各座標を親シートでの座標で求める。destに座標を格納。
//親がないシートの場合は、戻り値が1になり、locationが(0,0)であると仮定される。
//親のあるシートの場合は、戻り値が0になり、各座標が格納される。

uint Sheet_Internal_GetLocationP(UI_Sheet *sheet, DATA_Location2D *dest)
{
	if(sheet->parent == Null){
		dest->x = 0;
		dest->y = 0;
		return 1;
	}
	dest->x = sheet->location.x;
	dest->y = sheet->location.y;

	return 0;
}

uint Sheet_Internal_GetLocationQ(UI_Sheet *sheet, DATA_Location2D *dest)
{
	if(sheet->parent == Null){
		dest->x = 0;
		dest->y = (int)sheet->size.y - 1;
		return 1;
	}
	dest->x = sheet->location.x;
	dest->y = sheet->location.y + (int)sheet->size.y - 1;

	return 0;
}

uint Sheet_Internal_GetLocationR(UI_Sheet *sheet, DATA_Location2D *dest)
{
	if(sheet->parent == Null){
		dest->x = (int)sheet->size.x - 1;
		dest->y = (int)sheet->size.y - 1;
		return 1;
	}
	dest->x = sheet->location.x + (int)sheet->size.x - 1;
	dest->y = sheet->location.y + (int)sheet->size.y - 1;

	return 0;
}

uint Sheet_Internal_GetLocationS(UI_Sheet *sheet, DATA_Location2D *dest)
{
	if(sheet->parent == Null){
		dest->x = (int)sheet->size.x - 1;
		dest->y = 0;
		return 1;
	}
	dest->x = sheet->location.x + (int)sheet->size.x - 1;
	dest->y = sheet->location.y;

	return 0;
}

//uint Sheet_Internal_MapInitialize(UI_Sheet *parent);
//mapを初期化する。
//すでにマップが確保されていた場合は、解放した後に再確保する。
//マップの再構成も自動で行う。

uint Sheet_Internal_MapInitialize(UI_Sheet *parent)
{
	if(parent->flags.bit.using_map){
		parent->flags.bit.using_map = False;
		System_Memory_Free(parent->map, parent->mapsize);
	}

	parent->mapsize = parent->size.x * parent->size.y * 4;
	parent->map = (uint *)System_Memory_Allocate(parent->mapsize);
	parent->flags.bit.using_map = True;

	Sheet_Internal_MapRebuild(parent, 0, 0, parent->size.x - 1, parent->size.y - 1);

	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_Internal_MapInitialize:[0x%08X] map:[0x%08X]\n", parent, parent->map);
	#endif

	return 0;
}

//uint Sheet_Internal_Map[Write, Clear]FromSheet(UI_Sheet *sheet, bool force);
//各シートの範囲内のマップを生成する。
//forceがFalseの時は、mapのNullまたは自分のアドレスの部分しかいじらない。
//forceがTrueの時は、mapの範囲内に強制的に書き込む。

uint Sheet_Internal_MapWriteFromSheet(UI_Sheet *sheet, bool force, int px0, int py0, int px1, int py1)
{
	int x, y;
	DATA_Location2D p, r;

	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_MapWriteFromSheet:Null parent.\n");
		#endif
		return 1;
	}

	if(sheet->parent->map == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_MapWriteFromSheet:Null map.\n");
		#endif
		return 2;
	}

	if(!sheet->flags.bit.visible){
		return 0;
	}

	Sheet_Internal_GetLocationP(sheet, &p);
	Sheet_Internal_GetLocationR(sheet, &r);

	if(p.x < px0){
		p.x = px0;
	}
	if(p.y < py0){
		p.y = py0;
	}
	if(r.x > px1){
		r.x = px1;
	}
	if(r.y > py1){
		r.y = py1;
	}

	if(p.x < 0){
		p.x = 0;
	}
	if(p.y < 0){
		p.y = 0;
	}
	if(r.x >= (int)sheet->parent->size.x){
		r.x = (int)sheet->parent->size.x - 1;
	}
	if(r.y >= (int)sheet->parent->size.y){
		r.y = (int)sheet->parent->size.y - 1;
	}

	if(force){
		if(sheet->flags.bit.using_invcol){
			for(y = p.y; y <= r.y; y++){
				for(x = p.x; x <= r.x; x++){
					if(sheet->IsVisiblePixel(sheet, x, y)){
						sheet->parent->map[y * sheet->parent->size.x + x] = (uint)sheet;
					}
				}
			}
		} else{
			for(y = p.y; y <= r.y; y++){
				for(x = p.x; x <= r.x; x++){
					sheet->parent->map[y * sheet->parent->size.x + x] = (uint)sheet;
				}
			}
		}
	} else{
		if(sheet->flags.bit.using_invcol){
			for(y = p.y; y <= r.y; y++){
				for(x = p.x; x <= r.x; x++){
					if(sheet->parent->map[y * sheet->parent->size.x + x] == Null && sheet->IsVisiblePixel(sheet, x, y)){
						sheet->parent->map[y * sheet->parent->size.x + x] = (uint)sheet;
					}
				}
			}
		} else{
			for(y = p.y; y <= r.y; y++){
				for(x = p.x; x <= r.x; x++){
					if(sheet->parent->map[y * sheet->parent->size.x + x] == Null){
						sheet->parent->map[y * sheet->parent->size.x + x] = (uint)sheet;
					}
				}
			}
		}
	}

	return 0;
}

uint Sheet_Internal_MapClearFromSheet(UI_Sheet *sheet, bool force, int px0, int py0, int px1, int py1)
{
	int x, y;
	DATA_Location2D p, r;

	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_MapClearFromSheet:Null parent.\n");
		#endif
		return 1;
	}

	if(sheet->parent->map == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_MapClearFromSheet:Null map.\n");
		#endif
		return 2;
	}

	Sheet_Internal_GetLocationP(sheet, &p);
	Sheet_Internal_GetLocationR(sheet, &r);

	if(p.x < px0){
		p.x = px0;
	}
	if(p.y < py0){
		p.y = py0;
	}
	if(r.x > px1){
		r.x = px1;
	}
	if(r.y > py1){
		r.y = py1;
	}

	if(p.x < 0){
		p.x = 0;
	}
	if(p.y < 0){
		p.y = 0;
	}
	if(r.x >= (int)sheet->parent->size.x){
		r.x = (int)sheet->parent->size.x - 1;
	}
	if(r.y >= (int)sheet->parent->size.y){
		r.y = (int)sheet->parent->size.y - 1;
	}

	if(force){
		for(y = p.y; y <= r.y; y++){
			for(x = p.x; x <= r.x; x++){
				sheet->parent->map[y * sheet->parent->size.x + x] = Null;
			}
		}
	} else{
		for(y = p.y; y <= r.y; y++){
			for(x = p.x; x <= r.x; x++){
				if(sheet->parent->map[y * sheet->parent->size.x + x] == (uint)sheet){
					sheet->parent->map[y * sheet->parent->size.x + x] = Null;
				}
			}
		}
	}

	return 0;
}

//bool Sheet_Internal_IsLocationInRangeOfSheet(UI_Sheet *sheet, int px, int py);
//そのシートが、そのシートの親内部での座標である(px, py)を含むかどうかを返す。
//シートが無効でも、Falseを返すだけである。
//親がいないシートでこの関数を実行しても無意味だと思われるので、Falseを返すだけである。

bool Sheet_Internal_IsLocationInRangeOfSheet(UI_Sheet *sheet, int px, int py)
{
	if(sheet == Null){
		return False;
	}
	if(sheet->parent == Null){
		return False;
	}

	if(sheet->location.x > px || sheet->location.y > py){
		return False;
	}
	if(sheet->location.x + (int)sheet->size.x - 1 < px || sheet->location.y + (int)sheet->size.y - 1 < py){
		return False;
	}

	return True;
}

//bool Sheet_Internal_IsRangeOverlappedWithSheet(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
//指定されたシートが引数の座標範囲内に重なるかどうかを返す。

bool Sheet_Internal_IsRangeOverlappedWithSheet(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	if(sheet == Null){
		return False;
	}
	if(sheet->parent == Null){
		return False;
	}

	if(sheet->location.x > px1){
		return False;
	}

	if(sheet->location.y > py1){
		return False;
	}

	if(sheet->location.x + (int)sheet->size.x - 1 < px0){
		return False;
	}

	if(sheet->location.y + (int)sheet->size.y - 1 < py0){
		return False;
	}

	return True;
}

//UI_Sheet *Sheet_Internal_GetSheetFromLocation(UI_Sheet *parent, int px, int py)
//親内部での(px, py)における、可視状態のもっとも高い高さのシートを返す。
//この関数はきっと遅いので、mapが利用できない時のみ、使用するべきである。
//エラーが発生した時は、Nullを返すだけである。

UI_Sheet *Sheet_Internal_GetSheetFromLocation(UI_Sheet *parent, int px, int py)
{
	uint i;
	UI_Sheet *sheet, *search;

	sheet = Null;

	search = parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(search == Null){
			break;
		}
		if(Sheet_Internal_IsLocationInRangeOfSheet(search, px, py) && search->flags.bit.visible){
			sheet = search;
		}
		search = search->next;
	}

	return sheet;
}

//uint Sheet_Internal_MapRebuild(UI_Sheet *parent, int px0, int py0, int px1, int py1);
//その範囲内のマップを完全に再構成する。
uint Sheet_Internal_MapRebuild(UI_Sheet *parent, int px0, int py0, int px1, int py1)
{
	uint i;
	UI_Sheet *search;

	search = parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(search == Null){
			break;
		}
		if(Sheet_Internal_IsRangeOverlappedWithSheet(search, px0, py0, px1, py1)){
			Sheet_Internal_MapWriteFromSheet(search, True, px0, py0, px1, py1);
		}
		search = search->next;
	}

	return 0;
}

//uint Sheet_Internal_MapRefresh(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
//その範囲内のマップを、sheetに変更があったとして、最小限で再構成する。
//座標はmap(parent)なので注意。
//sheetの高さまではすべて再構成する。
//同時に、更新したシートに対して再描画をかける。
uint Sheet_Internal_MapRefresh(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	UI_Sheet *search;
	//int x, y;
	uint i;
	UI_Sheet **write_sheet;
	uint write_sheets;

	write_sheet = (UI_Sheet **)System_Memory_Allocate(4 * SHEET_MAX_CHILDREN);

	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= (int)sheet->parent->size.x){
		px1 = (int)sheet->parent->size.x - 1;
	}
	if(py1 >= (int)sheet->parent->size.y){
		py1 = (int)sheet->parent->size.y - 1;
	}

	write_sheets = 0;

	search = sheet->parent->child;
	for(i = 0; i < SHEET_MAX_CHILDREN; i++){
		if(search == Null){
			break;
		}
		if(Sheet_Internal_IsRangeOverlappedWithSheet(search, px0, py0, px1, py1)){
			Sheet_Internal_MapClearFromSheet(search, False, px0, py0, px1, py1);
			write_sheet[write_sheets] = search;
			write_sheets++;
		}
		if(search == sheet){
			break;
		}
		search = search->next;
	}


	for(; write_sheets != 0; ){
		write_sheets--;
		Sheet_Internal_MapWriteFromSheet(write_sheet[write_sheets], False, px0, py0, px1, py1);
		Sheet_Internal_RefreshSheet(write_sheet[write_sheets], px0, py0, px1, py1);
	}

/*
	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(sheet->parent->map[y * sheet->parent->size.x + x] == Null){
				search = Sheet_Internal_GetSheetFromLocation(sheet->parent, x, y);
				if(search != Null){
					Sheet_Internal_MapWriteFromSheet(search, False, px0, py0, px1, py1);
				}
			}
		}
	}
*/

	System_Memory_Free(write_sheet, 4 * SHEET_MAX_CHILDREN);

	return 0;
}

//uint Sheet_Internal_RefreshSheet(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
//親グローバル座標でのシートvramリフレッシュ。
uint Sheet_Internal_RefreshSheet(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	uint retv;

	#ifdef CHNOSPROJECT_DEBUG_CALLLINK
		debug("Sheet_Internal_RefreshSheet:Called from[0x%08X].\n", *((uint *)(&sheet - 1)));
	#endif

	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_RefreshSheet:[0x%08X]Null parent.\n", sheet);
		#endif
		return 1;
	}

	if(!sheet->parent->flags.bit.buffer_configured){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_RefreshSheet:[0x%08X]Not buffer_configured parent.\n", sheet);
		#endif
		return 2;
	}

	if(sheet->RefreshSheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_RefreshSheet:[0x%08X]Null Refresh function.\n", sheet);
		#endif
		return 3;
	}

	if(!sheet->flags.bit.visible){
		return 0;
	}

	retv = sheet->RefreshSheet(sheet, px0, py0, px1, py1);

	if(sheet->flags.bit.autorefresh_upperlevel && sheet->parent->parent != Null){
		//無限再帰を防ぐため、一時的に上位階層オートリフレッシュを無効にする
		sheet->flags.bit.autorefresh_upperlevel = False;
		Sheet_RefreshSheet(sheet->parent, px0, py0, px1, py1);
		sheet->flags.bit.autorefresh_upperlevel = True;
	}

	if(retv != 0){
		return 10 + retv;
	}

	return 0;
}

//uint Sheet_Internal_SlideSub(UI_Sheet *sheet, int rpx, int rpy);
//slide高速化サブ関数。シートの座標も変更する。引数は、移動量を示す符号あり整数。
uint Sheet_Internal_SlideSub(UI_Sheet *sheet, int rpx, int rpy)
{
	int xsize, ysize;
	DATA_Location2D A, B;
	int apx, apy;

	#ifdef CHNOSPROJECT_DEBUG_CALLLINK
		debug("Sheet_Internal_SlideSub:Called from[0x%08X].\n", *((uint *)(&sheet - 1)));
	#endif

	xsize = (int)sheet->size.x;
	ysize = (int)sheet->size.y;

	if(rpx < 0){
		apx = -rpx;
	} else{
		apx = rpx;
	}

	if(rpy < 0){
		apy = -rpy;
	} else{
		apy = rpy;
	}

	sheet->flags.bit.visible = False;

	if(apx >= xsize || apy >= ysize){
		Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + xsize - 1, sheet->location.y + ysize - 1);
		sheet->location.x += rpx;
		sheet->location.y += rpy;
		sheet->flags.bit.visible = True;
		Sheet_Internal_MapRefresh(sheet, sheet->location.x, sheet->location.y, sheet->location.x + xsize - 1, sheet->location.y + ysize - 1);

		Sheet_RefreshSheet_All(sheet);
		return 0;
	}

	if(rpx == 0){
		//x方向に移動しない
		if(rpy < 0){
			//yの負の方向に移動=上方向
			A.x = sheet->location.x;
			A.y = sheet->location.y + ysize - apy;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.y -= ysize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
		} else if(rpy > 0){
			//yの正の方向に移動=下方向
			A.x = sheet->location.x;
			A.y = sheet->location.y;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.y += ysize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
		} else{
			//y方向にも移動しない=そのまま
			sheet->flags.bit.visible = True;
		}
	} else if(rpx < 0){
		//xの負の方向に移動
		if(rpy < 0){
			//yの負の方向に移動=左上方向
			A.x = sheet->location.x;
			A.y = sheet->location.y + ysize - apy;
			B.x = sheet->location.x + xsize - apx;
			B.y = sheet->location.y;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.x -= apx;
			A.y -= ysize;
			B.x -= xsize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
		} else if(rpy > 0){
			//yの正の方向に移動=左下方向
			A.x = sheet->location.x;
			A.y = sheet->location.y;
			B.x = sheet->location.x + xsize - apx;
			B.y = sheet->location.y + apy;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.x -= apx;
			A.y += ysize;
			B.x -= xsize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
		} else{
			//y方向に移動しない=左方向
			A.x = sheet->location.x + xsize - apx;
			A.y = sheet->location.y;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + apx - 1, A.y + ysize - 1);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.x -= xsize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + apx - 1, A.y + ysize - 1);
		}
	} else{
		//xの正の方向に移動
		if(rpy < 0){
			//yの負の方向に移動=右上方向
			A.x = sheet->location.x;
			A.y = sheet->location.y + ysize - apy;
			B.x = sheet->location.x;
			B.y = sheet->location.y;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.x += apx;
			A.y -= ysize;
			B.x += xsize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
		} else if(rpy > 0){
			//yの正の方向に移動=右下方向
			A.x = sheet->location.x;
			A.y = sheet->location.y;
			B.x = sheet->location.x;
			B.y = sheet->location.y + apy;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.x += apx;
			A.y += ysize;
			B.x += xsize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + xsize - 1, A.y + apy - 1);
			Sheet_Internal_MapRefresh(sheet, B.x, B.y, B.x + apx - 1, B.y + ysize - apy);
		} else{
			//y方向に移動しない=右方向
			A.x = sheet->location.x;
			A.y = sheet->location.y;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + apx - 1, A.y + ysize - 1);
			sheet->location.x += rpx;
			sheet->location.y += rpy;
			sheet->flags.bit.visible = True;
			A.x += xsize;
			Sheet_Internal_MapRefresh(sheet, A.x, A.y, A.x + apx - 1, A.y + ysize - 1);
		}
	}

	Sheet_RefreshMap_All(sheet);
	return 0;
}

uint Sheet_Internal_ChangeHeight(UI_Sheet *sheet, uint height)
{
	UI_Sheet **search;
	uint i;

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
	if(!sheet->flags.bit.topmost){
		search = &sheet->parent->child;
		for(i = 0; i < SHEET_MAX_CHILDREN; i++){
			if(i == height){
				#ifdef CHNOSPROJECT_DEBUG_SHEET
					debug("Sheet_Internal_ChangeHeight:Search:Break(height).\n");
				#endif
				break;
			}
			if(*search == Null){
				#ifdef CHNOSPROJECT_DEBUG_SHEET
					debug("Sheet_Internal_ChangeHeight:Search:Break(End of link).\n");
				#endif
				break;
			}
			if((*search)->flags.bit.topmost){
				#ifdef CHNOSPROJECT_DEBUG_SHEET
					debug("Sheet_Internal_ChangeHeight:Search:Break(Under topmost sheet).\n");
				#endif
				break;
			}
			search = &(*search)->next;
		}
	} else{	/*topmost sheet. ignore height.*/
		search = &sheet->parent->child;
		for(i = 0; i < SHEET_MAX_CHILDREN; i++){
			if(*search == Null){
				#ifdef CHNOSPROJECT_DEBUG_SHEET
					debug("Sheet_Internal_ChangeHeight:Search:Break(Top most).\n");
				#endif
				break;
			}
			search = &(*search)->next;
		}
	}
	if(i == SHEET_MAX_CHILDREN){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet_Internal_ChangeHeight:Number of sheets is over SHEET_MAX_CHILDREN.\n");
		#endif
		return 3;
	}
	sheet->next = *search;
	*search = sheet;

	return 0;
}

//SheetXX_Internal_IsVisiblePixel(UI_Sheet *sheet, int px, int py)
//親シート内座標における、指定されたシートのピクセル(px, py)が可視状態であるかどうかを返す。
//引数チェックはすべて省略しているので、呼び出し元で厳密にチェックする必要がある。

//下の関数は、透明色モードオフ時に指定される、ダミー関数である。
bool Sheet_Internal_IsVisiblePixel_Invalid(UI_Sheet *sheet, int px, int py)
{
	return True;
}

uint Sheet_Internal_RefreshSheet_Invalid(struct UI_SHEET *sheet, int px0, int py0, int px1, int py1)
{
	#ifdef CHNOSPROJECT_DEBUG_SHEET
		debug("Sheet_Internal_RefreshSheet_Invalid:Invalid refresh request.\n");
	#endif
	return 0;
}
