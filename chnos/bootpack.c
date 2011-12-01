
#include "core.h"

void TestTask2(void);

void CHNMain(void)
{
	//uchar s[128];
	uint i, data;
	UI_Task *KBCT, *test2;
	UI_Task *mytask;
	IO_CallBIOSControl *callbiosctrl;

	Initialise_System();

	mytask = System_MultiTask_GetNowTask();

	KBCT = System_MultiTask_Task_Initialise(0);
	KBCT->tss->eip = (uint)&KeyboardControlTask;
	KBCT->tss->cs = SYSTEM_CS << 3;
	KBCT->tss->ss = SYSTEM_DS << 3;
	KBCT->tss->ds = SYSTEM_DS << 3;
	KBCT->tss->esp = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	System_MultiTask_Task_Run(KBCT);

	test2 = System_MultiTask_Task_Initialise(0);
	test2->tss->eip = (uint)&TestTask2;
	test2->tss->cs = SYSTEM_CS << 3;
	test2->tss->ss = SYSTEM_DS << 3;
	test2->tss->ds = SYSTEM_DS << 3;
	test2->tss->esp = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	System_MultiTask_Task_Run(test2);

	i = 0;

	callbiosctrl = System_CallBIOS_Get_Controller();
	callbiosctrl->CallBIOS_Task->tss->eax = 0x0013;
	System_CallBIOS_Execute(0x10, mytask->fifo, 0xff);

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			if(data == 0xff){
				#ifdef CHNOSPROJECT_DEBUG_CALLBIOS
					debug("Main:Receive BIOS Control End.\n");
				#endif
				break;
			}
		}
	}

	Drawing08_Initialise_Palette();
	Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0xffffff, 0, 0, VGA08_VRAM_XSIZE - 1, VGA08_VRAM_YSIZE - 1);
	Drawing08_Fill_Rectangle(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 0x00ff00, 100, 100, VGA08_VRAM_XSIZE - 10 - 1, VGA08_VRAM_YSIZE - 10);
	Drawing08_Put_String(VGA_VRAM_ADR, VGA08_VRAM_XSIZE, 10, 10, 0x000000, "Welcome to CHNOSProject!");

	for(;;){

	}
}

void KeyboardControlTask(void)
{
	UI_Task *mytask;
	uchar s[128];
	uint data;

	mytask = System_MultiTask_GetNowTask();

	snprintf(s, "KBCT:KeyboardControlTask Start Running.\nKBCT:UI_Task=0x%X\n", sizeof(s), mytask);
	TextMode_Put_String(s, white);

	Keyboard_Set_ReceiveFIFO(mytask->fifo, 0x100);

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			if(0x100 <= data && data <= 0x1ff){
				data -= 0x100;
				snprintf(s, "KBCT:KeyCode=0x%X\n", sizeof(s), data);
				TextMode_Put_String(s, white);
			}
		}
	}
}

void TestTask2(void)
{
	uchar s[128];
	uint i;
	UI_Task *mytask;

	i = 0;

	mytask = System_MultiTask_GetNowTask();

	for(;;){
		i++;
		snprintf(s, "TestTask2=%d=%d", sizeof(s), mytask->count, i);
		TextMode_Put_String_Absolute(s, white, 0, 3);
	}
}
