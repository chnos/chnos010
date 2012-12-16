
#include "core.h"

#define PHYSICAL_MEMORY_ALLOCATION_START_ADDRESS	0x00400000
#define SYSTEM_MEMORY_CONTROL_TAGS	1024
//
typedef union CPUID_FUNCTION_FLAGS {
	struct CPUID_FUNCTION_FLAGS_REG {
		uint edx;
		uint ecx;
	} reg;
	struct CPUID_FUNCTION_FLAGS_BITS {
		/*EAX=1,EDX*/
		unsigned FPU : 1;
		unsigned VME : 1;
		unsigned DE : 1;
		unsigned PSE : 1;
		unsigned TSC : 1;
		unsigned MSR : 1;
		unsigned PAE : 1;
		unsigned MCE : 1;
		unsigned CX8 : 1;
		unsigned APIC : 1;
		unsigned edx_bit10 : 1;
		unsigned SEP : 1;
		unsigned MTRR : 1;
		unsigned PGE : 1;
		unsigned MCA : 1;
		unsigned CMOV : 1;
		unsigned PAT : 1;
		unsigned PSE36 : 1;
		unsigned PSN : 1;
		unsigned CLFSH : 1;
		unsigned edx_bit20 : 1;
		unsigned DS : 1;
		unsigned ACPI : 1;
		unsigned MMX : 1;
		unsigned FXSR : 1;
		unsigned SSE : 1;
		unsigned SSE2 : 1;
		unsigned SS : 1;
		unsigned HTT : 1;
		unsigned TM : 1;
		unsigned edx_bit30 : 1;
		unsigned PBE : 1;
		/*EAX=1,ECX*/
		unsigned SSE3 : 1;
		unsigned ecx_bit1 : 1;
		unsigned ecx_bit2 : 1;
		unsigned MONITOR : 1;
		unsigned DSCPL : 1;
		unsigned ecx_bit5 : 1;
		unsigned ecx_bit6 : 1;
		unsigned EST : 1;
		unsigned TM2 : 1;
		unsigned ecx_bit9 : 1;
		unsigned CID : 1;
		unsigned ecx_bit11 : 1;
		unsigned ecx_bit12 : 1;
		unsigned CX16 : 1;
		unsigned XTPR : 1;
		unsigned ecx_bit15 : 1;
		unsigned ecx_bit16 : 1;
		unsigned ecx_bit17 : 1;
		unsigned ecx_bit18 : 1;
		unsigned ecx_bit19 : 1;
		unsigned ecx_bit20 : 1;
		unsigned ecx_bit21 : 1;
		unsigned ecx_bit22 : 1;
		unsigned ecx_bit23 : 1;
		unsigned ecx_bit24 : 1;
		unsigned ecx_bit25 : 1;
		unsigned ecx_bit26 : 1;
		unsigned ecx_bit27 : 1;
		unsigned ecx_bit28 : 1;
		unsigned ecx_bit29 : 1;
		unsigned ecx_bit30 : 1;
		unsigned ecx_bit31 : 1;
	} bit;
} CPUID_FunctionFlags;

typedef struct SYSTEM_COMMONDATA {
	uint RunningPhase;
	DATA_FIFO32 *InputFocus;
	struct SYSTEM_COMMONDATA_CONTROLLER {
		IO_MemoryControl Memory;
		UI_TaskControl *Task;
		IO_CallBIOSControl *CallBIOS;
		IO_DisplayControl *Display;
	} Controller;
	struct SYSTEM_COMMONDATA_ENVIRONMENT {
		struct SYSTEM_COMMONDATA_ENVIRONMENT_MEMORY {
			uint PhysicalSize;
		} Memory;
		struct SYSTEM_COMMONDATA_ENVIRONMENT_CPUID {
			uint max_id;
			uint max_eid;
			CPUID_FunctionFlags function_flags;
		} CPUID;
	} Environment;
	struct SYSTEM_COMMONDATA_CORETASK {
		UI_Task *Main;
		UI_Task *KeyboardControl;
		UI_Task *MouseControl;
	} CoreTask;
} System_CommonData;
//
System_CommonData System;

uchar *SystemRunningPhaseText[] = {
	" 0:Initialising System (Protected 32bit Text Mode)"
};
//
void System_Check_Memory(void);
uint System_CPUID(void *addr, uint id);

void Initialize_System(void)
{
	uint i;
	uchar s[128];
	uint cpuid_buf[4];
	CPU_EFlags eflags;
	CPU_ControlRegister4 cr4;

	IO_CLI();

	TextMode_Clear_Screen();
	Error_Set_Enable_Display_TextMode(True);

	TextMode_Put_String("Welcome to CHNOSProject!\n", green);

	System_Set_RunningPhase(0);

	TextMode_Put_String("\tInitialising SerialPort...\n", white);
	Initialize_SerialPort();
	Error_Set_Enable_SerialPort(True);

	#ifdef CHNOSPROJECT_DEBUG
		debug("%s:%d\n", __FILE__, __LINE__);
		debug("CHNOSProject is Running in Debug Mode.\n");
	#endif

//Config Control Register4
	cr4.cr4 = Load_CR4();
	cr4.bit.DE = True;
	Store_CR4(cr4.cr4);

	TextMode_Put_String("\tInitialising Memory...\n", white);
	System_Check_Memory();
	i = System_Get_PhisycalMemorySize();
	snprintf(s, sizeof(s), "\tMemory:%uByte %uKiB %uMib\n", i, i >> 10, i >> 20);
	TextMode_Put_String(s, white);
	System.Controller.Memory = Memory_Initialize_Control((void *)PHYSICAL_MEMORY_ALLOCATION_START_ADDRESS, i - PHYSICAL_MEMORY_ALLOCATION_START_ADDRESS, SYSTEM_MEMORY_CONTROL_TAGS);

	i = Memory_Get_FreeSize(System.Controller.Memory);
	snprintf(s, sizeof(s), "\tFreeMemory:%uByte %uKiB %uMib\n", i, i >> 10, i >> 20);
	TextMode_Put_String(s, white);

	TextMode_Put_String("\tInitialising GDT...\n", white);
	Initialize_GlobalDescriptorTable();

	TextMode_Put_String("\tInitialising IDT...\n", white);
	Initialize_InterruptDescriptorTable();

	TextMode_Put_String("\tInitialising PIC...\n", white);
	Initialize_ProgrammableInterruptController();

	TextMode_Put_String("\tInitialising PIT...\n", white);
	Initialize_ProgrammableIntervalTimer();

	TextMode_Put_String("\tInitialising Keyboard...\n", white);
	Initialize_Keyboard();

	TextMode_Put_String("\tInitialising MultiTask...\n", white);
	System.Controller.Task = Initialize_MultiTask_Control(System.Controller.Memory);
	Timer_Set_TaskSwitch(&System_TaskSwitch);
	System.CoreTask.Main = System_MultiTask_GetNowTask();
	System.InputFocus = System.CoreTask.Main->fifo;

	TextMode_Put_String("\tInitialising CallBIOS...\n", white);
	System.Controller.CallBIOS = Initialize_CallBIOS();

	TextMode_Put_String("\tReading CPU Identification...\n", white);
	eflags.eflags = IO_Load_EFlags();
	eflags.bit.ID = True;
	IO_Store_EFlags(eflags.eflags);
	eflags.eflags = IO_Load_EFlags();
	if(!eflags.bit.ID){
		TextMode_Put_String("\t\tCPUID is Disabled.\n", white);
		System.Environment.CPUID.max_id = 0xffffffff;
		System.Environment.CPUID.max_eid = 0;
	} else{
		eflags.bit.ID = False;
		IO_Store_EFlags(eflags.eflags);
		eflags.eflags = IO_Load_EFlags();
		if(eflags.bit.ID){
			TextMode_Put_String("\t\tCPUID is Disabled.\n", white);
			System.Environment.CPUID.max_id = 0xffffffff;
			System.Environment.CPUID.max_eid = 0;
		}
	}
	if(System.Environment.CPUID.max_id != 0xffffffff){
		System.Environment.CPUID.max_id = 0;
		TextMode_Put_String("\t\tCPUID is Enabled.\n", white);
		CPUID(cpuid_buf, 0);
		System.Environment.CPUID.max_id = cpuid_buf[0];
		snprintf(s, sizeof(s), "\t\tMaxID =0x%X\n", System.Environment.CPUID.max_id);
		TextMode_Put_String(s, white);
		CPUID(cpuid_buf, 0x80000000);
		if((cpuid_buf[0] & 0x80000000) == 0){
			TextMode_Put_String("\t\tExtended CPUID is Disabled.\n", white);
		} else{
			TextMode_Put_String("\t\tExtended CPUID is Enabled.\n", white);
			System.Environment.CPUID.max_eid = cpuid_buf[0];
			snprintf(s, sizeof(s), "\t\tMaxEID=0x%X\n", System.Environment.CPUID.max_eid);
			TextMode_Put_String(s, white);
		}
		CPUID(s, 0);
		s[16] = 0x00;
		TextMode_Put_String("\t\t", white);
		TextMode_Put_String(s + 4, white);
		TextMode_Put_String("\n", white);
		CPUID(cpuid_buf, 1);
		System.Environment.CPUID.function_flags.reg.edx = cpuid_buf[2];
		System.Environment.CPUID.function_flags.reg.ecx = cpuid_buf[3];
		snprintf(s, sizeof(s), "\t\tVME:%d\n", System.Environment.CPUID.function_flags.bit.VME);
		TextMode_Put_String(s, white);
	}

	TextMode_Put_String("\tInitialising PCI...\n", white);
	Initialize_PCI();

	TextMode_Put_String("\tSystem Initialising Phase End.\n", white);

	IO_STI();

	System.Controller.Display = Initialize_Display();

//Core Task Run.

	System.CoreTask.KeyboardControl = System_MultiTask_Task_Initialize(0);
	System.CoreTask.KeyboardControl->tss->eip = (uint)&KeyboardControlTask;
	System.CoreTask.KeyboardControl->tss->cs = SYSTEM_CS << 3;
	System.CoreTask.KeyboardControl->tss->ss = SYSTEM_DS << 3;
	System.CoreTask.KeyboardControl->tss->ds = SYSTEM_DS << 3;
	System.CoreTask.KeyboardControl->tss->esp = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	MultiTask_Push_Arguments(System.CoreTask.KeyboardControl, 1, &System.InputFocus);
	System_MultiTask_Task_Run(System.CoreTask.KeyboardControl);

	System.CoreTask.MouseControl = System_MultiTask_Task_Initialize(0);
	System.CoreTask.MouseControl->tss->eip = (uint)&MouseControlTask;
	System.CoreTask.MouseControl->tss->cs = SYSTEM_CS << 3;
	System.CoreTask.MouseControl->tss->ss = SYSTEM_DS << 3;
	System.CoreTask.MouseControl->tss->ds = SYSTEM_DS << 3;
	System.CoreTask.MouseControl->tss->esp = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	MultiTask_Push_Arguments(System.CoreTask.MouseControl, 2, &System.InputFocus, MouseCursor_Initialize(System.Controller.Display->vramsheet));
	System_MultiTask_Task_Run(System.CoreTask.MouseControl);

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
	return System.Environment.Memory.PhysicalSize;
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
	MultiTask_TaskSwitch(System.Controller.Task);
	return;
}

UI_Task *System_MultiTask_Task_Initialize(uint tss_additional_size)
{
	return MultiTask_Task_Initialize(System.Controller.Task, tss_additional_size);
}

void System_MultiTask_Task_Run(UI_Task *task)
{
	#ifdef CHNOSPROJECT_DEBUG_CALLLINK
		debug("System_MultiTask_Task_Run:Called from[0x%08X].\n", *((uint *)(&task - 1)));
	#endif
	MultiTask_Task_Run(System.Controller.Task, task);
	return;
}

void *System_Memory_Allocate(uint size)
{
	return Memory_Allocate(System.Controller.Memory, size);
}

UI_Task *System_MultiTask_GetNowTask(void)
{
	return MultiTask_GetNowTask(System.Controller.Task);
}

IO_CallBIOSControl *System_CallBIOS_Get_Controller(void)
{
	return System.Controller.CallBIOS;
}

void System_CallBIOS_Execute(uchar intn, DATA_FIFO32 *fifo, uint endsignal)
{
	CallBIOS_Execute(System.Controller.CallBIOS, intn, fifo, endsignal);
	return;
}

void System_Memory_Free(void *addr, uint size)
{
	Memory_Free(System.Controller.Memory, addr, size);
	return;
}

void System_CallBIOS_Send_End_Of_Operation(uint abort)
{
	CallBIOS_Send_End_Of_Operation(System.Controller.CallBIOS, abort);
	return;
}

void System_MultiTask_Task_Sleep(UI_Task *task)
{
	MultiTask_Task_Sleep(System.Controller.Task, task);
	return;
}

void System_MultiTask_Task_Kill(UI_Task *task)
{
	MultiTask_Task_Kill(System.Controller.Task, task);
	return;
}

DATA_FIFO32 *System_FIFO32_Initialize(uint size)
{
	return FIFO32_Initialize(System.Controller.Memory, size);
}

uint System_Display_VESA_Set_VideoMode(uint index)
{
	return Display_VESA_Set_VideoMode(System.Controller.Display, index);
}

IO_DisplayControl *System_Display_Get_Controller(void)
{
	return System.Controller.Display;
}

uint System_Memory_Get_FreeSize(void)
{
	return Memory_Get_FreeSize(System.Controller.Memory);
}

uint System_TaskControlMessage_Send_AllTask(uint message)
{
	UI_Task *task;
	uint sended_tasks;

	sended_tasks = 0;
	for(task = System.Controller.Task->start; task != Null; task = task->next){
		if(task->fifo != Null){
			sended_tasks++;
			FIFO32_Put(task->fifo, message);
		}
	}

	return sended_tasks;
}

uint System_Sheet_SetParentToVRAM(UI_Sheet *sheet)
{
	return Sheet_SetParent(sheet, System.Controller.Display->vramsheet);
}

uint System_InputFocus_Change(DATA_FIFO32 *fifo)
{
	if(System.InputFocus != fifo){
		FIFO32_Put(System.InputFocus, INPUTSIGNAL_OFFSET + INPUTSIGNAL_FOCUS_LOST);
		System.InputFocus = fifo;
		FIFO32_Put(System.InputFocus, INPUTSIGNAL_OFFSET + INPUTSIGNAL_FOCUS_GOT);
	}
	return 0;
}

UI_TaskControl *System_MultiTask_GetController(void)
{
	return System.Controller.Task;
}

//
void System_Check_Memory(void)
{
	System.Environment.Memory.PhysicalSize = Memory_Test(0x00400000, 0xbfffffff);
	return;
}

uint System_CPUID(void *addr, uint id)	//addr番地のuint[4]に、CPUの識別情報id番をEAX・EBX・EDX・ECXの順番で格納する。
{					//CPUIDが利用不可の場合は、戻り値は0xffffffff、利用可能なら、最大入力値（基本情報の場合）を返す。
	if(System.Environment.CPUID.max_id == 0xffffffff){
		return 0;
	}

	if(id <= System.Environment.CPUID.max_id || (0x80000000 <= id && id <= System.Environment.CPUID.max_eid)){
		CPUID(addr, id);
	}

	return System.Environment.CPUID.max_id;
}

