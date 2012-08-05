
/*システム構造体共通管理ルーチン*/

#include "core.h"

System_CommonStruct *System_CommonStruct_Allocate(uint structid)
{
	uint strsize;
	System_CommonStruct *str;

	strsize = 0;
	switch(structid){
		case SYSTEM_STRUCTID_SHEET:
			strsize = sizeof(UI_Sheet);
			break;
		case SYSTEM_STRUCTID_FIFO32:
			strsize = sizeof(DATA_FIFO32);
			break;
		case SYSTEM_STRUCTID_TEXTBOX:
			strsize = sizeof(UI_TextBox);
			break;
		case SYSTEM_STRUCTID_CONSOLE:
			strsize = sizeof(UI_Console);
			break;
		case SYSTEM_STRUCTID_FLOPPYDISK:
			strsize = sizeof(IO_FloppyDisk);
			break;
		case SYSTEM_STRUCTID_FILE:
			strsize = sizeof(IO_File);
			break;
		default:
			#ifdef CHNOSPROJECT_DEBUG_COMMON_STRUCT
				debug("System_CommonStruct_Allocate:Unknown Struct.\n");
			#endif
			return Null;
	}

	str = System_Memory_Allocate(strsize);
	str->structid = structid;
	str->structsize = strsize;

	return str;
}

uint System_CommonStruct_Free(System_CommonStruct *str)
{
	switch(str->structid){
		case SYSTEM_STRUCTID_SHEET:
		case SYSTEM_STRUCTID_FIFO32:
		case SYSTEM_STRUCTID_TEXTBOX:
		case SYSTEM_STRUCTID_CONSOLE:
		case SYSTEM_STRUCTID_FLOPPYDISK:
		case SYSTEM_STRUCTID_FILE:
			break;
		default:
			#ifdef CHNOSPROJECT_DEBUG_COMMON_STRUCT
				debug("System_CommonStruct_Free:Unknown Struct.\n");
			#endif
		return 1;
	}
	str->structid = SYSTEM_STRUCTID_UNDEFINED;
	System_Memory_Free(str, str->structsize);
	return 0;
}

