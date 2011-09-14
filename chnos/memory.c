
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

void *Memory_Allocate(IO_MemoryControl ctrl, uint size)
{
	uint i;
	void *addr;

	uint eflags;

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

	Error_Report(ERROR_NO_MORE_FREE_MEMORY, ctrl, size);

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

