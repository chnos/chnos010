
#include "core.h"

//FIFOによるタスクの自動起動は既定で有効ですが、タスクが明示的に実行されるまでは無効になっています。
//タスクはSTI下で開始されます（外部割り込み有効）。

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

	maintask->flags.linked = True;
	maintask->flags.running = True;
	maintask->flags.first_run = False;
	FIFO32_Set_Task(maintask->fifo, maintask);

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
	task->tss->iomap_base		= 0x00004000;	//TSSリミット以上なら無効

	task->selector = System_SegmentDescriptor_Set(sizeof(CPU_TaskStateSegment) + tss_additional_size - 1, (uint)task->tss, AR_TSS32);

	task->next = 0;
	task->count = 0;

	task->fifo = FIFO32_Initialise(ctrl->sysmemctrl, TASK_FIFOSIZE);

	task->flags.initialized = True;
	task->flags.linked = False;
	task->flags.first_run = True;

	return task;
}

uint MultiTask_Internal_Task_SetLink(UI_TaskControl *ctrl, UI_Task *task)
{
	UI_Task **last;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	for(last = &ctrl->start; *last != Null; last = &(*last)->next){
		if(*last == task){
			#ifdef CHNOSPROJECT_DEBUG_MULTITASK
				debug("MultiTask_Internal_Task_SetLink:Task has already been linked(sel:0x%X).\n", task->selector);
			#endif
			return 1;
		}
	}
	task->next = Null;
	*last = task;
	task->flags.linked = True;

	IO_Store_EFlags(eflags);

	return 0;
}

uint MultiTask_Internal_Task_ClearLink(UI_TaskControl *ctrl, UI_Task *task)
{
	UI_Task **find;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	for(find = &ctrl->start; *find != task; find = &(*find)->next){
		if(*find == Null){
			#ifdef CHNOSPROJECT_DEBUG_MULTITASK
				debug("MultiTask_Internal_Task_ClearLink:Task not found(sel:0x%X).\n", task->selector);
			#endif
			return 1;
		}
	}
	*find = task->next;
	task->next = Null;
	task->flags.linked = False;
	task->flags.running = False;

	if(ctrl->now == task){
		MultiTask_TaskSwitch(ctrl);
	}

	IO_Store_EFlags(eflags);

	return 0;
}

void MultiTask_Task_Run(UI_TaskControl *ctrl, UI_Task *task)
{
	#ifdef CHNOSPROJECT_DEBUG_CALLLINK
		debug("MultiTask_Task_Run:Called from[0x%08X].\n", *((uint *)(&ctrl - 1)));
	#endif

	#ifdef CHNOSPROJECT_DEBUG_MULTITASK
		debug("MultiTask_Task_Run:Start Running Rq(sel:0x%X).\n", task->selector);
	#endif

	if(task->flags.running){
		#ifdef CHNOSPROJECT_DEBUG_MULTITASK
			debug("MultiTask_Task_Run:Task has already been running(sel:0x%X).\n", task->selector);
		#endif
		return;
	}

	if(!task->flags.linked){
		MultiTask_Internal_Task_SetLink(ctrl, task);
	}

	task->flags.running = True;

	#ifdef CHNOSPROJECT_DEBUG_MULTITASK
		debug("MultiTask_Task_Run:Start Running(sel:0x%X).\n", task->selector);
	#endif

	if(task->flags.first_run){
		#ifdef CHNOSPROJECT_DEBUG_MULTITASK
			debug("MultiTask_Task_Run:FIFO task autorun is enabled.\n");
		#endif
		FIFO32_Set_Task(task->fifo, task);
		task->flags.first_run = False;
	}

	return;
}

void MultiTask_TaskSwitch(UI_TaskControl *ctrl)
{
	UI_Task *nexttask;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	for(nexttask = ctrl->now->next; ; nexttask = nexttask->next){
		if(nexttask == Null){	//リンク終端に来たら先頭へ巻き戻す
			nexttask = ctrl->start;
		}
		if(nexttask == ctrl->now){	//実行中状態のタスクは現在実行中のこのタスクしかないのでタスクスイッチなし
			return;
		}
		if(nexttask->flags.running){	//実行中状態のほかのタスクが見つかったのでタスクスイッチ
			ctrl->now = nexttask;
			ctrl->now->count++;
			FarJMP(0, ctrl->now->selector << 3);
			break;
		}
	}

	IO_Store_EFlags(eflags);

	return;
}

void MultiTask_Task_Sleep(UI_TaskControl *ctrl, UI_Task *task)
{
	//CPL=0下を想定

	uint eflags;

	if(task == ctrl->now && task == ctrl->start){	//自分が唯一無二のタスクだった場合、スリープは実行できないのでHLTする
		eflags = IO_Load_EFlags();
		IO_STIHLT();
		IO_Store_EFlags(eflags);
		return;
	}

	task->flags.running = False;

	if(ctrl->now == task){
		MultiTask_TaskSwitch(ctrl);
	}

	return;
}

void MultiTask_Task_Kill(UI_TaskControl *ctrl, UI_Task *task)
{
	//CPL=0下を想定

	if(task == ctrl->now && task == ctrl->start){	//自分が唯一無二のタスクだった場合、killはOS自体の終了を意味するので、アボート
		#ifdef CHNOSPROJECT_DEBUG_MULTITASK
			debug("MultiTask_Task_Stop:Attempted to stop last task. Abort.\n");
		#endif
		INT_3();
		return;
	}

	MultiTask_Internal_Task_ClearLink(ctrl, task);

	return;
}
/*
void MultiTask_Task_Free(UI_TaskControl *ctrl, UI_Task *task)
{
	if(task->flags.linked){
		MultiTask_Task_Kill(ctrl, task);
	}

}
*/
UI_Task *MultiTask_GetNowTask(UI_TaskControl *ctrl)
{
	return ctrl->now;
}

uint MultiTask_Push_Arguments(UI_Task *task, uint args, ...)
{
	uint *vargs;
	uint i;

	vargs = (uint *)(&args + 1);

	task->tss->esp -= 4 * (args + 1);

	for(i = 1; i < args + 1; i++){
		*((uint *)(task->tss->esp + (i * 4))) = vargs[i - 1];
	}

	return 0;
}
