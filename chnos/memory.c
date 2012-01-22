
#include "core.h"

uint Memory_Test(uint start, uint end)
{
	uchar flg486 = 0;
	uint i;
	CPU_EFlags eflg;
	CPU_ControlRegister0 cr0;

	eflg.eflags = IO_Load_EFlags();
	eflg.bit.AC = True;
	IO_Store_EFlags(eflg.eflags);

	eflg.eflags = IO_Load_EFlags();
	if(eflg.bit.AC){
		flg486 = 1;
	}
	eflg.bit.AC = False;
	IO_Store_EFlags(eflg.eflags);

	if(flg486 != 0){
		cr0.cr0 = Load_CR0();
		cr0.bit.NW = True;
		cr0.bit.CD = True;
		Store_CR0(cr0.cr0);
	}
	i = Memory_Test_Sub(start, end);
	if(flg486 != 0){
		cr0.cr0 = Load_CR0();
		cr0.bit.NW = False;
		cr0.bit.CD = False;
		Store_CR0(cr0.cr0);
	}
	return i;
}

IO_MemoryControl Memory_Initialise_Control(void *start, uint size, uint tags)
{
	IO_MemoryControl ctrl;

	size = (size + 7) & ~7;

	if((tags * sizeof(IO_MemoryControlTag)) > size){
		return 0;
	}

	if(tags < 3){
		return 0;
	}

	ctrl = (IO_MemoryControl)(start + (size - (tags * sizeof(IO_MemoryControlTag))));
	ctrl[0].addr = 0;
	ctrl[0].size = tags;

	ctrl[1].addr = start;
	ctrl[1].size = size - (tags * sizeof(IO_MemoryControlTag));

	ctrl[2].addr = 0;
	ctrl[2].size = 0xffffffff;

	return ctrl;
}

void Memory_Free(IO_MemoryControl ctrl, void *addr, uint size)
{
	uint i, j, k, minsize;
	uint eflags;

	size = (size + 7) & ~7;

#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryFree:Start ctrl:0x%08X addr:0x%08X size 0x%08X\n", ctrl, addr, size);
#endif

	if(size == 0){
		return;
	}

	eflags = IO_Load_EFlags();

	IO_CLI();

	j = 0;
	k = 0;
	minsize = 0xffffffff;
	for(i = 1; i < ctrl[0].size; i++){
		if(ctrl[i].size == 0xffffffff){	/*終端*/
			break;
		}
		if(ctrl[i].size < minsize){	/*最小空きメモリの検索*/
			k = i;
		}
		if(addr + size <= ctrl[i].addr){	/*解放しようとしている空きメモリの次に来るべき空きメモリ*/
#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryFree:Found NextTagIndex:%d\n", i);
#endif
			j = i;
			break;
		}
	}
	for(i--; i < ctrl[0].size; i++){
		if(ctrl[i].size == 0xffffffff){	/*終端*/
			break;
		}
		if(ctrl[i].size < minsize){	/*最小空きメモリの検索*/
			k = i;
		}
	}
#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryFree:Loop EndTagIndex:%d\n", i);
#endif
	if(j == 0){	/*自分よりもアドレスの小さな空き情報が見つからなかった*/
		j = i;
	}
	if(i == ctrl[0].size){	/*空きメモリ配列が満杯*/
		Error_Report(ERROR_NO_MORE_FREE_TAG, ctrl);	/*本当はここで、ctrl[0].addrを利用して、片方向リストに登録しなければいけない。*/
		if(ctrl[k].size >= size){	/*最小の空きメモリは、解放しようとしている空きメモリだったので、登録中止*/
			IO_Store_EFlags(eflags);
			return;
		} else if(j <= k){	/*最小の空きメモリは、解放しようとしている空きメモリの挿入位置よりも後にある*/
			for(; k > j; k--){
				ctrl[k] = ctrl[k - 1];
			}
		} else if(j > k){	/*最小の空きメモリは、解放しようとしている空きメモリの挿入位置よりも前にある*/
			for(; k < j - 1; k++){
				ctrl[k] = ctrl[k + 1];
			}
			j--;
		}
	} else{	/*空きは十分あるのでずらす*/
		if(i + 1 != ctrl[0].size - 1){
			ctrl[i + 1].addr = 0;
			ctrl[i + 1].size = 0xffffffff;
		}
		for(; i > j; i--){
			ctrl[i] = ctrl[i - 1];
		}
	}
	ctrl[j].addr = addr;
	ctrl[j].size = size;

	Memory_Free_Sub(ctrl, j);

	IO_Store_EFlags(eflags);

	#ifdef CHNOSPROJECT_DEBUG_MEMORY_ALLOCATE_AND_FREE
		debug("Memory_Free:ctrl:[0x%X] addr:[0x%X] size:0x%X\n", ctrl, addr, size);
	#endif

	return;
}

void Memory_Free_Sub(IO_MemoryControl ctrl, uint tagno)
{
	uint i, j, k;
	uint eflags;

	eflags = IO_Load_EFlags();

	IO_CLI();

	if(1 < tagno && tagno < ctrl[0].size - 1){
		i = tagno - 1;
		j = tagno + 1;
	} else if(tagno == 1){
		i = 1;
		j = 2;
	} else if(tagno == ctrl[0].size - 1){
		i = ctrl[0].size - 2;
		j = ctrl[0].size - 1;
	} else{
		Error_Report(ERROR_INVALID_FREE_MEMORY_INDEX, ctrl, tagno);
		IO_Store_EFlags(eflags);
		return;
	}

	for(; i < j; i++){
		if(ctrl[i].addr + ctrl[i].size >= ctrl[i + 1].addr){	/*ctrl[i]の後ろに、ctrl[i + 1]がつながる*/
			if(ctrl[i].addr + ctrl[i].size > ctrl[i + 1].addr){	/*範囲が重なっている*/
				Error_Report(ERROR_MEMORY_FREE_RANGE_OVERLAPPED, ctrl, i);
				ctrl[i].size = (uint)ctrl[i + 1].addr - (uint)ctrl[i].addr;
			}
			ctrl[i].size += ctrl[i + 1].size;
			for(k = i + 1; k < ctrl[0].size - 1; k++){
				if(ctrl[k].size == 0xffffffff){
					break;
				}
				ctrl[k] = ctrl[k + 1];
			}
			if(k != ctrl[0].size - 1){
				ctrl[k].addr = 0;
				ctrl[k].size = 0xffffffff;
			}
			i -= 2;
			j--;
		}
	}
	IO_Store_EFlags(eflags);
	return;
}

void *Memory_Allocate(IO_MemoryControl ctrl, uint size)
{
	uint i;
	void *addr;
	uint eflags;

	size = (size + 7) & ~7;

#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryAllocate:Start ctrl:0x%08X size 0x%08X\n", ctrl, size);
#endif

	eflags = IO_Load_EFlags();

	IO_CLI();

	for(i = 1; i < ctrl[0].size; i++){
		if(ctrl[i].size == 0xffffffff){	/*終端*/
			break;
		}
		if(ctrl[i].size >= size){	/*十分な空きを発見*/
#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryAllocate:Found index:%d\n", i);
#endif
			addr = ctrl[i].addr;
			if(ctrl[i].size == size){	/*ぴったりだったので空き情報を破棄*/
				for(; i < ctrl[0].size - 1; i++){
					if(ctrl[i].size == 0xffffffff){
						break;
					}
					ctrl[i] = ctrl[i + 1];
				}
				if(i != ctrl[0].size - 1){
					ctrl[i].addr = 0;
					ctrl[i].size = 0xffffffff;
				}
			} else{	/*まだ残っているので空き情報を調整*/
				ctrl[i].addr += size;
				ctrl[1].size -= size;
			}
			IO_Store_EFlags(eflags);
			//メモリをゼロクリア。
			MOVSD_ZeroFill(addr, size);

			#ifdef CHNOSPROJECT_DEBUG_MEMORY_ALLOCATE_AND_FREE
						debug("Memory_Alloc:ctrl:[0x%X] addr:[0x%X] size:0x%X\n", ctrl, addr, size);
			#endif

			return addr;
		}
	}

	Error_Report(ERROR_NOT_ENOUGH_FREE_MEMORY, ctrl, size);

	IO_Store_EFlags(eflags);

	return 0;
}

void *Memory_Allocate_Aligned(IO_MemoryControl ctrl, uint size, uint align)
/*alignは2の冪乗倍として解釈する。2の冪乗倍でない場合は、最大のセットされているビットに対応する値でアラインされる*/
{
	uint i;
	void *notaligned;
	void *aligned;

	if(align != 0){
		for(i = 0; i < 32; i++){
			align = align >> 1;
			if(align == 0){
				break;
			}
		}
		notaligned = Memory_Allocate(ctrl, size + (1 << i) - 1);
		aligned = (void *)((((uint)notaligned + (1 << i) - 1) >> i) << i);
		Memory_Free(ctrl, notaligned, (uint)aligned - (uint)notaligned);
		Memory_Free(ctrl, aligned + size, ((uint)notaligned + size + (1 << i) - 1) - ((uint)aligned + size));
	} else{
		aligned = Memory_Allocate(ctrl, size);
	}

	return aligned;
}

uint Memory_Get_FreeSize(IO_MemoryControl ctrl)
{
	uint i, size;

	size = 0;
	for(i = 1; i < ctrl[0].size; i++){
		if(ctrl[i].size == 0xffffffff){
			break;
		}
		size += ctrl[i].size;
	}

	return size;
}

