
#include "core.h"

uint PIT_10ms_Tick;

void Initialise_ProgrammableIntervalTimer(void)
{
	PIT_10ms_Tick = 0;

	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	System_GateDescriptor_Set(0x20, (uint)asm_InterruptHandler20, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x00);

	return;
}

void InterruptHandler20(uint *esp)
{
	uchar s[32];

	ProgrammableInterruptController_InterruptRequest_Complete(0x00);
	PIT_10ms_Tick++;

	sprintf(s, "10ms_Tick:%u", PIT_10ms_Tick);
	TextMode_Put_String_Absolute(s, white, 50, 0);

	return;
}
