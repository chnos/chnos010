
#include "core.h"

System_CommonData System;

uchar *SystemRunningPhaseText[] = {
	" 0:Initialising System (Protected 32bit Text Mode)"
};

void CHNMain(void)
{
	uchar s[128];
	uint i, emu_steps;
	CPU_ControlRegister0 cr0;
	Emulator_x86_Environment x86emu;

	IO_CLI();

	TextMode_Clear_Screen();
	Error_Set_Enable_Display_TextMode(true);

	TextMode_Put_String("Welcome to CHNOSProject!\n", green);

	System_Set_RunningPhase(0);

	TextMode_Put_String("\tInitialising SerialPort...\n", white);
	Initialise_SerialPort();
	Error_Set_Enable_SerialPort(true);

	TextMode_Put_String("\tInitialising Memory...\n", white);
	System_Check_Memory();
	i = System_Get_PhisycalMemorySize();
	snprintf(s, "\tMemory:%uByte %uKiB %uMib\n", sizeof(s), i, i >> 10, i >> 20);
	TextMode_Put_String(s, white);
	System.MemoryController = Memory_Initialise_Control((void *)PHYSICAL_MEMORY_ALLOCATION_START_ADDRESS, i - PHYSICAL_MEMORY_ALLOCATION_START_ADDRESS, SYSTEM_MEMORY_CONTROL_TAGS);

	i = Memory_Get_FreeSize(System.MemoryController);
	snprintf(s, "\tFreeMemory:%uByte %uKiB %uMib\n", sizeof(s), i, i >> 10, i >> 20);
	TextMode_Put_String(s, white);

	Memory_Allocate_Aligned(System.MemoryController, 4096, 4096);

	TextMode_Put_String("\tInitialising GDT...\n", white);
	Initialise_GlobalDescriptorTable();

	TextMode_Put_String("\tInitialising IDT...\n", white);
	Initialise_InterruptDescriptorTable();

	TextMode_Put_String("\tInitialising PIC...\n", white);
	Initialise_ProgrammableInterruptController();

	TextMode_Put_String("\tInitialising PIT...\n", white);
	Initialise_ProgrammableIntervalTimer();

	TextMode_Put_String("\tInitialising Keyboard...\n", white);
	Initialise_Keyboard();

	TextMode_Put_String("\tHardware Initialising Phase End.\n", white);

	snprintf(s, "TEST X %X\n", sizeof(s), 0xABCDEF32);
	TextMode_Put_String(s, white);

	snprintf(s, "TEST x %x\n", sizeof(s), 0xABCDEF32);
	TextMode_Put_String(s, white);

	snprintf(s, "TEST u %u\n", sizeof(s), 1234567890);
	TextMode_Put_String(s, white);

	snprintf(s, "TEST f %f\n", sizeof(s), 0.5);
	TextMode_Put_String(s, white);

	snprintf(s, "TEST c %c\n", sizeof(s), 'C');
	TextMode_Put_String(s, white);

	snprintf(s, "TEST s %s\n", sizeof(s), "TESTString.");
	TextMode_Put_String(s, white);

	snprintf(s, "TEST d %d\n", sizeof(s), -1234);
	TextMode_Put_String(s, white);

	snprintf(s, "TEST i %i\n", sizeof(s), -1234);
	TextMode_Put_String(s, white);

	cr0.cr0 = Load_CR0();

	snprintf(s, "CR0.PE=%d PG=%d\n", sizeof(s), cr0.bit.PE, cr0.bit.PG);
	TextMode_Put_String(s, white);

	IO_STI();

	Emulator_x86_Initialise(&x86emu);

	x86emu.EIP = 0xc200;
	x86emu.GReg[OPCODE_REG_ESP] = 0xc200;

	emu_steps = Emulator_x86_Execute_Auto(&x86emu);

	snprintf(s, "x86Emulator:Instructions=%d\n", sizeof(s), emu_steps);
	TextMode_Put_String(s, white);

	for (;;) {
		IO_HLT();
	}
}

void System_Set_RunningPhase(uint phase)
{
	System.RunningPhase = phase;
	TextMode_Put_String("\nNow SystemRunningPhase is", white);
	TextMode_Put_String(SystemRunningPhaseText[System.RunningPhase], skyblue);
	TextMode_Put_String("\n", white);

	return;
}

uint System_Get_RunningPhase(void)
{
	return System.RunningPhase;
}

void System_Check_Memory(void)
{
	System.PhysicalMemorySize = Memory_Test(0x00400000, 0xbfffffff);
	return;
}

uint System_Get_PhisycalMemorySize(void)
{
	return System.PhysicalMemorySize;
}


