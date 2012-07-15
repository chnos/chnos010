
#include "core.h"

UI_Console *Console_Initialize(void)
{
	UI_Console *console;

	console = (UI_Console *)System_CommonStruct_Allocate(SYSTEM_STRUCTID_CONSOLE);

	console->task = System_MultiTask_Task_Initialize(0);
	console->task->tss->eip = (uint)&Console_MainTask;
	console->task->tss->cs = SYSTEM_CS << 3;
	console->task->tss->ss = SYSTEM_DS << 3;
	console->task->tss->ds = SYSTEM_DS << 3;
	console->task->tss->esp = (uint)System_Memory_Allocate(1024 * 32) + (1024 * 32);
	MultiTask_Push_Arguments(console->task, 1, console);

	console->textbox = TextBox_Initialize();

	console->flags.bit.initialized = True;

	#ifdef CHNOSPROJECT_DEBUG_CONSOLE
		debug("Console_Initialize:initialized[0x%X]\n", console);
	#endif

	return console;
}

uint Console_SetSize(UI_Console *console, uint xchars, uint ychars)
{
	if(console == Null){
		return 1;
	}

	if(!console->flags.bit.initialized){
		return 2;
	}

	if(console->flags.bit.configured_size){
		return 3;
	}

	TextBox_SetBuffer(console->textbox, xchars, ychars, 8, Null);
	System_Sheet_SetParentToVRAM(console->textbox->sheet);
	Sheet_SetMovable(console->textbox->sheet, True);
	Sheet_SetInputFIFO(console->textbox->sheet, console->task->fifo);

	console->flags.bit.configured_size = True;

	#ifdef CHNOSPROJECT_DEBUG_CONSOLE
		debug("Console_SetSize:configured[0x%X]\n", console);
	#endif

	return 0;
}

uint Console_Run(UI_Console *console)
{
	if(console == Null){
		return 1;
	}

	if(!console->flags.bit.initialized){
		return 2;
	}

	if(!console->flags.bit.configured_size){
		return 3;
	}

	#ifdef CHNOSPROJECT_DEBUG_CONSOLE
		debug("Console_Run:start running[0x%X]\n", console);
	#endif

	System_MultiTask_Task_Run(console->task);
	return 0;
}

void Console_MainTask(UI_Console *myconsole)
{
	UI_Task *mytask;
	uint data;

	data = 0;

	mytask = System_MultiTask_GetNowTask();

	#ifdef CHNOSPROJECT_DEBUG
		debug("CMT:ConsoleMainTask[UI_Task=0x%X] Start Running.\n", mytask);
	#endif

	TextBox_Show(myconsole->textbox, SHEET_MAX_CHILDREN, (int)(myconsole->textbox->sheet->parent->size.x >> 1) - (int)(myconsole->textbox->sheet->size.x >> 1), (int)(myconsole->textbox->sheet->parent->size.y >> 1) - (int)(myconsole->textbox->sheet->size.y >> 1));
	if(myconsole->textbox->sheet->location.x < 0){
		Sheet_Slide_Absolute(myconsole->textbox->sheet, 0, SHEET_LOCATION_NOCHANGE);
	}
	if(myconsole->textbox->sheet->location.y < 0){
		Sheet_Slide_Absolute(myconsole->textbox->sheet, SHEET_LOCATION_NOCHANGE, 0);
	}
	TextBox_Put_Character(myconsole->textbox, '>');
	TextBox_SetEnable_RecordInputText(myconsole->textbox, True);
	myconsole->flags.bit.isprompt = True;

	for(;;){
		if(FIFO32_MyTaskFIFO_Status() == 0){
			System_MultiTask_Task_Sleep(mytask);
		} else{
			data = FIFO32_MyTaskFIFO_Get();
			#ifdef CHNOSPROJECT_DEBUG_CMT
				debug("CMT:Receive data from FIFO(data:0x%X).\n", data);
			#endif
			if(data < INPUTSIGNAL_OFFSET){
				//汎用利用可能領域0
			} else if(data < SIGNAL_KEY_OFFSET){
				//入力通知領域
			} else if(data < SIGNAL_KEY_OFFSET + 0xffff){
				//keyid通知
				data -= SIGNAL_KEY_OFFSET;
				TextBox_Put_Character(myconsole->textbox, data);
			} else if(data < TCM_OFFSET){
				//汎用利用可能領域1
			} else{
				//TCM領域
			}
		}
	}
}
