
#include "core.h"

void CPU_ExceptionHandler00(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_00, esp);
}

void CPU_ExceptionHandler01(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_01, esp);
}

void CPU_ExceptionHandler02(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_02, esp);
}

void CPU_ExceptionHandler03(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_03, esp);
}

void CPU_ExceptionHandler04(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_04, esp);
}

void CPU_ExceptionHandler05(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_05, esp);
}

void CPU_ExceptionHandler06(uint *esp)
{
	IO_CallBIOSControl *callbios;

	callbios = System_CallBIOS_Get_Controller();
/*Invalid Opcode Exception*/
	if(System_MultiTask_GetNowTask() == callbios->CallBIOS_Task){	/*もし、例外を起こしたタスクがCallBIOSだったら*/
		#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
			debug("Exception_0x06:UD2 Opcode Found in v8086mode.\n");
		#endif
		callbios->retvalue.eip = esp[0x0a];
		callbios->retvalue.eax = esp[0x07];
		callbios->retvalue.ecx = esp[0x06];
		callbios->retvalue.edx = esp[0x05];
		callbios->retvalue.ebx = esp[0x04];
		callbios->retvalue.esp = esp[0x03];
		callbios->retvalue.ebp = esp[0x02];
		callbios->retvalue.esi = esp[0x01];
		callbios->retvalue.edi = esp[0x00];
		callbios->retvalue.es = esp[0x09];
		callbios->retvalue.cs = esp[0x0b];
		callbios->retvalue.ss = esp[0x0e];
		callbios->retvalue.ds = esp[0x08];
		callbios->retvalue.eflags.eflags = esp[0x0c];

		System_CallBIOS_Send_End_Of_Operation(False);
		System_MultiTask_Task_Kill(System_MultiTask_GetNowTask());
	} else{
		Error_Report(ERROR_CPU_EXCEPTION_06, esp);
	}
}

void CPU_ExceptionHandler07(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_07, esp);
}

void CPU_ExceptionHandler08(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_08, esp);
}

void CPU_ExceptionHandler09(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_09, esp);
}

void CPU_ExceptionHandler0a(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_0A, esp);
}

void CPU_ExceptionHandler0b(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_0B, esp);
}

void CPU_ExceptionHandler0c(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_0C, esp);
}

void CPU_ExceptionHandler0d(uint *esp)
{
/*General Protection Exception*/
	if(System_MultiTask_GetNowTask() != System_CallBIOS_Get_Controller()->CallBIOS_Task){	/*もし、例外を起こしたタスクがCallBIOSだったら*/
		Error_Report(ERROR_CPU_EXCEPTION_0D, esp);
	}
	CallBIOS_Check_Privileged_Operation(esp);
	return;
}

void CPU_ExceptionHandler0e(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_0E, esp);
}

void CPU_ExceptionHandler0f(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_0F, esp);
}

void CPU_ExceptionHandler10(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_10, esp);
}

void CPU_ExceptionHandler11(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_11, esp);
}

void CPU_ExceptionHandler12(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_12, esp);
}

void CPU_ExceptionHandler13(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_13, esp);
}

void CPU_ExceptionHandler14(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_14, esp);
}

void CPU_ExceptionHandler15(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_15, esp);
}

void CPU_ExceptionHandler16(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_16, esp);
}

void CPU_ExceptionHandler17(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_17, esp);
}

void CPU_ExceptionHandler18(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_18, esp);
}

void CPU_ExceptionHandler19(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_19, esp);
}

void CPU_ExceptionHandler1a(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_1A, esp);
}

void CPU_ExceptionHandler1b(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_1B, esp);
}

void CPU_ExceptionHandler1c(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_1C, esp);
}

void CPU_ExceptionHandler1d(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_1D, esp);
}

void CPU_ExceptionHandler1e(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_1E, esp);
}

void CPU_ExceptionHandler1f(uint *esp)
{
	Error_Report(ERROR_CPU_EXCEPTION_1F, esp);
}
