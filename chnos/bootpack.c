
#include "core.h"

System_CommonData System;

uchar *SystemRunningPhaseText[] = {
	" 0:Initialising Hardware (Protected 32bit Text Mode)",
	" 1:Initialising Software (Protected 32bit Text Mode)"
};

void CHNMain(void)
{
	uchar s[128];
	uint i;

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

	IO_STI();

	System_Set_RunningPhase(1);

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


