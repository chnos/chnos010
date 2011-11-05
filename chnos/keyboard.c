
#include "core.h"

uint kbd_data0;
DATA_FIFO32 *kbd_fifo;

void Initialise_Keyboard(void)
{
	System_GateDescriptor_Set(0x21, (uint)asm_InterruptHandler21, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x01);
	kbd_data0 = 0;
	kbd_fifo = 0;
	return;
}

void InterruptHandler21(uint *esp)
{
	uint data;

	data = IO_In8(KEYB_DATA);

	ProgrammableInterruptController_InterruptRequest_Complete(0x01);

	if(kbd_fifo != 0){
		FIFO32_Put(kbd_fifo, data + kbd_data0);
	}

	return;
}

void Keyboard_Set_ReceiveFIFO(DATA_FIFO32 *fifo, uint data0)
{
	kbd_data0 = data0;
	kbd_fifo = fifo;

	return; 
}
