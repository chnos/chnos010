
#include "core.h"

IO_CallBIOSControl *Initialise_CallBIOS(void)
{
	IO_CallBIOSControl *ctrl;
	uint i;
	const uchar *p;
	uchar *q;
	CPU_ControlRegister4 cr4;

	ctrl = System_Memory_Allocate(sizeof(IO_CallBIOSControl));

	#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
		debug("16bitcode:0x%X bootpackbase:0x%X\n", &asm_16bit_CallBIOSTask, ADR_BOOTPACK);
	#endif

	//ud2命令=16bitコードの末端を探す
	p = (uchar *)(&asm_16bit_CallBIOSTask + ADR_BOOTPACK);
	for(i = 0; ; i++){
		if(p[i] == 0x0f){
			if(p[i + 1] == 0x0b){
				break;
			}
		}
	}

	#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
		debug("TEST01\n");
	#endif

	ctrl->codesize = i + 2;

	//asmheadのあったところにコピー
	q = (uchar *)0xc200;
	for(i = 0; i < ctrl->codesize; i++){
		q[i] = p[i];
	}

	cr4.cr4 = Load_CR4();
	cr4.bit.VME = True;
	Store_CR4(cr4.cr4);

	#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
		debug("CallBIOS:Copied %d Bytes.\n", ctrl->codesize);
	#endif

	ctrl->CallBIOS_Task = System_MultiTask_Task_Initialise(32);

	q = (uchar *)(ctrl->CallBIOS_Task->tss + sizeof(CPU_TaskStateSegment));
	for(i = 0; i < 32; i++){
		q[i] = 0xff;
	}
	q[1] = ~(1 << 3);

	ctrl->CallBIOS_Task->tss->iomap_base = sizeof(CPU_TaskStateSegment) + 32;

	ctrl->CallBIOS_Task->tss->esp0 = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	ctrl->CallBIOS_Task->tss->ss0 = SYSTEM_DS << 3;

	return ctrl;
}

void CallBIOS_Execute(IO_CallBIOSControl *ctrl, uchar intn)
{
	uchar *q;

	ctrl->CallBIOS_Task->tss->eip = 0xc200;
	ctrl->CallBIOS_Task->tss->eflags.bit.VM = True;
	ctrl->CallBIOS_Task->tss->cs = 0;
	ctrl->CallBIOS_Task->tss->ss = 0;
	ctrl->CallBIOS_Task->tss->ds = 0;
	ctrl->CallBIOS_Task->tss->esp = 0xc200;

	q = (uchar *)0xc201;
	*q = intn;

	System_MultiTask_Task_Run(ctrl->CallBIOS_Task);

	return;
}
