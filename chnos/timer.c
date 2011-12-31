
#include "core.h"

uint PIT_10ms_Tick;
void (*TaskSwitch)(void);

void Initialise_ProgrammableIntervalTimer(void)
{
	PIT_10ms_Tick = 0;
	TaskSwitch = &Timer_TaskSwitch_Invalid;

	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	System_GateDescriptor_Set(0x20, (uint)asm_InterruptHandler20, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x00);

	return;
}

void InterruptHandler20(uint *esp)
{
	//uchar s[32];

	ProgrammableInterruptController_InterruptRequest_Complete(0x00);
	PIT_10ms_Tick++;

	//snprintf(s, sizeof(s), "10ms_Tick:%u", PIT_10ms_Tick);
	//TextMode_Put_String_Absolute(s, white, 50, 0);

	if((PIT_10ms_Tick & 0x00000002) == 0){
		TaskSwitch();
	}

	return;
}

void Timer_Set_TaskSwitch(void (*TaskSwitchFunction)(void))
{
	if(TaskSwitchFunction != 0){
		TaskSwitch = TaskSwitchFunction;
	} else{
		TaskSwitch = Timer_TaskSwitch_Invalid;
	}
	return;
}

void Timer_TaskSwitch_Invalid(void)
{
	return;
}

typedef struct UI_TIMER {
	uint tick;
	uint timeout;
	struct UI_TIMER *root_next;
	struct UI_TIMER *tree_next;
	DATA_FIFO32 *fifo;
	uint fifo_putdata;
	union UI_TIMER_FLAGS {
		uint flags;
		struct UI_TIMER_FLAGS_BITS {
			unsigned initialized : 1;
			unsigned configured : 1;
		} bit;
	} flags;
} UI_Timer;
