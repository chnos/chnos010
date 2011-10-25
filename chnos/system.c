
#include "core.h"

#define PHYSICAL_MEMORY_ALLOCATION_START_ADDRESS	0x00400000
#define SYSTEM_MEMORY_CONTROL_TAGS	1024
//
typedef struct SYSTEM_COMMONDATA {
	uint RunningPhase;
	uint PhysicalMemorySize;
	IO_MemoryControl MemoryController;
	UI_TaskControl *TaskController;
} System_CommonData;
//
System_CommonData System;

uchar *SystemRunningPhaseText[] = {
	" 0:Initialising System (Protected 32bit Text Mode)"
};
//
void System_Check_Memory(void);

void Initialise_System(void)
{
	uint i;
	uchar s[128];

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

	TextMode_Put_String("\tInitialising MultiTask...\n", white);
	System.TaskController = Initialise_MultiTask_Control(System.MemoryController);
	Timer_Set_TaskSwitch(&System_TaskSwitch);

	TextMode_Put_String("\tSystem Initialising Phase End.\n", white);

	IO_STI();
	return;
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

uint System_Get_PhisycalMemorySize(void)
{
	return System.PhysicalMemorySize;
}

void System_SegmentDescriptor_Set_Absolute(uint selector, uint limit, uint base, uint ar)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if(selector >= 8192){
		return;
	}

	SegmentDescriptor_Set(&gdt[selector], limit, base, ar);

	return;
}

uint System_SegmentDescriptor_Get_Base(uint selector)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if(selector >= 8192){
		return 0;
	}

	return SegmentDescriptor_Get_Base(&gdt[selector]);
}

uint System_SegmentDescriptor_Get_Limit(uint selector)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if(selector >= 8192){
		return 0;
	}

	return SegmentDescriptor_Get_Limit(&gdt[selector]);
}

uint System_SegmentDescriptor_Get_AccessRight(uint selector)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if(selector >= 8192){
		return 0;
	}

	return SegmentDescriptor_Get_AccessRight(&gdt[selector]);
}

uint System_SegmentDescriptor_Set(uint limit, uint base, uint ar)
{
	uint *retaddr;
	uint i;

	retaddr = &limit - 1;

	for(i = 1; i < 8192; i++){
		if(System_SegmentDescriptor_Get_Limit(i) == 0){
			System_SegmentDescriptor_Set_Absolute(i, limit, base, ar);
			return i;
		}
	}

	Error_Report(ERROR_NO_MORE_SEGMENT, *retaddr);
	for(;;){
		IO_HLT();
	}

	return 0;
}

void System_GateDescriptor_Set(uint irq, uint offset, uint selector, uint ar)
{
	IO_GateDescriptor *idt = (IO_GateDescriptor *)ADR_IDT;

	GateDescriptor_Set(&idt[irq], offset, selector, ar);

	return;
}

void System_TaskSwitch(void)
{
	MultiTask_TaskSwitch(System.TaskController);
	return;
}

UI_Task *System_MultiTask_Task_Initialise(void)
{
	return MultiTask_Task_Initialise(System.TaskController);
}

void System_MultiTask_Task_Run(UI_Task *task)
{
	MultiTask_Task_Run(System.TaskController, task);
	return;
}

void *System_Memory_Allocate(uint size)
{
	return Memory_Allocate(System.MemoryController, size);
}

UI_Task *System_MultiTask_GetNowTask(void)
{
	return MultiTask_GetNowTask(System.TaskController);
}

//
void System_Check_Memory(void)
{
	System.PhysicalMemorySize = Memory_Test(0x00400000, 0xbfffffff);
	return;
}

