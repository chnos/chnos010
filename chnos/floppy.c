
#include "core.h"

IO_FloppyDisk *FloppyDisk_Initialize(void *img)
{
	IO_FloppyDisk *fd;

	fd = (IO_FloppyDisk *)System_CommonStruct_Allocate(SYSTEM_STRUCTID_FLOPPYDISK);

	fd->img = img;
	fd->files = (IO_FloppyDisk_DirectoryEntry *)(fd->img + FLOPPYDISK_RDE_OFFSET);
	fd->userdataarea = (sector *)(fd->img + FLOPPYDISK_RDE_OFFSET + sizeof(IO_FloppyDisk_DirectoryEntry) * FLOPPYDISK_RDE_ENTRIES);
//fat16
	fd->fat = System_Memory_Allocate(FLOPPYDISK_SECTORS * sizeof(ushort));
	FloppyDisk_Decode_FAT16(fd, False);

	return fd;
}

uint FloppyDisk_Decode_FAT16(IO_FloppyDisk *fd, bool backup)
{
	uint i;
	uchar *fatimg;

	i = 0;

	if(fd == Null){
		return 1;
	}
	if(fd->img == Null){
		return 2;
	}
	if(fd->fat == Null){
		return 3;
	}

	if(backup){
		fatimg = fd->img + FLOPPYDISK_FAT1_OFFSET;
	} else{
		fatimg = fd->img + FLOPPYDISK_FAT0_OFFSET;
	}

	for(i = 0; i < 2880; i += 2){
		fd->fat[i + 0] = (fatimg[(i >> 1) * 3 + 0] | fatimg[(i >> 1) * 3 + 1] << 8) & 0xfff;
		fd->fat[i + 1] = (fatimg[(i >> 1) * 3 + 1] >> 4 | fatimg[(i >> 1) * 3 + 2] << 4) & 0xfff;
	}
	return 0;
}

bool FloppyDisk_IsPathExist(IO_FloppyDisk *fd, const uchar path[])
{
	if(fd == Null){
		return False;
	}

	if(FloppyDisk_Internal_GetDirectoryEntryFromPath(fd, path) != Null){
		return True;
	}

	return False;
}

uint FloppyDisk_LoadFile(IO_FloppyDisk *fd, IO_File *file, const uchar path[])
{
	IO_FloppyDisk_DirectoryEntry *dir_entry;
	uint i, now_size;
	ushort now_cluster;

	if(fd == Null){
		return 1;
	}
	if(fd->fat == Null){
		return 2;
	}
	if(file == Null){
		return 3;
	}
	if(!file->flags.bit.initialized){
		return 4;
	}

	dir_entry = FloppyDisk_Internal_GetDirectoryEntryFromPath(fd, path);

	if(dir_entry == Null){
		return 5;
	}

	for(i = 0; i < FILE_PATH_LENGTH_MAX; i++){
		if(path[i] == 0x00){
			break;
		}
	}

	file->path = System_Memory_Allocate(i + 1);
	CFunction_MemoryMove(file->path, i + 1, path, i);
	file->path[i] = 0x00;

	file->img = System_Memory_Allocate(dir_entry->size);
	file->size = dir_entry->size;

	now_cluster = dir_entry->cluster;
	now_size = dir_entry->size;

	for(i = 0; ; i++){
		if(now_size <= 512){
			CFunction_MemoryMove(&((uchar *)file->img)[i * sizeof(sector)], file->size - (i * sizeof(sector)), &fd->userdataarea[now_cluster - 2], now_size);
			break;
		}
		CFunction_MemoryMove(&((uchar *)file->img)[i * sizeof(sector)], file->size - (i * sizeof(sector)), &fd->userdataarea[now_cluster - 2], sizeof(sector));
		now_size -= sizeof(sector);
		now_cluster = fd->fat[now_cluster];
	}

	file->flags.bit.img_loaded = True;

	return 0;
}

IO_FloppyDisk_DirectoryEntry *FloppyDisk_Internal_GetDirectoryEntryFromPath(IO_FloppyDisk *fd, const uchar path[])
{
	uchar filename[11];
	uint i, j;
	IO_FloppyDisk_DirectoryEntry_Attribute rdeattr;

	if(fd == Null){
		return Null;
	}

	for(i = 0; i < 11; i++){
		filename[i] = ' ';
	}

	j = 0;
	for(i = 0; i < 11; i++){
		if(j >= 11){
			break;
		}
		if(path[i] == 0x00){
			break;
		}
		if(path[i] == '.'){
			j = 8;
		} else{
			filename[j] = path[i];
			if('a' <= filename[j] && filename[j] <= 'z'){
				filename[j] -= 0x20;
			}
			j++;
		}
	}

	for(i = 0; i < FLOPPYDISK_RDE_ENTRIES; i++){
		if(fd->files[i].name[0] == 0x00){
			break;
		}
		rdeattr.attribute = fd->files[i].attribute;
		if(fd->files[i].name[0] != 0xe5 && fd->files[i].name[0] != 0x05 && !rdeattr.bit.volumelabel && !rdeattr.bit.directory){
			if(CFunction_CompareStrings_n(fd->files[i].name, filename, 11)){
				return &fd->files[i];
			}
		}
	}
	return Null;
}

