
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

void Console_MainTask(UI_Console *console)
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

	console->printf_buffer = (uchar *)System_Memory_Allocate(CONSOLE_PRINTF_BUFFER_SIZE);

	TextBox_Show(console->textbox, SHEET_MAX_CHILDREN, (int)(console->textbox->sheet->parent->size.x >> 1) - (int)(console->textbox->sheet->size.x >> 1), (int)(console->textbox->sheet->parent->size.y >> 1) - (int)(console->textbox->sheet->size.y >> 1));
	if(console->textbox->sheet->location.x < 0){
		Sheet_Slide_Absolute(console->textbox->sheet, 0, SHEET_LOCATION_NOCHANGE);
	}
	if(console->textbox->sheet->location.y < 0){
		Sheet_Slide_Absolute(console->textbox->sheet, SHEET_LOCATION_NOCHANGE, 0);
	}
	TextBox_Put_Character(console->textbox, '>');
	TextBox_SetEnable_RecordInputText(console->textbox, True);
	console->flags.bit.isprompt = True;

	ctimer = Timer_Initialize();
	Timer_Config(ctimer, 500, mytask->fifo, 1, True);
	Timer_Run(ctimer);

	console->boot_fd = FloppyDisk_Initialize(ADR_DISKIMG);

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
					TextBox_Cursor_Blink(console->textbox);
				}
			} else if(data < SIGNAL_KEY_OFFSET){
				data -= INPUTSIGNAL_OFFSET;
				if(data == INPUTSIGNAL_FOCUS_GOT){
					TextBox_SetEnable_CursorBlink(console->textbox, True);
				} else if(data == INPUTSIGNAL_FOCUS_LOST){
					TextBox_SetEnable_CursorBlink(console->textbox, False);
				}
				//入力通知領域
			} else if(data < SIGNAL_KEY_OFFSET + 0xffff){
				key_ignore = False;
				//keyid通知
				data -= SIGNAL_KEY_OFFSET;
				if(!(data & KEYID_MASK_BREAK) && (data & KEYID_MASK_EXTENDED)){
					if((data & KEYID_MASK_ID) == KEYID_ENTER){
						key_ignore = True;
						TextBox_SetEnable_RecordInputText(console->textbox, False);
						TextBox_Put_Character(console->textbox, '\n');
						if(Console_CompareCommandline_n(console, "test", 4)){
							TextBox_Put_String(console->textbox, "Hello, World.\n");
							if(console->textbox->text_buf[4] == ' '){
								Console_printf(console, "value16:0x%X\n", strtol(&console->textbox->text_buf[5], Null, 0));
								Console_printf(console, "value10:%u\n", strtol(&console->textbox->text_buf[5], Null, 0));
							}
						} else if(Console_CompareCommandline_s(console, "mem")){
							Console_printf(console, "Total:%10uBytes %5uKB\n", System_Get_PhisycalMemorySize(), System_Get_PhisycalMemorySize() >> 10);
							Console_printf(console, "Free :%10uBytes %5uKB\n", System_Memory_Get_FreeSize(), System_Memory_Get_FreeSize() >> 10);
						} else if(Console_CompareCommandline_s(console, "dir")){
							Console_Command_dir(console);
						} else if(Console_CompareCommandline_n(console, "pci", 3)){
							Console_Command_pci(console);
						} else if(Console_CompareCommandline_n(console, "type", 4)){
							Console_Command_type(console);
						} else{
							TextBox_Put_String(console->textbox, "Console:There is no such file or command:");
							TextBox_Put_String(console->textbox, console->textbox->text_buf);
						}
						TextBox_Put_Character(console->textbox, '\n');
						TextBox_Put_Character(console->textbox, '>');
						TextBox_SetEnable_RecordInputText(console->textbox, True);
					}
				}
				if(!key_ignore){
					TextBox_Put_Key(console->textbox, data);
				}
			} else if(data < TCM_OFFSET){
				//汎用利用可能領域1
			} else{
				//TCM領域
			}
		}
	}
}

bool Console_CompareCommandline_s(UI_Console *console, const uchar s[])
{
	return CFunction_CompareStrings(console->textbox->text_buf, s);
}

bool Console_CompareCommandline_n(UI_Console *console, const uchar s[], unsigned int n)
{
	return CFunction_CompareStrings_n(console->textbox->text_buf, s, n);
}

uint Console_printf(UI_Console *console, const uchar format[], ...)
{
	CFunction_vsnprintf(console->printf_buffer, CONSOLE_PRINTF_BUFFER_SIZE, format, (uint *)(&format + 1));
	TextBox_Put_String(console->textbox, console->printf_buffer);
	return 0;
}

uint Console_Command_dir(UI_Console *console)
{
	uint i, j;
	IO_FloppyDisk_DirectoryEntry_UpdateTime rdetime;
	IO_FloppyDisk_DirectoryEntry_UpdateDate rdedate;
	IO_FloppyDisk_DirectoryEntry_Attribute rdeattr;

	for(i = 0; i < FLOPPYDISK_RDE_ENTRIES; i++){
		if(console->boot_fd->files[i].name[0] == 0x00){
			break;
		}
		rdeattr.attribute = console->boot_fd->files[i].attribute;
		if(console->boot_fd->files[i].name[0] != 0xe5 && console->boot_fd->files[i].name[0] != 0x05 && !rdeattr.bit.volumelabel && !rdeattr.bit.directory){
			for(j = 0; j < 8; j++){
				TextBox_Put_Character(console->textbox, console->boot_fd->files[i].name[j]);
			}
			TextBox_Put_Character(console->textbox, '.');
			for(j = 0; j < 3; j++){
				TextBox_Put_Character(console->textbox, console->boot_fd->files[i].ext[j]);
			}
			Console_printf(console, " %6d", console->boot_fd->files[i].size);
			rdedate.updatedate = console->boot_fd->files[i].updatedate;
			Console_printf(console, " %04d/%02d/%02d", rdedate.bit.year + 1980, rdedate.bit.month, rdedate.bit.day);
			rdetime.updatetime = console->boot_fd->files[i].updatetime;
			Console_printf(console, " %02d:%02d:%02d\n", rdetime.bit.hour, rdetime.bit.minute, rdetime.bit.second << 1);
		}
	}

	return 0;
}

uint Console_Command_pci(UI_Console *console)
{
	uchar *p;
	uint bus, device, function;
	uint data;

	TextBox_Put_String(console->textbox, "-<pci information>-\n");
	if(CFunction_String_GetWord(console->textbox->text_buf, &p, 1)){
		if(CFunction_String_GetWord(console->textbox->text_buf, &p, 3)){
			function = strtol(p, Null, 0);
			CFunction_String_GetWord(console->textbox->text_buf, &p, 2);
			device = strtol(p, Null, 0);
			CFunction_String_GetWord(console->textbox->text_buf, &p, 1);
			bus = strtol(p, Null, 0);
			Console_printf(console, "Bus%3d.Device%2d.Function%2d:\n", bus, device, function);
			if(bus < 256 && device < 32 && function < 8){
				PCI_ConfigurationRegister_SelectDevice(bus, device, function);
				data = PCI_ConfigurationRegister_Read32(0x00);
				if(data == 0xffffffff){
					TextBox_Put_String(console->textbox, "Device not exist.\n");
				} else{
					Console_printf(console, "DeviceVendor:%s(0x%04X)\n", PCI_GetDeviceVendor(data & 0xffff), data & 0xffff);
					Console_printf(console, "DeviceID    :0x%04X\n", data >> 16);

					data = PCI_ConfigurationRegister_Read32(0x08);
					data = CFunction_ExtractBits(data, 8, 31);
					Console_printf(console, "ClassCode   :%s(0x%06X)\n", PCI_GetDeviceClass(data), data);

					data = PCI_ConfigurationRegister_Read32(0x0c);
					data = CFunction_ExtractBits(data, 16, 22);
					Console_printf(console, "DeviceType  :%s(%d)\n", PCI_GetDeviceType(data), data);
				}
			} else{
				TextBox_Put_String(console->textbox, "Invalid Device.\n");
			}
		}
	} else{
		TextBox_Put_String(console->textbox, "Usage:");
		TextBox_Put_String(console->textbox, "pci <bus> <device> <function>\n\nDevices which exist:\n");
		for(bus = 0; bus < 256; bus++){
			for(device = 0; device < 32; device++){
				for(function = 0; function < 8; function++){
					PCI_ConfigurationRegister_SelectDevice(bus, device, function);
					data = PCI_ConfigurationRegister_Read32(0x00);
					if(data != 0xffffffff){
						Console_printf(console, "Bus%3d.Device%2d.Function%2d\n", bus, device, function);
					}
				}
			}
		}
	}
	return 0;
}

uint Console_Command_type(UI_Console *console)
{
	uchar *p;
	IO_File *file;
	uint i;

	TextBox_Put_String(console->textbox, "-<type>-");
	if(CFunction_String_GetWord(console->textbox->text_buf, &p, 1)){
			if(FloppyDisk_IsPathExist(console->boot_fd, p)){
				TextBox_Put_String(console->textbox, ":");
				TextBox_Put_String(console->textbox, p);
				TextBox_Put_String(console->textbox, "\n");
				file = File_Initilaize();
				if(FloppyDisk_LoadFile(console->boot_fd, file, p) == 0){
					for(i = 0; i < file->size; i++){
						TextBox_Put_Character(console->textbox, ((uchar *)file->img)[i]);
					}
				} else{
					TextBox_Put_String(console->textbox, "type:File load Error.\n");
				}
				File_Free(file);
			} else{
				TextBox_Put_String(console->textbox, "\ntype:The path is not exist.\n");
			}
	} else{
		TextBox_Put_String(console->textbox, "\nUsage:");
		TextBox_Put_String(console->textbox, "type filepath\n");
	}
	return 0;
}


