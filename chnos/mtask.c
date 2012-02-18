
#include "core.h"

UI_TaskControl *Initialise_MultiTask_Control(IO_MemoryControl sysmemctrl)
{
	UI_TaskControl *ctrl;
	UI_Task *maintask;

	ctrl = Memory_Allocate(sysmemctrl, sizeof(UI_TaskControl));
	ctrl->now = 0;
	ctrl->sysmemctrl = sysmemctrl;

	maintask = MultiTask_Task_Initialise(ctrl, 0);

	Load_TR(maintask->selector << 3);

	ctrl->start = maintask;
	ctrl->now = maintask;

	return ctrl;
}

UI_Task *MultiTask_Task_Initialise(UI_TaskControl *ctrl, uint tss_additional_size)
{
	UI_Task *task;

	task = Memory_Allocate(ctrl->sysmemctrl, sizeof(UI_Task));
	task->tss = Memory_Allocate(ctrl->sysmemctrl, sizeof(CPU_TaskStateSegment) + tss_additional_size);

	task->tss->reserve00		= 0;
	task->tss->reserve01		= 0;
	task->tss->reserve02		= 0;
	task->tss->reserve03		= 0;
	task->tss->reserve04		= 0;
	task->tss->reserve05		= 0;
	task->tss->reserve06		= 0;
	task->tss->reserve07		= 0;
	task->tss->reserve08		= 0;
	task->tss->reserve09		= 0;
	task->tss->reserve10		= 0;
	task->tss->reserve11		= 0;

	task->tss->backlink		= 0;
	task->tss->esp0			= 0;
	task->tss->ss0			= 0;
	task->tss->esp1			= 0;
	task->tss->ss1			= 0;
	task->tss->esp2			= 0;
	task->tss->ss2			= 0;

	task->tss->cr3			= 0;
	task->tss->eip			= 0;
	task->tss->eflags.eflags	= 0x00000202;	//bit1=True, IF=True

	task->tss->eax			= 0;
	task->tss->ecx			= 0;
	task->tss->edx			= 0;
	task->tss->ebx			= 0;
	task->tss->esp			= 0;
	task->tss->ebp			= 0;
	task->tss->esi			= 0;
	task->tss->edi			= 0;

	task->tss->es			= 0;
	task->tss->cs			= 0;
	task->tss->ss			= 0;
	task->tss->ds			= 0;
	task->tss->fs			= 0;
	task->tss->gs			= 0;

	task->tss->ldtr			= 0;
	task->tss->flag_trap		= False;
	task->tss->iomap_base		= 0x00004000;	//TSS���~�b�g�ȏ�Ȃ疳��

	task->selector = System_SegmentDescriptor_Set(sizeof(CPU_TaskStateSegment) + tss_additional_size - 1, (uint)task->tss, AR_TSS32);

	task->next = 0;
	task->count = 0;

	task->fifo = FIFO32_Initialise(ctrl->sysmemctrl, 128);

	task->flags.initialized = True;
	task->flags.linked = False;

	return task;
}

void MultiTask_Task_Run(UI_TaskControl *ctrl, UI_Task *task)
{
	UI_Task *last;

	for(last = ctrl->start; last->next != 0; last = last->next){
		if(last == task){	/*���łɃ^�X�N���s�����N�ɓ����Ă�����*/
			return;
		}
	}

	task->next = 0;
	last->next = task;
	task->flags.linked = True;
	return;
}

void MultiTask_TaskSwitch(UI_TaskControl *ctrl)
{
	if(ctrl->now->next == 0){	//�^�X�N�����N�̏I�[
		if(ctrl->start != ctrl->now){	//����^�X�N�łȂ���΃^�X�N�X�C�b�`
			ctrl->now = ctrl->start;
			ctrl->now->count++;
			FarJMP(0, ctrl->now->selector << 3);
		}
	} else{
		ctrl->now = ctrl->now->next;
		ctrl->now->count++;
		FarJMP(0, ctrl->now->selector << 3);
	}
	return;
}

void MultiTask_Task_Sleep(UI_TaskControl *ctrl, UI_Task *task)
{
	UI_Task **find;

	uint eflags;

	for(find = &ctrl->start; (*find) != 0; find = &(*find)->next){
		if(*find == task){	/*�^�X�N���s�����N����݂�����*/
			break;
		}
	}
	if((*find) == 0){	/*�������̂łȂ��A�I�[��������*/
		return;
	}

	eflags = IO_Load_EFlags();
	IO_CLI();

	*find = task->next;
	task->flags.linked = False;

	if(ctrl->now == task){
		MultiTask_TaskSwitch(ctrl);
	}

	IO_Store_EFlags(eflags);

	return;
}

UI_Task *MultiTask_GetNowTask(UI_TaskControl *ctrl)
{
	return ctrl->now;
}


