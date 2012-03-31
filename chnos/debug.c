#include "core.h"

//DR7:RWn Breakpoint Condition
//	CR4.DE==1
//		00:Execute Instruction
//		01:Write Data
//		10:I/O R/W
//		11:Data R/W
//	CR4.DE==0
//		00:Execute Operation
//		01:Write Data
//		10:(Undefined)
//		11:Data R/W

//DR7:LENn Address Location Size
//	00:Byte(or Instruction)
//	01:Word
//	10:(Undefined)
//	11:Double Word

#ifdef CHNOSPROJECT_DEBUG

extern uchar *cpu_exception_infos[16];	//error.c
extern uint Error_Output_Display_GraphicMode_UsedLines; //error.c

uint debug_exception_last_addr;

void debug(const uchar format[], ...)
{
	uchar s[256];

	vsnprintf(s, sizeof(s), format, (uint *)(&format + 1));
	SerialPort_Send(s);
	return;
}

void Debug_PhysicalMemoryDump(void *addr, uint bytes)
{
	uchar *p;
	uint i, j;
	uint eflags;

	bytes += (uint)addr - ((uint)addr & 0xfffffff0);
	(uint)addr &= 0xfffffff0; 

	j = (bytes + 0x0f) >> 4;

	eflags = IO_Load_EFlags();
	IO_CLI();

	debug("\n***PhysicalMemoryDump Start***\n");
	debug("Dump from[0x%08X] for 0x%X bytes.\n", addr, j << 4);
	debug("Address+|00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F|\n");
	for(i = 0; i < j; i++){
		p = (uchar *)((uint)addr + (i << 4));
		debug("%08X:%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X|\n", p, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
	}
	debug("***PhysicalMemoryDump End***\n");

	IO_Store_EFlags(eflags);

	return;
}

void Debug_Set_Breakpoint(uint reg, void *addr, uint rw, uint len)
{
	CPU_DebugRegister7 dr7;

	dr7.dr7 = Load_DR7();

	dr7.bit.LE = True;
	dr7.bit.GE = True;

	switch(reg){
		case 0:
			Store_DR0((uint)addr);
			dr7.bit.L0 = True;
			dr7.bit.G0 = True;
			dr7.bit.RW0 = rw;
			dr7.bit.LEN0 = len;
			debug("Debug_Set_Breakpoint:Breakpoint%d Enabled(target:[0x%08X] rw:%d len:%d).\n", reg, addr, dr7.bit.RW0, dr7.bit.LEN0);
			break;
	}

	Store_DR7(dr7.dr7);

	return;
}

void Debug_ExceptionHandler(uint *esp)
{
	uint i, j;
	IO_SegmentDescriptor *gdt;

	gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if(esp[0x0a] == debug_exception_last_addr){
		debug("Debug_ExceptionHandler:Ignore debug exception(same eip.)\n");
		return;
	}

	Error_Put_String("Exception 0x01 Debug. CHNOSProject is running in debug mode.");

	Error_Put_String("#PUSHAD by _asm_CPU_ExceptionHandler");
	for(i = 0; i < 4; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#PUSH by _asm_CPU_ExceptionHandler");
	for(; i < 5; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
	}

	Error_Put_String("#PUSH by CPU");
	Error_Put_String("%s:0x%08X", cpu_exception_infos[(i << 1) + 1], esp[i << 1]);
	i++;
	for(; i < 8; i++){
		Error_Put_String("%s:0x%08X %s:0x%08X", cpu_exception_infos[i << 1], esp[(i << 1) - 1], cpu_exception_infos[(i << 1) + 1], esp[i << 1]);
	}

	Error_Put_String("#Control Registers");
	Error_Put_String("CR0 = 0x%08X", Load_CR0());
	Error_Put_String("CR2 = 0x%08X", Load_CR2());
	Error_Put_String("CR3 = 0x%08X", Load_CR3());
	Error_Put_String("#Debug Registers");
	Error_Put_String("DR0 = 0x%08X", Load_DR0());
	Error_Put_String("DR1 = 0x%08X", Load_DR1());
	Error_Put_String("DR2 = 0x%08X", Load_DR2());
	Error_Put_String("DR3 = 0x%08X", Load_DR3());
	Error_Put_String("DR6 = 0x%08X", Load_DR6());
	Error_Put_String("DR7 = 0x%08X", Load_DR7());


	Error_Put_String("Opcode[0x%X:0x%X]:0x%X", SegmentDescriptor_Get_Base(&gdt[esp[0x0b] >> 3]), esp[0x0a], ((uchar *)(SegmentDescriptor_Get_Base(&gdt[esp[0x0b] >> 3])))[esp[0x0a]]);
	Error_Put_String("Press any key to continue.");

	debug_exception_last_addr = esp[0x0a];

	if(((uchar *)(SegmentDescriptor_Get_Base(&gdt[esp[0x0b] >> 3])))[esp[0x0a]] == 0x89){
		esp[0x0a] += 3;
	}

Debug_PhysicalMemoryDump((void *)Load_DR0(), 32);

//Wait press any key.
	for(;;){
		if((IO_In8(PORT_KEYSTA) & 0x01) != 0){
			break;
		}
	}

//Clear Display.
	j = Error_Output_Display_GraphicMode_UsedLines;
	Error_Output_Display_GraphicMode_UsedLines = 0;
	for(i = 0; i < j; i++){
		Error_Put_String("");
	}
	Error_Output_Display_GraphicMode_UsedLines = 0;

	return;
}

#endif
