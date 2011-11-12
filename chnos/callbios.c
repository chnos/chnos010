
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

	ctrl->CallBIOS_Task = System_MultiTask_Task_Initialise((256 >> 3) + (65536 >> 3) + 1);

	q = (uchar *)(ctrl->CallBIOS_Task->tss + sizeof(CPU_TaskStateSegment));
	for(i = 0; i < (256 >> 3); i++){
		//q[i] = 0xff;	/*全ソフトウエア割り込みで保護モード一般保護例外を起こす*/
		q[i] = 0x00;	/*全ソフトウエア割り込みを仮想86モードで処理する*/
	}
	//q[1] = ~(1 << 3);	/*(8 * 1) + 3 - 1 = INT:0x10を仮想86モードで処理する*/

	ctrl->CallBIOS_Task->tss->iomap_base = sizeof(CPU_TaskStateSegment) + (256 >> 3);

	q = (uchar *)(ctrl->CallBIOS_Task->tss + sizeof(CPU_TaskStateSegment) + (256 >> 3));
	for(i = 0; i < (65536 >> 3); i++){
		q[i] = 0x00;	/*全IOポートを、IOPL以下のCPLでもアクセス可能とする*/
	}
	q[i] = 0xff;

	ctrl->CallBIOS_Task->tss->esp0 = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	ctrl->CallBIOS_Task->tss->ss0 = SYSTEM_DS << 3;

	return ctrl;
}

void CallBIOS_Execute(IO_CallBIOSControl *ctrl, uchar intn, DATA_FIFO32 *fifo, uint endsignal)
{
	uchar *q;

	ctrl->CallBIOS_Task->tss->eip = 0xc200;
	ctrl->CallBIOS_Task->tss->eflags.bit.VM = True;
	ctrl->CallBIOS_Task->tss->cs = 0;
	ctrl->CallBIOS_Task->tss->ss = 0;
	ctrl->CallBIOS_Task->tss->ds = 0;
	ctrl->CallBIOS_Task->tss->esp = 0xc200;

	ctrl->fifo = fifo;
	ctrl->endsignal = endsignal;

	q = (uchar *)0xc201;
	*q = intn;

	System_MultiTask_Task_Run(ctrl->CallBIOS_Task);

	return;
}

void CallBIOS_Send_End_Of_Operation(IO_CallBIOSControl *ctrl)
{
	if(ctrl->fifo != 0){
		FIFO32_Put(ctrl->fifo, ctrl->endsignal);
	}
	return;
}
