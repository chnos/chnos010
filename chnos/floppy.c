
#include "core.h"

IO_FloppyDisk *FloppyDisk_Initialize(void *img)
{
	IO_FloppyDisk *fd;

	fd = (IO_FloppyDisk *)System_CommonStruct_Allocate(SYSTEM_STRUCTID_FLOPPYDISK);

	fd->img = img;
	fd->files = (IO_FloppyDisk_RootDirectoryEntry *)(fd->img + FLOPPYDISK_RDE_OFFSET);
	fd->userdataarea = (sector *)(fd->img + FLOPPYDISK_RDE_OFFSET + sizeof(IO_FloppyDisk_RootDirectoryEntry) * FLOPPYDISK_RDE_ENTRIES);

//	fd->fat = MemoryBlock_Allocate_System(2 * 2880);
//	MemoryBlock_Write_Description(fd->fat, "IO_FD_FAT");
//	FloppyDisk_Decode_FAT16(fd->img, fd->fat, false);

	return fd;
}
