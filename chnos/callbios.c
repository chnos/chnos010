
#include "core.h"

IO_CallBIOSControl *Initialise_CallBIOS(void)
{
	IO_CallBIOSControl *ctrl;
	uint i;
	const uchar *p;
	uchar *q;
	CPU_ControlRegister4 cr4;

	ctrl = System_Memory_Allocate(sizeof(IO_CallBIOSControl));

	//ud2命令=16bitコードの末端を探す
	p = (uchar *)(&asm_16bit_CallBIOSTask + ADR_BOOTPACK);
	for(i = 0; ; i++){
		if(p[i] == 0x0f){
			if(p[i + 1] == 0x0b){
				break;
			}
		}
	}

	ctrl->codesize = i + 2;

	//asmheadのあったところにコピー
	q = (uchar *)0xc200;
	for(i = 0; i < ctrl->codesize; i++){
		q[i] = p[i];
	}

	cr4.cr4 = Load_CR4();
	cr4.bit.VME = True;
	Store_CR4(cr4.cr4);

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

void CallBIOS_Check_Privileged_Operation(uint *esp)
{
//例外発生時に呼ばれることを想定
//エラーコードのない例外からだと、スタックがずれるので注意
	uchar *eip;
	ushort *userstack;
	Emulator_x86_FarPointer *vector;
	CPU_EFlags eflags;

	eip = (uchar *)((esp[EXCEPTION_INFO_CS] << 4) + (esp[EXCEPTION_INFO_EIP] & 0x0000ffff));
	userstack = (ushort *)((esp[EXCEPTION_INFO_USER_SS] << 4) + (esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff));

	if(eip[0] == 0xCD){	/*INT n*/
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("CallBIOS:Privileged Operation Found in v8086mode.\n");
			debug("UserStack:[0x%X:0x%X] = 0x%X\n", esp[EXCEPTION_INFO_USER_SS], esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff, userstack);
			debug("Opcode[0x%X]:INT 0x%X\n", eip, eip[1]);
		#endif
		vector = (Emulator_x86_FarPointer *)(eip[1] << 2);
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("New EIP = [0x%X:0x%X] = [0x%X]\n", vector->selector, vector->offset, (vector->selector << 4) + vector->offset);
		#endif

		eflags.eflags = esp[EXCEPTION_INFO_EFLAGS];
		//eflags.bit.IF = False;
		eflags.bit.IF = True;	/*割り込み禁止にしない*/
		eflags.bit.TF = False;
		eflags.bit.AC = False;
		CallBIOS_Push_Data_To_Stack(esp, eflags.eflags);
		CallBIOS_Push_Data_To_Stack(esp, esp[EXCEPTION_INFO_CS]);
		CallBIOS_Push_Data_To_Stack(esp, esp[EXCEPTION_INFO_EIP] + 2);
		esp[EXCEPTION_INFO_CS] = vector->selector;
		esp[EXCEPTION_INFO_EIP] = vector->offset;

		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("Return to [0x%X:0x%X] = [0x%X]. Emulation End.\n", esp[EXCEPTION_INFO_CS], esp[EXCEPTION_INFO_EIP], (esp[EXCEPTION_INFO_CS] << 4) + esp[EXCEPTION_INFO_EIP]);
		#endif

		return;
	} else if(eip[0] == 0xFB){	/*STI*/
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("CallBIOS:Privileged Operation Found in v8086mode.\n");
			debug("Opcode[0x%X]:STI\n", eip);
		#endif
		/*何もしない*/
		//eflags.eflags = esp[EXCEPTION_INFO_EFLAGS];
		//eflags.bit.IF = True;
		esp[EXCEPTION_INFO_EIP]++;

		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("Return to [0x%X:0x%X] = [0x%X]. Emulation End.\n", esp[EXCEPTION_INFO_CS], esp[EXCEPTION_INFO_EIP], (esp[EXCEPTION_INFO_CS] << 4) + esp[EXCEPTION_INFO_EIP]);
		#endif

		return;
	} else if(eip[0] == 0xCF){	/*IRET*/
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("CallBIOS:Privileged Operation Found in v8086mode.\n");
			debug("UserStack:[0x%X:0x%X] = 0x%X\n", esp[EXCEPTION_INFO_USER_SS], esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff, userstack);
			debug("Opcode[0x%X]:IRET\n", eip);
		#endif
		esp[EXCEPTION_INFO_EIP] = CallBIOS_Pop_Data_From_Stack(esp);
		esp[EXCEPTION_INFO_CS] = CallBIOS_Pop_Data_From_Stack(esp);
		eflags.eflags = esp[EXCEPTION_INFO_EFLAGS];
		eflags.eflags = (eflags.eflags & 0xffff0000) | CallBIOS_Pop_Data_From_Stack(esp);

		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("Return to [0x%X:0x%X] = [0x%X]. Emulation End.\n", esp[EXCEPTION_INFO_CS], esp[EXCEPTION_INFO_EIP], (esp[EXCEPTION_INFO_CS] << 4) + esp[EXCEPTION_INFO_EIP]);
		#endif

		return;
	} else{
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("CallBIOS:Privileged Operation Found in v8086mode.\n");
			debug("Opcode[0x%X]:0x%X\n", eip, eip[0]);
			debug("Task Sleeping...\n");
		#endif
	}

	for(;;){
		System_MultiTask_Task_Sleep(System_MultiTask_GetNowTask());
	}

	return;
}

uint CallBIOS_Push_Data_To_Stack(uint *esp, ushort data)
{
	ushort *userstack;

	if(esp[EXCEPTION_INFO_USER_ESP] < 2){	/*スタックが足りない*/
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("PushToStack:No More Stack.\n");
		#endif
		return 1;
	}

	esp[EXCEPTION_INFO_USER_ESP] -= 2;
	userstack = (ushort *)((esp[EXCEPTION_INFO_USER_SS] << 4) + (esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff));
	userstack[0] = data;

	#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
		debug("PUSH 0x%X To [0x%X:0x%X] = [0x%X]\n", data, esp[EXCEPTION_INFO_USER_SS], esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff, userstack);
	#endif

	return 0;
}

uint CallBIOS_Pop_Data_From_Stack(uint *esp)
{
	ushort *userstack;

	if(esp[EXCEPTION_INFO_USER_ESP] > 0x0000fffd){	/*スタックが足りない*/
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("PopFromStack:No More Stack.\n");
		#endif
		return 1;
	}


	userstack = (ushort *)((esp[EXCEPTION_INFO_USER_SS] << 4) + (esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff));

	#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
		debug("Pop 0x%X From [0x%X:0x%X] = [0x%X]\n", userstack[0], esp[EXCEPTION_INFO_USER_SS], esp[EXCEPTION_INFO_USER_ESP] & 0x0000ffff, userstack);
	#endif

	esp[EXCEPTION_INFO_USER_ESP] += 2;

	return userstack[0];
}
