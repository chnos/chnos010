
/*includes*/
#include "coredef0.h"	/*�V�X�e���萔�錾*/
#include "coredef1.h"	/*�V�X�e���f�[�^�^�錾*/
#include "coredef2.h"	/*�V�X�e���O�����\�[�X�錾*/
#include "coredef3.h"	/*�V�X�e��FIFO�R�}���h�錾*/
#include "keyid.h"	/*KeyIdentifier �L�[ID�錾*/

/*functions*/
/*bootpack.c �����*/
void KeyboardControlTask(void);

/*callbios.c 32bit����BIOS���R�[�����邽�߂̊֐��Q*/

IO_CallBIOSControl *Initialise_CallBIOS(void);
void CallBIOS_Execute(IO_CallBIOSControl *ctrl, uchar intn, DATA_FIFO32 *fifo, uint endsignal);
void CallBIOS_Send_End_Of_Operation(IO_CallBIOSControl *ctrl, uint abort);
void CallBIOS_Check_Privileged_Operation(uint *esp);
uint CallBIOS_Push_Data_To_Stack(uint *esp, ushort data);
uint CallBIOS_Pop_Data_From_Stack(uint *esp);

/*cfunc.c vsnprintf�̓Ǝ�������*/
int snprintf(uchar s[], const uchar format[], uint n, ...);
int vsnprintf(uchar s[], const uchar format[], uint n, uint vargs[]);
//
int CFunction_vsnprintf(uchar s[], const uchar format[], uint n, uint vargs[]);
void CFunction_vsnprintf_Initialise_WorkArea(CFunction_vsnprintf_WorkArea *work, uchar s[], const uchar format[], uint n, uint vargs[]);
int CFunction_vsnprintf_Check_FormatBuffer(CFunction_vsnprintf_WorkArea *work);
int CFunction_vsnprintf_Check_DestinationBuffer(CFunction_vsnprintf_WorkArea *work);
uchar CFunction_vsnprintf_Read_FormatBuffer(CFunction_vsnprintf_WorkArea *work);
void CFunction_vsnprintf_Write_DestinationBuffer(CFunction_vsnprintf_WorkArea *work, uchar c);
void CFunction_vsnprintf_End(CFunction_vsnprintf_WorkArea *work);
uint CFunction_vsnprintf_Get_NextArgument(CFunction_vsnprintf_WorkArea *work);
void CFunction_vsnprintf_To_String_From_Hex_Upper(CFunction_vsnprintf_WorkArea *work, uint hex);
void CFunction_vsnprintf_To_String_From_Hex_Lower(CFunction_vsnprintf_WorkArea *work, uint hex);
void CFunction_vsnprintf_To_String_From_Decimal_Unsigned(CFunction_vsnprintf_WorkArea *work, uint d);

/*display.c �f�B�X�v���C����֘A*/
IO_DisplayControl *Initialise_Display(void);
uint Display_VESA_Set_VideoMode(IO_DisplayControl *ctrl, uint index);

/*draw08.c 8bit�`��֘A*/
void Drawing08_Initialise_Palette(void);
void Drawing08_Set_Palette(uint start, uint end, uchar *rgb);
void Drawing08_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Drawing08_Put_Font(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font);
void Drawing08_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar s[]);

/*draw16.c 16bit�`��֘A*/
void Drawing16_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Drawing16_Put_Font(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font);
void Drawing16_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar s[]);

/*draw32.c 32bit�`��֘A*/
void Drawing32_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Drawing32_Put_Font(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font);
void Drawing32_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar s[]);

/*drawing.c �`��֘A*/
extern void (*Drawing_Put_String)(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
extern void (*Drawing_Fill_Rectangle)(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Drawing_Invalid_Put_String(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
void Drawing_Invalid_Fill_Rectangle(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Initialise_Drawing(void);
uchar RGB_32_To_08(uint c32);
uchar RGB_32_To_08_xy(uint c32, int x, int y);
ushort RGB_32_To_16(uint c32);

/*dsctbl.c �Z�O�����g�E�Q�[�g�f�B�X�N���v�^�֘A*/
void Initialise_GlobalDescriptorTable(void);
void Initialise_InterruptDescriptorTable(void);
void SegmentDescriptor_Set(IO_SegmentDescriptor *seg_desc, uint limit, uint base, uint ar);
uint SegmentDescriptor_Get_Base(IO_SegmentDescriptor *seg_desc);
uint SegmentDescriptor_Get_Limit(IO_SegmentDescriptor *seg_desc);
uint SegmentDescriptor_Get_AccessRight(IO_SegmentDescriptor *seg_desc);
void GateDescriptor_Set(IO_GateDescriptor *gate_desc, uint offset, uint selector, uint ar);

/*emu86.c x86�G�~�����[�^�[�֘A*/
void Emulator_x86_Initialise(Emulator_x86_Environment *env);
uint Emulator_x86_Execute(Emulator_x86_Environment *env);
uint Emulator_x86_Execute_Auto(Emulator_x86_Environment *env);
int Emulator_x86_Put_EmulationInformation(Emulator_x86_Environment *env, const uchar format[], ...);
uint Emulator_x86_FetchCode(Emulator_x86_Environment *env, uint bytes);
void Emulator_x86_InstructionPointer_Increment(Emulator_x86_Environment *env);
uint Emulator_x86_Get_EffectivePhysicalAddress(Emulator_x86_Environment *env, uint sreg, uint offset);
uint Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(Emulator_x86_Environment *env, uint modrm);
void Emulator_x86_MoveToGReg(Emulator_x86_Environment *env, uint reg, uint data, uint fullsize);
uint Emulator_x86_MoveFromGReg(Emulator_x86_Environment *env, uint reg, uint fullsize);
void Emulator_x86_MoveToSReg(Emulator_x86_Environment *env, uint sreg, ushort selector);
void Emulator_x86_Push_Data_To_Stack(Emulator_x86_Environment *env, uint data, uint size_dword);
uint Emulator_x86_Pop_Data_From_Stack(Emulator_x86_Environment *env, uint size_dword);
void Emulator_x86_Push_eIP_To_Stack(Emulator_x86_Environment *env);
void Emulator_x86_Pop_eIP_From_Stack(Emulator_x86_Environment *env);
void Emulator_x86_Push_eFLAGS_To_Stack(Emulator_x86_Environment *env);
void Emulator_x86_Pop_eFLAGS_From_Stack(Emulator_x86_Environment *env);
void Emulator_x86_Push_SReg_To_Stack(Emulator_x86_Environment *env, uint sreg);
void Emulator_x86_Pop_SReg_From_Stack(Emulator_x86_Environment *env, uint sreg);
//
void Emulator_x86_Operation_MOV_To_Reg_FullSize(Emulator_x86_Environment *env);
void Emulator_x86_Operation_MOV_To_SegReg(Emulator_x86_Environment *env);
void Emulator_x86_Operation_MOV_To_ByteReg(Emulator_x86_Environment *env);
void Emulator_x86_Operation_INTn(Emulator_x86_Environment *env);
void Emulator_x86_Operation_LEA(Emulator_x86_Environment *env);
void Emulator_x86_Operation_CALL_Near_Relative(Emulator_x86_Environment *env);
void Emulator_x86_Operation_PUSHA(Emulator_x86_Environment *env);
void Emulator_x86_Operation_POPA(Emulator_x86_Environment *env);
void Emulator_x86_Operation_MOV_To_ByteReg_Gb_Eb(Emulator_x86_Environment *env);
void Emulator_x86_Operation_CMP_AL(Emulator_x86_Environment *env);
void Emulator_x86_Operation_Jcc_JE_rel8(Emulator_x86_Environment *env);
void Emulator_x86_Operation_INC_RegOnly(Emulator_x86_Environment *env);
void Emulator_x86_Operation_JMP_rel8(Emulator_x86_Environment *env);
void Emulator_x86_Operation_RET_Near(Emulator_x86_Environment *env);
void Emulator_x86_Operation_PUSH_RegOnly(Emulator_x86_Environment *env);
void Emulator_x86_Operation_XOR_Eb_Gb(Emulator_x86_Environment *env);
void Emulator_x86_Operation_DEC_RegOnly(Emulator_x86_Environment *env);
void Emulator_x86_Operation_MOV_eAX_Ov(Emulator_x86_Environment *env);
void Emulator_x86_Operation_MOV_Ov_eAX(Emulator_x86_Environment *env);
void Emulator_x86_Operation_OUT_AL(Emulator_x86_Environment *env);
void Emulator_x86_Operation_CMP_Gv_Ev(Emulator_x86_Environment *env);
void Emulator_x86_Operation_LOOP_Jv(Emulator_x86_Environment *env);
void Emulator_x86_Operation_POP_Ev(Emulator_x86_Environment *env);
void Emulator_x86_Operation_POP_RegOnly(Emulator_x86_Environment *env);
void Emulator_x86_Operation_Jcc_JNE_rel8(Emulator_x86_Environment *env);
void Emulator_x86_Operation_STI(Emulator_x86_Environment *env);
void Emulator_x86_Operation_SBB_Gv_Ev(Emulator_x86_Environment *env);
void Emulator_x86_Operation_MOV_Ev_Gv(Emulator_x86_Environment *env);
void Emulator_x86_Operation_PUSH_Ib(Emulator_x86_Environment *env);
void Emulator_x86_Operation_CLD(Emulator_x86_Environment *env);

/*emu86asm.nas x86�G�~�����[�^�[�֘A�A�Z���u���֐�*/
uint asm_Emulator_x86_Get_EFlags_Compare(uint first_op, uint second_op);	//eflags��Ԃ�
uint asm_Emulator_x86_Get_EFlags_Increment(uint first_op);
uint asm_Emulator_x86_Get_EFlags_XOR(uint first_op, uint second_op);
uint asm_Emulator_x86_Get_EFlags_Decrement(uint first_op);
uint asm_Emulator_x86_Get_EFlags_Subtract_with_Borrow(uint *first_op, uint second_op, uint borrow);

/*error.c �G���[�֘A*/
uint Error_Report(uint error_no, ...);
void Error_Abort(void);
void Error_Set_Enable_SerialPort(bool serial);
void Error_Set_Enable_Display_TextMode(bool tdisp);
int Error_Put_String(const uchar format[], ...);
void Error_CPU_Exception_Put_Registers_With_ErrorCode(uint *esp);
void Error_CPU_Exception_Put_Registers_Without_ErrorCode(uint *esp);

/*fifo.c FIFO�o�b�t�@�֘A*/
DATA_FIFO32 *FIFO32_Initialise(IO_MemoryControl memctrl, uint size);
int FIFO32_Put(DATA_FIFO32 *fifo, uint data);
int FIFO32_Put_Arguments(DATA_FIFO32 *fifo, uint args, ...);
uint FIFO32_Get(DATA_FIFO32 *fifo);
uint FIFO32_Status(DATA_FIFO32 *fifo);
void FIFO32_Free(DATA_FIFO32 *fifo);
uint FIFO32_MyTaskFIFO_Status(void);
uint FIFO32_MyTaskFIFO_Get(void);

/*intrpt.c ���荞�݊֘A*/
void Initialise_ProgrammableInterruptController(void);
void ProgrammableInterruptController_InterruptMask_Clear(uint irq);
void ProgrammableInterruptController_InterruptRequest_Complete(uint irq);
void InterruptHandler27(uint *esp);

/*keyboard.c �L�[�{�[�h�֘A*/
void Initialise_Keyboard(void);
void InterruptHandler21(uint *esp);
void Keyboard_Set_ReceiveFIFO(DATA_FIFO32 *fifo, uint data0);
ushort Keyboard_Decode_KeyCode(uchar keycode);

/*memory.c �������֘A*/
uint Memory_Test(uint start, uint end);
IO_MemoryControl Memory_Initialise_Control(void *start, uint size, uint tags);
void Memory_Free(IO_MemoryControl ctrl, void *addr, uint size);
void Memory_Free_Sub(IO_MemoryControl ctrl, uint tagno);
void *Memory_Allocate(IO_MemoryControl ctrl, uint size);
void *Memory_Allocate_Aligned(IO_MemoryControl ctrl, uint size, uint align);
uint Memory_Get_FreeSize(IO_MemoryControl ctrl);

/*mtask.c �}���`�^�X�N�֘A*/
UI_TaskControl *Initialise_MultiTask_Control(IO_MemoryControl sysmemctrl);
UI_Task *MultiTask_Task_Initialise(UI_TaskControl *ctrl, uint tss_additional_size);
void MultiTask_Task_Run(UI_TaskControl *ctrl, UI_Task *task);
void MultiTask_TaskSwitch(UI_TaskControl *ctrl);
void MultiTask_Task_Sleep(UI_TaskControl *ctrl, UI_Task *task);
UI_Task *MultiTask_GetNowTask(UI_TaskControl *ctrl);

/*serial.c �V���A���ʐM�֘A*/
void Initialise_SerialPort(void);
void SerialPort_Send(const uchar s[]);
//
#ifdef CHNOSPROJECT_DEBUG
void debug(const uchar format[], ...);
#endif

/*system.c �V�X�e���f�[�^�E�������֘A*/
void Initialise_System(void);
void System_Set_RunningPhase(uint phase);
uint System_Get_RunningPhase(void);
uint System_Get_PhisycalMemorySize(void);
void System_SegmentDescriptor_Set_Absolute(uint selector, uint limit, uint base, uint ar);
uint System_SegmentDescriptor_Get_Base(uint selector);
uint System_SegmentDescriptor_Get_Limit(uint selector);
uint System_SegmentDescriptor_Get_AccessRight(uint selector);
uint System_SegmentDescriptor_Set(uint limit, uint base, uint ar);
void System_GateDescriptor_Set(uint irq, uint offset, uint selector, uint ar);
void System_TaskSwitch(void);
UI_Task *System_MultiTask_Task_Initialise(uint tss_additional_size);
void System_MultiTask_Task_Run(UI_Task *task);
void *System_Memory_Allocate(uint size);
UI_Task *System_MultiTask_GetNowTask(void);
IO_CallBIOSControl *System_CallBIOS_Get_Controller(void);
void System_CallBIOS_Execute(uchar intn, DATA_FIFO32 *fifo, uint endsignal);
void System_Memory_Free(void *addr, uint size);
void System_CallBIOS_Send_End_Of_Operation(uint abort);
void System_MultiTask_Task_Sleep(UI_Task *task);
DATA_FIFO32 *System_FIFO32_Initialise(uint size);
uint System_Display_VESA_Set_VideoMode(uint index);
IO_DisplayControl *System_Display_Get_Controller(void);

/*timer.c �^�C�}�[�֘A*/
void Initialise_ProgrammableIntervalTimer(void);
void InterruptHandler20(uint *esp);
void Timer_Set_TaskSwitch(void (*TaskSwitchFunction)(void));
void Timer_TaskSwitch_Invalid(void);

/*vgatmode.c VGA�e�L�X�g���[�h�֘A*/
uchar VGA_CRTController_ReadRegister(uchar regno);
void VGA_CRTController_WriteRegister(uchar regno, uchar data);
void TextMode_Write_TextRAM(ushort index, uchar data);
void TextMode_Put_Character_Absolute(uchar c, col_text col, ushort location);
void TextMode_Put_String_Absolute(const uchar s[], col_text col, uint x, uint y);
void TextMode_Clear_Screen(void);
ushort TextMode_Get_CursorLocation(void);
void TextMode_Set_CursorLocation(ushort location);
void TextMode_Put_Character(uchar c, col_text col);
void TextMode_Newline(void);
void TextMode_Put_String(const uchar s[], col_text col);

/*xception.c ��O�֘A*/
void CPU_ExceptionHandler00(uint *esp);
void CPU_ExceptionHandler01(uint *esp);
void CPU_ExceptionHandler02(uint *esp);
void CPU_ExceptionHandler03(uint *esp);
void CPU_ExceptionHandler04(uint *esp);
void CPU_ExceptionHandler05(uint *esp);
void CPU_ExceptionHandler06(uint *esp);
void CPU_ExceptionHandler07(uint *esp);
void CPU_ExceptionHandler08(uint *esp);
void CPU_ExceptionHandler09(uint *esp);
void CPU_ExceptionHandler0a(uint *esp);
void CPU_ExceptionHandler0b(uint *esp);
void CPU_ExceptionHandler0c(uint *esp);
void CPU_ExceptionHandler0d(uint *esp);
void CPU_ExceptionHandler0e(uint *esp);
void CPU_ExceptionHandler0f(uint *esp);
void CPU_ExceptionHandler10(uint *esp);
void CPU_ExceptionHandler11(uint *esp);
void CPU_ExceptionHandler12(uint *esp);
void CPU_ExceptionHandler13(uint *esp);
void CPU_ExceptionHandler14(uint *esp);
void CPU_ExceptionHandler15(uint *esp);
void CPU_ExceptionHandler16(uint *esp);
void CPU_ExceptionHandler17(uint *esp);
void CPU_ExceptionHandler18(uint *esp);
void CPU_ExceptionHandler19(uint *esp);
void CPU_ExceptionHandler1a(uint *esp);
void CPU_ExceptionHandler1b(uint *esp);
void CPU_ExceptionHandler1c(uint *esp);
void CPU_ExceptionHandler1d(uint *esp);
void CPU_ExceptionHandler1e(uint *esp);
void CPU_ExceptionHandler1f(uint *esp);

/*nasfunc0.nas ���̊֐��ɑS���ˑ����Ȃ��A�Z���u���֐��Q*/
void IO_HLT(void);			//CPU���~������B���荞�݂�����ƍĊJ����B�������߁B
void IO_CLI(void);			//CPU�ւ̊O�����荞�݂��~������B�������߁BNMI�Ɨ�O�ɂ͌����Ȃ��B
void IO_STI(void);			//CPU�ւ̊O�����荞�݂��ĊJ������B�������߁B
void IO_STIHLT(void);			//CPU�ւ̊O�����荞�݂��ĊJ��������ACPU���~������Bsti��hlt�̊Ԃɔ������銄�荞�݂ւ̑΍�B�B
uint IO_In8(uint port);			//port�Ԃ�I/O�|�[�g����f�[�^���o�C�g�P�ʂŎ󂯎��B�߂�l�͎󂯎�����f�[�^�BI/O�������߁B
void IO_Out8(uint port, uint data);	//port�Ԃ�I/O�|�[�g��data���o�C�g�P�ʂŏo�͂���BI/O�������߁B
uint IO_In16(uint port);		//port�Ԃ�I/O�|�[�g����f�[�^�����[�h�P�ʂŎ󂯎��B�߂�l�͎󂯎�����f�[�^�BI/O�������߁B
void IO_Out16(uint port, uint data);	//port�Ԃ�I/O�|�[�g��data�����[�h�P�ʂŏo�͂���BI/O�������߁B
uint IO_In32(uint port);		//port�Ԃ�I/O�|�[�g����f�[�^���_�u�����[�h�P�ʂŎ󂯎��B�߂�l�͎󂯎�����f�[�^�BI/O�������߁B
void IO_Out32(uint port, uint data);	//port�Ԃ�I/O�|�[�g��data���_�u�����[�h�P�ʂŏo�͂���BI/O�������߁B
uint IO_Load_EFlags(void);		//EFLAGS���W�X�^��ǂݍ��ށB
void IO_Store_EFlags(uint eflags);	//EFLAGS���W�X�^��eflags�ɕύX����B
uint Load_CR0(void);			//�R���g���[�����W�X�^0��ǂݍ��ށB�������߁B
void Store_CR0(uint cr0);		//�R���g���[�����W�X�^0��cr0�ɕύX����B�������߁B
uint Load_CR2(void);			//�R���g���[�����W�X�^2��ǂݍ��ށB�������߁B
void Store_CR2(uint cr2);		//�R���g���[�����W�X�^2��cr2�ɕύX����B�������߁B
uint Load_CR3(void);			//�R���g���[�����W�X�^3��ǂݍ��ށB�������߁B
void Store_CR3(uint cr3);		//�R���g���[�����W�X�^3��cr3�ɕύX����B�������߁B
uint Load_CR4(void);			//�R���g���[�����W�X�^4��ǂݍ��ށB�������߁B
void Store_CR4(uint cr4);		//�R���g���[�����W�X�^4��cr4�ɕύX����B�������߁B
void Load_GDTR(uint limit, uint addr);	//�O���[�o���E�f�B�X�N���v�^�E�e�[�u���E���W�X�^(GDTR)�ɁAaddr����limit�͈̔͂�GDT�Ƃ��ă��[�h������B�������߁B
void Load_IDTR(uint limit, uint addr);	//�C���^���v�g�E�f�B�X�N���v�^�E�e�[�u���E���W�X�^(IDTR)�ɁAaddr����limit�͈̔͂�IDT�Ƃ��ă��[�h������B�������߁B
void Load_TR(uint tr);			//�Z�O�����g�E�Z���N�^tr���A�^�X�N���W�X�^�Ɍ��݂̃^�X�N�Ƃ��ă��[�h������B�������߁B
void FarJMP(uint eip, uint cs);		//���݂̃Z�O�����g�Ƃ͈قȂ�Z�O�����gcs�̃I�t�Z�b�geip�Ƀv���O�����̐�����ڂ��B
void FarCall(uint eip, uint cs);	//���݂̃Z�O�����g�Ƃ͈قȂ�Z�O�����gcs�̃I�t�Z�b�geip�Ƀv���O�����̐���𕪊򂳂���B
void CLTS(void);			//�R���g���[���E���W�X�^0���ɂ���ATask-Switched�t���O���N���A����BFPU�̐���ɗ��p�B�������߁B
void FNSave(uint *addr);		//FPU�������addr����n�܂�108�o�C�g�Ɋi�[���AFPU������������B�ڍׂȃ`�F�b�N�͍s��Ȃ��B
void FRStore(uint *addr);		//FPU�������addr����n�܂�108�o�C�g���畜������B
void PIT_Beep_On(void);			//�r�[�v�����J�n����B
void PIT_Beep_Off(void);		//�r�[�v�����~����B
void PIT_Beep_Set(uint fq);		//�r�[�v���̎��g�����AfqHz�ɕύX����B
					//fq�̒l
					//C:262 C#:277 D:294 D#:311 E:330 F:349 F#:370 G:392 G#:415 A:440 A#:466 B:494 C:523
void CPUID(void *addr, uint id);	//addr�Ԓn��uint[4]�ɁACPU�̎��ʏ��id�Ԃ�EAX�EEBX�EEDX�EECX�̏��ԂŊi�[����B
void CPUID2(void *addr, uint id);	//addr�Ԓn��uint[4]�ɁACPU�̎��ʏ��id�Ԃ�EAX�EEBX�EECX�EEDX�̏��ԂŊi�[����B
					//��L��̊֐��́AEFLAGS����ID�t���O(�r�b�g21)���ύX�\�ȏꍇ�̂ݎ��s�ł���B
void TSC_Read(uint *addr);		//addr�Ԓn��uint[2]�ɁA�}�V���ŗL���W�X�^(MSR)���ɂ���A�^�C���E�X�^���v�E�J�E���^�̏�ʁE���ʂ��ꂼ��32�r�b�g��ǂݍ��ށB
					//���̊֐��́Acpuid��TSC�r�b�g���L���łȂ���Ύg�p�ł��Ȃ��B
uint Memory_Test_Sub(uint start, uint end);
void INT_3(void);			//�u���[�N�|�C���g��O�𔭐�������B
uint DIV_64_32(uint dividend_low, uint dividend_high, uint divisor);
					//=((dividend_high << 32) | dividend_low) / divisor
uint MOD_64_32(uint dividend_low, uint dividend_high, uint divisor);
					//=((dividend_high << 32) | dividend_low) % divisor
/*nasfunc1.nas ���̊֐��Ɉˑ�����A�Z���u���֐��Q*/
void asm_CPU_ExceptionHandler00(void);
void asm_CPU_ExceptionHandler01(void);
void asm_CPU_ExceptionHandler02(void);
void asm_CPU_ExceptionHandler03(void);
void asm_CPU_ExceptionHandler04(void);
void asm_CPU_ExceptionHandler05(void);
void asm_CPU_ExceptionHandler06(void);
void asm_CPU_ExceptionHandler07(void);
void asm_CPU_ExceptionHandler08(void);
void asm_CPU_ExceptionHandler09(void);
void asm_CPU_ExceptionHandler0a(void);
void asm_CPU_ExceptionHandler0b(void);
void asm_CPU_ExceptionHandler0c(void);
void asm_CPU_ExceptionHandler0d(void);
void asm_CPU_ExceptionHandler0e(void);
void asm_CPU_ExceptionHandler0f(void);
void asm_CPU_ExceptionHandler10(void);
void asm_CPU_ExceptionHandler11(void);
void asm_CPU_ExceptionHandler12(void);
void asm_CPU_ExceptionHandler13(void);
void asm_CPU_ExceptionHandler14(void);
void asm_CPU_ExceptionHandler15(void);
void asm_CPU_ExceptionHandler16(void);
void asm_CPU_ExceptionHandler17(void);
void asm_CPU_ExceptionHandler18(void);
void asm_CPU_ExceptionHandler19(void);
void asm_CPU_ExceptionHandler1a(void);
void asm_CPU_ExceptionHandler1b(void);
void asm_CPU_ExceptionHandler1c(void);
void asm_CPU_ExceptionHandler1d(void);
void asm_CPU_ExceptionHandler1e(void);
void asm_CPU_ExceptionHandler1f(void);
//
void asm_InterruptHandler20(void);
void asm_InterruptHandler21(void);
void asm_InterruptHandler27(void);

/*nasfunc2.nas 16bit�R�[�h*/
void asm_16bit_CallBIOSTask(void);
