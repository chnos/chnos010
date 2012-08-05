
#include "core.h"

IO_File *File_Initilaize(void)
{
	IO_File *file;

	file = (IO_File *)System_CommonStruct_Allocate(SYSTEM_STRUCTID_FILE);

	file->flags.bit.initialized = True;

	return file;
}

uint File_Free(IO_File *file)
{
	uint i;

	if(file == Null){
		return 1;
	}
	if(file->img != Null){
		System_Memory_Free(file->img, file->size);
	}
	if(file->path != Null){
		for(i = 0; file->path[i] != 0x00; i++){

		}
		System_Memory_Free(file->path, i + 1);
	}

	System_CommonStruct_Free(&file->common_tag);
	return 0;
}
