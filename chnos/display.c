
#include "core.h"

//VBEが使えるのは、ctrl->display_mode == DISPLAYMODE_VBE_LINEARの時だけ。

#define SIGNAL_BIOS_OPERATION_END	0xff

IO_DisplayControl *Initialise_Display(void)
{
	//最初に現在の画面モードをVGA320*200-8bitに設定する。(BIOS)

	IO_CallBIOSControl *callbiosctrl;
	INFO_VBE_BIOS *info_vbe_bios;
	uint data;
	IO_DisplayControl *ctrl;
	uint i;
	INFO_VBE_VideoMode *info_vbe_vmode;

	ctrl = (IO_DisplayControl *)System_Memory_Allocate(sizeof(IO_DisplayControl));

	ctrl->bios_signal = System_FIFO32_Initialise(64);

	callbiosctrl = System_CallBIOS_Get_Controller();
	callbiosctrl->CallBIOS_Task->tss->eax = 0x0013;
	System_CallBIOS_Execute(0x10, ctrl->bios_signal, SIGNAL_BIOS_OPERATION_END);

	for(;;){
		if(FIFO32_Status(ctrl->bios_signal) == 0){

		} else{
			data = FIFO32_Get(ctrl->bios_signal);
			if(data == SIGNAL_BIOS_OPERATION_END || data == SIGNAL_BIOS_OPERATION_END + 1){
				break;
			}
		}
	}

	ctrl->vmode = 0x0000;
	ctrl->vmode_index = 0;
	ctrl->bpp = 8;
	ctrl->xsize = VGA08_VRAM_XSIZE;
	ctrl->ysize = VGA08_VRAM_YSIZE;
	ctrl->vram = VGA_VRAM_ADR;

	Drawing08_Initialise_Palette();
	Error_Set_Enable_Display_TextMode(false);
	Error_Set_Enable_Display_GraphicMode(true, ctrl->vram, ctrl->xsize, ctrl->ysize >> 4);


	//次に、VBEのBIOS情報を得る。

	callbiosctrl->CallBIOS_Task->tss->eax = 0x4f00;
	callbiosctrl->CallBIOS_Task->tss->es = 0x0000;
	callbiosctrl->CallBIOS_Task->tss->edi = 0x0e00;
	System_CallBIOS_Execute(0x10, ctrl->bios_signal, SIGNAL_BIOS_OPERATION_END);

	for(;;){
		if(FIFO32_Status(ctrl->bios_signal) == 0){

		} else{
			data = FIFO32_Get(ctrl->bios_signal);
			if(data == SIGNAL_BIOS_OPERATION_END || data == SIGNAL_BIOS_OPERATION_END + 1){
				break;
			}
		}
	}

	info_vbe_bios = VBE_REALMODE_RETVALUE_ADDRESS;

	if(data == SIGNAL_BIOS_OPERATION_END && (callbiosctrl->retvalue.eax & 0x000000ff) == 0x4f && (callbiosctrl->retvalue.eax & 0x0000ff00) == 0x00){
		ctrl->VBE.version_minor = info_vbe_bios->ver_minor;
		ctrl->VBE.version_major = info_vbe_bios->ver_major;
		ctrl->VBE.vram_supported_size_kb = info_vbe_bios->vram_supported_size_kb;
		ctrl->VBE.flags = (info_vbe_bios->flags[1] << 8) | info_vbe_bios->flags[0];
		ctrl->VBE.oem_string = (uchar *)(info_vbe_bios->oem_string_segment << 4 | info_vbe_bios->oem_string_offset);
		ctrl->VBE.vmode_args = (ushort *)(info_vbe_bios->vmode_args_segment << 4 | info_vbe_bios->vmode_args_offset);
		if(ctrl->VBE.version_major >= 0x02){
			ctrl->display_mode = DISPLAYMODE_VBE_LINEAR;
		} else{
			ctrl->display_mode = DISPLAYMODE_VBE_WINDOW;
		}
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("Display:VBE is Enbaled(Version %d.%d).\n", ctrl->VBE.version_major, ctrl->VBE.version_minor);
			debug("Display:OEM:[%s]\n", ctrl->VBE.oem_string);
		#endif
	} else{
		ctrl->display_mode = DISPLAYMODE_BIOS;
	}

	if(ctrl->display_mode == DISPLAYMODE_VBE_LINEAR){
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("Display:Getting Video Modes...\n");
		#endif
		for(i = 0; ctrl->VBE.vmode_args[i] != 0xffff; i++){

		}
		ctrl->VBE.list_vmode_tags = i;
		ctrl->VBE.list_vmode = (INFO_VBE_VideoModeTag *)System_Memory_Allocate(sizeof(INFO_VBE_VideoModeTag) * ctrl->VBE.list_vmode_tags);
		info_vbe_vmode = VBE_REALMODE_RETVALUE_ADDRESS;
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("Display:VBE has %d Video Modes.\n", ctrl->VBE.list_vmode_tags);
		#endif
		for(i = 0; i < ctrl->VBE.list_vmode_tags; i++){
			callbiosctrl->CallBIOS_Task->tss->eax = 0x4f01;
			callbiosctrl->CallBIOS_Task->tss->es = 0x0000;
			callbiosctrl->CallBIOS_Task->tss->edi = 0x0e00;
			callbiosctrl->CallBIOS_Task->tss->ecx = ctrl->VBE.vmode_args[i];
			System_CallBIOS_Execute(0x10, ctrl->bios_signal, SIGNAL_BIOS_OPERATION_END);

			for(;;){
				if(FIFO32_Status(ctrl->bios_signal) == 0){

				} else{
					data = FIFO32_Get(ctrl->bios_signal);
					if(data == SIGNAL_BIOS_OPERATION_END || data == SIGNAL_BIOS_OPERATION_END + 1){
						break;
					}
				}
			}

			if(data == SIGNAL_BIOS_OPERATION_END && (callbiosctrl->retvalue.eax & 0x000000ff) == 0x4f && (callbiosctrl->retvalue.eax & 0x0000ff00) == 0x00){
				ctrl->VBE.list_vmode[i].mode_number = ctrl->VBE.vmode_args[i];
				ctrl->VBE.list_vmode[i].xsize = info_vbe_vmode->XResolution;
				ctrl->VBE.list_vmode[i].ysize = info_vbe_vmode->YResolution;
				ctrl->VBE.list_vmode[i].bpp = info_vbe_vmode->BitsPerPixel;
				ctrl->VBE.list_vmode[i].memory_model = info_vbe_vmode->MemoryModel;
				ctrl->VBE.list_vmode[i].attribute = info_vbe_vmode->ModeAttributes;
				ctrl->VBE.list_vmode[i].vram = info_vbe_vmode->PhysBasePtr;
				#ifdef CHNOSPROJECT_DEBUG_DISPLAY
					debug("Display:%d:0x%X (%dx%d-%dbit) [0x%X]\nDisplay:", i, ctrl->VBE.list_vmode[i].mode_number, ctrl->VBE.list_vmode[i].xsize, ctrl->VBE.list_vmode[i].ysize, ctrl->VBE.list_vmode[i].bpp, ctrl->VBE.list_vmode[i].vram);
					if(ctrl->VBE.list_vmode[i].memory_model == 0x04){
						debug(" 256-palette");
					}
					if(ctrl->VBE.list_vmode[i].memory_model == 0x06){
						debug(" direct-color");
					}
					if((ctrl->VBE.list_vmode[i].attribute & (1 << 3)) != 0){
						debug(" color");
					} else{
						debug(" monochrome");
					}
					if((ctrl->VBE.list_vmode[i].attribute & (1 << 4)) != 0){
						debug(" graphic");
					} else{
						debug(" text");
					}
					if((ctrl->VBE.list_vmode[i].attribute & (1 << 7)) != 0){
						debug(" linear-access-supported");
					}
					debug("\n");
					debug("%d:0x%X %dx%d-%dbits\n", 0, ctrl->VBE.list_vmode[0].mode_number, ctrl->VBE.list_vmode[0].xsize, ctrl->VBE.list_vmode[0].ysize, ctrl->VBE.list_vmode[0].bpp);

				#endif
			} else{
				ctrl->VBE.list_vmode[i].mode_number = ctrl->VBE.vmode_args[i];
				ctrl->VBE.list_vmode[i].vram = (void *)0x00000000;
				#ifdef CHNOSPROJECT_DEBUG_DISPLAY
					debug("Display:%d:0x%X Function Failed.\n", i, ctrl->VBE.list_vmode[i].mode_number);
				#endif
			}
		}


	}

	#ifdef CHNOSPROJECT_DEBUG_DISPLAY
		debug("Display:Ctrl:[0x%X]\n", ctrl);
		debug("Display:ctrl->VBE.list_vmode[0]:[0x%X]\n", &ctrl->VBE.list_vmode[0]);
		debug("Display:ctrl->bios_signal->buf:[0x%X]\n", ctrl->bios_signal->buf);
	#endif

	return ctrl;
}

uint Display_VESA_Set_VideoMode(IO_DisplayControl *ctrl, uint index)
{
	//retvalue:0 = 切り替え成功
	//retvalue:1 = 切り替え失敗(BIOS Call Error)
	//retvalue:2 = 切り替え失敗(VBE無効 version < 2.0)
	//retvalue:3 = 切り替え失敗(Invalid VideoMode Index)
	//retvalue:4 = 切り替え失敗(Linear Mode Not Supported)
	//retvalue:5 = 切り替え失敗(システム(CHNOSProject)上で未対応のビット数のため)
	//retvalue:6 = 切り替え失敗(システム(CHNOSProject)上で無効とマークされている(vram == null)ため)

	IO_CallBIOSControl *callbiosctrl;
	uint data;

	callbiosctrl = System_CallBIOS_Get_Controller();

	if(ctrl->display_mode != DISPLAYMODE_VBE_LINEAR){
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("SetVideo:Error:VBE version < 2.0.\n");
		#endif
		return 2;
	}

	if(index >= ctrl->VBE.list_vmode_tags){
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("SetVideo:Error:Invalid VideoMode Index.\n");
		#endif
		return 3;
	}

	if((ctrl->VBE.list_vmode[index].attribute & (1 << 7)) == 0){
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("SetVideo:Error:This VideoMode is Not Supported Linear Mode.\n");
		#endif
		return 4;
	}

	if(!(ctrl->VBE.list_vmode[index].bpp == 8 || ctrl->VBE.list_vmode[index].bpp == 16 || ctrl->VBE.list_vmode[index].bpp == 32)){
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("SetVideo:Error:This VideoMode's BitsPerPixel is Not Supported in CHNOSProject.\n");
		#endif
		return 5;
	}

	if(ctrl->VBE.list_vmode[index].vram == 0x00000000){
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("SetVideo:Error:This VideoMode has been Marked Invalid(vram==null).\n");
		#endif
		return 6;
	}

	callbiosctrl->CallBIOS_Task->tss->eax = 0x4f02;
	callbiosctrl->CallBIOS_Task->tss->ebx = ctrl->VBE.list_vmode[index].mode_number | VESA_LINEAR_ACCESS;
	System_CallBIOS_Execute(0x10, ctrl->bios_signal, SIGNAL_BIOS_OPERATION_END);

	for(;;){
		if(FIFO32_Status(ctrl->bios_signal) == 0){

		} else{
			data = FIFO32_Get(ctrl->bios_signal);
			if(data == SIGNAL_BIOS_OPERATION_END || data == SIGNAL_BIOS_OPERATION_END + 1){
				break;
			}
		}
	}

	if(data == SIGNAL_BIOS_OPERATION_END && (callbiosctrl->retvalue.eax & 0x000000ff) == 0x4f && (callbiosctrl->retvalue.eax & 0x0000ff00) == 0x00){
		ctrl->vmode = ctrl->VBE.list_vmode[index].mode_number;
		ctrl->vmode_index = index;
		ctrl->bpp = ctrl->VBE.list_vmode[index].bpp;
		ctrl->xsize = ctrl->VBE.list_vmode[index].xsize;
		ctrl->ysize = ctrl->VBE.list_vmode[index].ysize;
		ctrl->vram = ctrl->VBE.list_vmode[index].vram;
		#ifdef CHNOSPROJECT_DEBUG_DISPLAY
			debug("Display:VBE VideoMode Changed.\nDisplay:%d:0x%X (%dx%d-%dbit) [0x%X]\nDisplay:", index, ctrl->VBE.list_vmode[index].mode_number, ctrl->VBE.list_vmode[index].xsize, ctrl->VBE.list_vmode[index].ysize, ctrl->VBE.list_vmode[index].bpp, ctrl->VBE.list_vmode[index].vram);
			if(ctrl->VBE.list_vmode[index].memory_model == 0x04){
				debug(" 256-palette");
			}
			if(ctrl->VBE.list_vmode[index].memory_model == 0x06){
				debug(" direct-color");
			}
			if((ctrl->VBE.list_vmode[index].attribute & (1 << 3)) != 0){
				debug(" color");
			} else{
				debug(" monochrome");
			}
			if((ctrl->VBE.list_vmode[index].attribute & (1 << 4)) != 0){
				debug(" graphic");
			} else{
				debug(" text");
			}
			if((ctrl->VBE.list_vmode[index].attribute & (1 << 7)) != 0){
				debug(" linear-access-supported");
			}
			debug("\n");
		#endif
		Initialise_Drawing();
		Error_Set_Enable_Display_GraphicMode(true, ctrl->vram, ctrl->xsize, ctrl->ysize >> 4);
		return 0;
	}

	#ifdef CHNOSPROJECT_DEBUG_DISPLAY
		debug("Display:%d:0x%X Function Failed.\n", index, ctrl->VBE.list_vmode[index].mode_number);
	#endif

	return 1;
}
