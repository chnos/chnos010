
#include "core.h"

uchar *cpu_exceptions[0x20] = {
	"Divided by zero.",
	"Reserved.",
	"Nonmaskable interrupt.",
	"Breakpoint.",
	"Overflow.",
	"Outside BOUND.",
	"Invalid opcode.",
	"Disable Device.",
	"Double fault.",
	"Coprocessor Segment Overrun.",
	"Invalid task status segment.",
	"Segment not present.",
	"Stack Segment Fault.",
	"General Protection Exception.",
	"Page fault.",
	"Reserved.",
	"Floating point error.",
	"Alignment Check.",
	"Machine Check.",
	"SIMD floating-point exception.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved."
};

uchar *cpu_exception_infos[16] = {
	"EDI      ",
	"ESI      ",
	"EBP      ",
	"ESP      ",
	"EBX      ",
	"EDX      ",
	"ECX      ",
	"EAX      ",
	"DS       ",
	"ES       ",
	"ERRORCODE",
	"EIP      ",
	"CS       ",
	"EFLAGS   ",
	"APP's ESP",
	"APP's SS "
};

uint Error_Report(uint error_no, ...)
{
	uint *retaddr, *va_args;

	retaddr = &error_no - 1;
	va_args = &error_no + 1;

	debug("[0x%08X]Error:0x%08X ", *retaddr, error_no);

	if(error_no == ERROR_CPU_EXCEPTION_00){
	} else if(error_no == ERROR_CPU_EXCEPTION_01){
	} else if(error_no == ERROR_CPU_EXCEPTION_02){
	} else if(error_no == ERROR_CPU_EXCEPTION_03){
	} else if(error_no == ERROR_CPU_EXCEPTION_04){
	} else if(error_no == ERROR_CPU_EXCEPTION_05){
	} else if(error_no == ERROR_CPU_EXCEPTION_06){
	} else if(error_no == ERROR_CPU_EXCEPTION_07){
	} else if(error_no == ERROR_CPU_EXCEPTION_08){
	} else if(error_no == ERROR_CPU_EXCEPTION_09){
	} else if(error_no == ERROR_CPU_EXCEPTION_0A){
	} else if(error_no == ERROR_CPU_EXCEPTION_0B){
	} else if(error_no == ERROR_CPU_EXCEPTION_0C){
	} else if(error_no == ERROR_CPU_EXCEPTION_0D){
	} else if(error_no == ERROR_CPU_EXCEPTION_0E){
	} else if(error_no == ERROR_CPU_EXCEPTION_0F){
	} else if(error_no == ERROR_CPU_EXCEPTION_00){
	} else if(error_no == ERROR_CPU_EXCEPTION_01){
	} else if(error_no == ERROR_CPU_EXCEPTION_02){
	} else if(error_no == ERROR_CPU_EXCEPTION_03){
	} else if(error_no == ERROR_CPU_EXCEPTION_04){
	} else if(error_no == ERROR_CPU_EXCEPTION_05){
	} else if(error_no == ERROR_CPU_EXCEPTION_06){
	} else if(error_no == ERROR_CPU_EXCEPTION_07){
	} else if(error_no == ERROR_CPU_EXCEPTION_08){
	} else if(error_no == ERROR_CPU_EXCEPTION_09){
	} else if(error_no == ERROR_CPU_EXCEPTION_0A){
	} else if(error_no == ERROR_CPU_EXCEPTION_0B){
	} else if(error_no == ERROR_CPU_EXCEPTION_0C){
	} else if(error_no == ERROR_CPU_EXCEPTION_0D){
	} else if(error_no == ERROR_CPU_EXCEPTION_0E){
	} else if(error_no == ERROR_CPU_EXCEPTION_0F){
	} else if(error_no == ERROR_NO_MORE_SEGMENT){
		debug("No More Segment Descriptor(requested at 0x%08X).\n", *retaddr, *va_args);
		Error_Abort();
	} else{
		debug("[0x%08X]Error:Unknown Error Number.\n", *retaddr);
		Error_Abort();
	}
	return 0;
}

void Error_Abort(void)
{
	IO_CLI();
	for(;;){
		IO_HLT();
	}
}
