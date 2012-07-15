
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

//Memory Control System
//ctrl[0].addr = 0;
//ctrl[0].size = tags;
//Memory Control�z��́A�Ǘ��Ώۂ̃������̏I�[�ɔz�u�����B

IO_MemoryControl Memory_Initialize_Control(void *start, uint size, uint tags)
{
	IO_MemoryControl ctrl;

	start = (void *)(((uint)start + 7) & ~7);
	size = (size + 7) & ~7;

//Memory Control�z�񂪓��镪��葽���A�Ǘ��Ώۂ̃�������^�����Ă��邩�`�F�b�N�B
	if((tags * sizeof(IO_MemoryControlTag)) > size){
		return 0;
	}

//�Ǘ��̓s����A3�^�O�ȏ�Ȃ��ƊǗ����������Ȃ��̂ŁA���̃`�F�b�N�B
	if(tags < 3){
		return 0;
	}

//�Ǘ��Ώۃ������͈͂̍Ōォ��AMemory Control�z��̑傫�������������̂ڂ����n�_��MemoryControl�z��̐擪�Ƃ���B
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
		if(ctrl[i].size == 0xffffffff){	/*�I�[*/
			break;
		}
		if(ctrl[i].size < minsize){	/*�ŏ��󂫃������̌���*/
			k = i;
		}
		if(addr + size <= ctrl[i].addr){	/*������悤�Ƃ��Ă���󂫃������̎��ɗ���ׂ��󂫃�����*/
#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryFree:Found NextTagIndex:%d\n", i);
#endif
			j = i;
			break;
		}
	}
	for(i--; i < ctrl[0].size; i++){
		if(ctrl[i].size == 0xffffffff){	/*�I�[*/
			break;
		}
		if(ctrl[i].size < minsize){	/*�ŏ��󂫃������̌���*/
			k = i;
		}
	}
#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryFree:Loop EndTagIndex:%d\n", i);
#endif
	if(j == 0){	/*���������A�h���X�̏����ȋ󂫏�񂪌�����Ȃ�����*/
		j = i;
	}
	if(i == ctrl[0].size){	/*�󂫃������z�񂪖��t*/
		Error_Report(ERROR_NO_MORE_FREE_TAG, ctrl);	/*�{���͂����ŁActrl[0].addr�𗘗p���āA�Е������X�g�ɓo�^���Ȃ���΂����Ȃ��B*/
		if(ctrl[k].size >= size){	/*�ŏ��̋󂫃������́A������悤�Ƃ��Ă���󂫃������������̂ŁA�o�^���~*/
			IO_Store_EFlags(eflags);
			return;
		} else if(j <= k){	/*�ŏ��̋󂫃������́A������悤�Ƃ��Ă���󂫃������̑}���ʒu������ɂ���*/
			for(; k > j; k--){
				ctrl[k] = ctrl[k - 1];
			}
		} else if(j > k){	/*�ŏ��̋󂫃������́A������悤�Ƃ��Ă���󂫃������̑}���ʒu�����O�ɂ���*/
			for(; k < j - 1; k++){
				ctrl[k] = ctrl[k + 1];
			}
			j--;
		}
	} else{	/*�󂫂͏\������̂ł��炷*/
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
		if(ctrl[i].addr + ctrl[i].size >= ctrl[i + 1].addr){	/*ctrl[i]�̌��ɁActrl[i + 1]���Ȃ���*/
			if(ctrl[i].addr + ctrl[i].size > ctrl[i + 1].addr){	/*�͈͂��d�Ȃ��Ă���*/
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
		if(ctrl[i].size == 0xffffffff){	/*�I�[*/
			break;
		}
		if(ctrl[i].size >= size){	/*�\���ȋ󂫂𔭌�*/
#ifdef CHNOSPROJECT_DEBUG_MEMORY
	debug("DEBUG:MemoryAllocate:Found index:%d\n", i);
#endif
			addr = ctrl[i].addr;
			if(ctrl[i].size == size){	/*�҂����肾�����̂ŋ󂫏���j��*/
				for(; i < ctrl[0].size - 1; i++){
					//�I�[�𔭌�������Break.
					if(ctrl[i].size == 0xffffffff){
						break;
					}
					//�^�O���R�s�[���đO�ɋl�߂�B
					ctrl[i] = ctrl[i + 1];
				}
				//�l�߂����ʁA�I�[�^�O���K�v��������ǉ�����i����Ȃ��ƂȂ��͂�����ˁH�c�j
				//if(i != ctrl[0].size){
				//	ctrl[i].addr = 0;
				//	ctrl[i].size = 0xffffffff;
				//}
			} else{	/*�܂��c���Ă���̂ŋ󂫏��𒲐�*/
				ctrl[i].addr += size;
				ctrl[i].size -= size;
			}
			IO_Store_EFlags(eflags);
			//���������[���N���A�B
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
/*align��2�̙p��{�Ƃ��ĉ��߂���B2�̙p��{�łȂ��ꍇ�́A�ő�̃Z�b�g����Ă���r�b�g�ɑΉ�����l�ŃA���C�������*/
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



