
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
	UI_Timer *ctimer;
	bool key_ignore;

	data = 0;

	mytask = System_MultiTask_GetNowTask();

	#ifdef CHNOSPROJECT_DEBUG
		debug("CMT:ConsoleMainTask[UI_Task=0x%X] Start Running.\n", mytask);
	#endif

	myconsole->printf_buffer = (uchar *)System_Memory_Allocate(CONSOLE_PRINTF_BUFFER_SIZE);

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

	ctimer = Timer_Initialize();
	Timer_Config(ctimer, 500, mytask->fifo, 1, True);
	Timer_Run(ctimer);

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
				if(data == 1){
					TextBox_Cursor_Blink(myconsole->textbox);
				}
			} else if(data < SIGNAL_KEY_OFFSET){
				data -= INPUTSIGNAL_OFFSET;
				if(data == INPUTSIGNAL_FOCUS_GOT){
					TextBox_SetEnable_CursorBlink(myconsole->textbox, True);
				} else if(data == INPUTSIGNAL_FOCUS_LOST){
					TextBox_SetEnable_CursorBlink(myconsole->textbox, False);
				}
				//入力通知領域
			} else if(data < SIGNAL_KEY_OFFSET + 0xffff){
				key_ignore = False;
				//keyid通知
				data -= SIGNAL_KEY_OFFSET;
				if(!(data & KEYID_MASK_BREAK) && (data & KEYID_MASK_EXTENDED)){
					if((data & KEYID_MASK_ID) == KEYID_ENTER){
						key_ignore = True;
						TextBox_SetEnable_RecordInputText(myconsole->textbox, False);
						TextBox_Put_Character(myconsole->textbox, '\n');
						if(Console_CompareCommandline_s(myconsole, "test")){
							TextBox_Put_String(myconsole->textbox, "Hello, World.");
						} else if(Console_CompareCommandline_s(myconsole, "mem")){
							Console_printf(myconsole, "Total:%10uBytes %5uKB\n", System_Get_PhisycalMemorySize(), System_Get_PhisycalMemorySize() >> 10);
							Console_printf(myconsole, "Free :%10uBytes %5uKB\n", System_Memory_Get_FreeSize(), System_Memory_Get_FreeSize() >> 10);
						} else{
							TextBox_Put_String(myconsole->textbox, "Console:No such file or command:");
							TextBox_Put_String(myconsole->textbox, myconsole->textbox->text_buf);
						}
						TextBox_Put_Character(myconsole->textbox, '\n');
						TextBox_Put_Character(myconsole->textbox, '>');
						TextBox_SetEnable_RecordInputText(myconsole->textbox, True);
					}
				}
				if(!key_ignore){
					TextBox_Put_Key(myconsole->textbox, data);
				}
			} else if(data < TCM_OFFSET){
				//汎用利用可能領域1
			} else{
				//TCM領域
			}
		}
	}
}

bool Console_CompareCommandline_s(UI_Console *myconsole, const uchar s[])
{
	return CFunction_CompareStrings(myconsole->textbox->text_buf, s);
}

uint Console_printf(UI_Console *myconsole, const uchar format[], ...)
{
	CFunction_vsnprintf(myconsole->printf_buffer, CONSOLE_PRINTF_BUFFER_SIZE, format, (uint *)(&format + 1));
	TextBox_Put_String(myconsole->textbox, myconsole->printf_buffer);
	return 0;
}

