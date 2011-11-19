
#include "core.h"

bool Error_Output_Enable_SerialPort = false;
bool Error_Output_Enable_Display_TextMode = false;

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
	"User ESP ",
	"User SS  "
};

uint Error_Report(uint error_no, ...)
{
	uint *retaddr, *va_args;

	retaddr = &error_no - 1;
	va_args = &error_no + 1;

	if(error_no <= ERROR_CPU_EXCEPTIONS){
		Error_Put_String("\nException 0x%02X:%s\n", error_no, cpu_exceptions[error_no]);
		if(error_no == ERROR_CPU_EXCEPTION_00){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_01){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_02){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_03){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_04){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_05){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_06){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_07){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_08){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_09){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_0A){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_0B){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_0C){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_0D){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_0E){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_0F){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_10){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_11){
			Error_CPU_Exception_Put_Registers_With_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_12){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_13){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_14){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_15){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_16){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_17){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_18){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_19){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_1A){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_1B){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_1C){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_1D){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_1E){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		} else if(error_no == ERROR_CPU_EXCEPTION_1F){
			Error_CPU_Exception_Put_Registers_Without_ErrorCode((uint *)*va_args);
		}
		Error_Abort();
	} else{
		Error_Put_String("\n[0x%08X]Error:0x%08X ", *retaddr, error_no);
		if(error_no == ERROR_NO_MORE_SEGMENT){
			Error_Put_String("No More Segment Descriptor(requested at 0x%08X).\n", *va_args);
			Error_Abort();
		} else if(error_no == ERROR_NOT_ENOUGH_FREE_MEMORY){
			Error_Put_String("No More Free Memory(Control:0x%08X Request Size:0x%08X).\n", *va_args, *(va_args + 1));
		} else if(error_no == ERROR_MEMORY_FREE_RANGE_OVERLAPPED){
			Error_Put_String("Memory Free Range Overlapped(Control:0x%08X TagIndex:%u).\n", *va_args, *(va_args + 1));
		} else if(error_no == ERROR_NO_MORE_FREE_TAG){
			Error_Put_String("No More Free Tag(Control:0x%08X).\n", *va_args);
		} else if(error_no == ERROR_INVALID_FREE_MEMORY_INDEX){
			Error_Put_String("Invalid Free Memory Index(Control:0x%08X TagIndex:%u).\n", *va_args, *(va_args + 1));
		} else{
			Error_Put_String("Unknown Error Number.\n");
			Error_Abort();
		}
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

void Error_Set_Enable_SerialPort(bool serial)
{
	Error_Output_Enable_SerialPort = serial;
	return;
}

void Error_Set_Enable_Display_TextMode(bool tdisp)
{
	Error_Output_Enable_Display_TextMode = tdisp;
	return;
}

int Error_Put_String(const uchar format[], ...)
{
	int i;
	uchar s[256];

	i = vsnprintf(s, format, sizeof(s), (uint *)(&format + 1));
	if(Error_Output_Enable_SerialPort){
		SerialPort_Send(s);
	}
	if(Error_Output_Enable_Display_TextMode){
		TextMode_Put_String(s, white);
	}
	return i;
}

void Error_CPU_Exception_Put_Registers_With_ErrorCode(uint *esp)
{
	uint i;

	Error_Put_String("#PUSHAD by _asm_CPU_ExceptionHandler\n");
	for(i = 0; i < 4; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X\n", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#PUSH by _asm_CPU_ExceptionHandler\n");
	for(; i < 5; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X\n", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#PUSH by CPU\n");
	for(; i < 8; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X\n", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#Control Registers\n");
	Error_Put_String("CR0 = 0x%08X\n", Load_CR0());
	Error_Put_String("CR2 = 0x%08X\n", Load_CR2());
	Error_Put_String("CR3 = 0x%08X\n", Load_CR3());
	Error_Put_String("CR4 = 0x%08X\n", Load_CR4());

	Error_Put_String("Opcode[0x%X]:0x%X\n", esp[0x0b], ((uchar *)(esp[0x0b]))[0]);
	return;
}

void Error_CPU_Exception_Put_Registers_Without_ErrorCode(uint *esp)
{
	uint i;

	Error_Put_String("#PUSHAD by _asm_CPU_ExceptionHandler\n");
	for(i = 0; i < 4; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X\n", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#PUSH by _asm_CPU_ExceptionHandler\n");
	for(; i < 5; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X\n", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#PUSH by CPU\n");
	Error_Put_String("%s:0x%08X\n", cpu_exception_infos[(i << 1) + 1], esp[i << 1]);
	i++;
	for(; i < 8; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X\n", cpu_exception_infos[i << 1], esp[(i << 1) - 1], cpu_exception_infos[(i << 1) + 1], esp[i << 1]);
	}

	Error_Put_String("#Control Registers\n");
	Error_Put_String("CR0 = 0x%08X\n", Load_CR0());
	Error_Put_String("CR2 = 0x%08X\n", Load_CR2());
	Error_Put_String("CR3 = 0x%08X\n", Load_CR3());
	return;
}
