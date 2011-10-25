
#include "core.h"

UI_TaskControl *Initialise_MultiTask_Control(IO_MemoryControl sysmemctrl)
{
	UI_TaskControl *ctrl;
	UI_Task *maintask;

	ctrl = Memory_Allocate(sysmemctrl, sizeof(UI_TaskControl));
	ctrl->now = 0;
	ctrl->sysmemctrl = sysmemctrl;

	maintask = MultiTask_Task_Initialise(ctrl);

	Load_TR(maintask->selector << 3);

	ctrl->start = maintask;
	ctrl->now = maintask;

	return ctrl;
}

UI_Task *MultiTask_Task_Initialise(UI_TaskControl *ctrl)
{
	UI_Task *task;

	task = Memory_Allocate(ctrl->sysmemctrl, sizeof(UI_Task));
	task->tss = Memory_Allocate(ctrl->sysmemctrl, sizeof(CPU_TaskStateSegment));

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
	task->tss->iomap_base		= 0x00004000;	//TSSリミット以上なら無効

	task->selector = System_SegmentDescriptor_Set(sizeof(CPU_TaskStateSegment) - 1, (uint)task->tss, AR_TSS32);

	task->next = 0;
	task->count = 0;

	return task;
}

void MultiTask_Task_Run(UI_TaskControl *ctrl, UI_Task *task)
{
	UI_Task *last;

	for(last = ctrl->start; last->next != 0; last = last->next){

	}

	task->next = 0;
	last->next = task;
	return;
}

void MultiTask_TaskSwitch(UI_TaskControl *ctrl)
{
	if(ctrl->now->next == 0){	//タスクリンクの終端
		if(ctrl->start != ctrl->now){	//同一タスクでなければタスクスイッチ
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

UI_Task *MultiTask_GetNowTask(UI_TaskControl *ctrl)
{
	return ctrl->now;
}


