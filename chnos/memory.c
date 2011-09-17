
#include "core.h"


uint Memory_Test(uint start, uint end)
{
	uchar flg486 = 0;
	uint eflg, cr0, i;

	eflg = IO_Load_EFlags();
	eflg |= EFLAGS_AC_BIT;
	IO_Store_EFlags(eflg);

	eflg = IO_Load_EFlags();
	if((eflg & EFLAGS_AC_BIT) != 0){
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT;
	IO_Store_EFlags(eflg);

	if(flg486 != 0){
		cr0 = Load_CR0();
		cr0 |= CR0_ALL_CACHE_DISABLE;
		Store_CR0(cr0);
	}
	i = Memory_Test_Sub(start, end);
	if(flg486 != 0){
		cr0 = Load_CR0();
		cr0 &= ~CR0_ALL_CACHE_DISABLE;
		Store_CR0(cr0);		
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
			j = 1;
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
		for(; i > j; i--){
			ctrl[i] = ctrl[i - 1];
		}
	}
	ctrl[j].addr = addr;
	ctrl[j].size = size;

	Memory_Free_Sub(ctrl, j);

	IO_Store_EFlags(eflags);

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
		if(ctrl[i].addr + ctrl[i].size == ctrl[i + 1].addr){	/*ctrl[i]の後ろに、ctrl[i + 1]がつながる*/
			ctrl[i].size += ctrl[i + 1].size;
			for(k = i + 1; k < ctrl[0].size - 1; k++){
				if(ctrl[k].size == 0xffffffff){
					break;
				}
				ctrl[k] = ctrl[k + 1];
			}
			if(k == ctrl[0].size - 1){
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

	eflags = IO_Load_EFlags();

	IO_CLI();

	for(i = 1; i < ctrl[0].size; i++){
		if(ctrl[i].size == 0xffffffff){	/*終端*/
			break;
		}
		if(ctrl[i].size >= size){	/*十分な空きを発見*/
			addr = ctrl[i].addr;
			if(ctrl[i].size == size){	/*ぴったりだったので空き情報を破棄*/
				for(; i < ctrl[0].size - 1; i++){
					if(ctrl[i].size == 0xffffffff){
						break;
					}
					ctrl[i] = ctrl[i + 1];
				}
				if(i == ctrl[0].size - 1){
					ctrl[i].addr = 0;
					ctrl[i].size = 0xffffffff;
				}
			} else{	/*まだ残っているので空き情報を調整*/
				ctrl[i].addr += size;
				ctrl[1].size -= size;
			}
			IO_Store_EFlags(eflags);
			return addr;
		}
	}

	Error_Report(ERROR_NOT_ENOUGH_FREE_MEMORY, ctrl, size);

	IO_Store_EFlags(eflags);

	return 0;
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

