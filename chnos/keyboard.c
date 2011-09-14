
#include "core.h"

void Initialise_Keyboard(void)
{
	System_GateDescriptor_Set(0x21, (uint)asm_InterruptHandler21, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x01);
}

void InterruptHandler21(uint *esp)
{
	uchar s[32];
	uint data;

	data = IO_In8(KEYB_DATA);

	ProgrammableInterruptController_InterruptRequest_Complete(0x01);

	snprintf(s, "KBD:0x%X", sizeof(s), data);
	TextMode_Put_String_Absolute(s, white, 40, 0);

	return;
}
