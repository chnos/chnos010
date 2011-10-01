
#include "core.h"

//x86エミュレータ
//仮想86モードを利用するのが難しそうなので、16bitコードのみ対応で良いから、
//CPUと同等の動作をするエミュレーターを作る。
//メモリへの全権アクセスが可能であることが前提。

uchar *Emulator_x86_RegNames[] = {
	"AX",
	"CX",
	"DX",
	"BX",
	"SP",
	"BP",
	"SI",
	"DI"
};

uchar *Emulator_x86_SRegNames[] = {
	"ES",
	"CS",
	"SS",
	"DS",
	"FS",
	"GS",
	"xx",
	"xx"
};

uchar *Emulator_x86_ByteRegNames[] = {
	"AL",
	"CL",
	"DL",
	"BL",
	"AH",
	"CH",
	"DH",
	"BH"
};

void Emulator_x86_Initialise(Emulator_x86_Environment *env)
{
	env->EFLAGS.eflags			= 0x00000002;
	env->EIP				= 0x0000fff0;

	env->CR0.cr0				= 0x60000010;
	env->CR2				= 0x00000000;
	env->CR3.cr3				= 0x00000000;
	env->CR4.cr4				= 0x00000000;

	env->SReg[OPCODE_SREG3_ES].selector	= 0x0000;
	env->SReg[OPCODE_SREG3_ES].base		= 0x00000000;
	env->SReg[OPCODE_SREG3_ES].limit	= 0xffff;
	env->SReg[OPCODE_SREG3_ES].type		= AR_PRESENT + AR_TYPE_DATA_RW + AR_TYPE_ACCESSED;
	env->SReg[OPCODE_SREG3_CS]		= env->SReg[OPCODE_SREG3_ES];
	env->SReg[OPCODE_SREG3_SS]		= env->SReg[OPCODE_SREG3_ES];
	env->SReg[OPCODE_SREG3_DS]		= env->SReg[OPCODE_SREG3_ES];
	env->SReg[OPCODE_SREG3_FS]		= env->SReg[OPCODE_SREG3_ES];
	env->SReg[OPCODE_SREG3_GS]		= env->SReg[OPCODE_SREG3_ES];

	env->GReg[OPCODE_REG_EAX]		= 0x00000000;
	env->GReg[OPCODE_REG_ECX]		= 0x00000000;
	env->GReg[OPCODE_REG_EDX]		= 0x00000000;
	env->GReg[OPCODE_REG_EBX]		= 0x00000000;
	env->GReg[OPCODE_REG_ESP]		= 0x00000000;
	env->GReg[OPCODE_REG_EBP]		= 0x00000000;
	env->GReg[OPCODE_REG_ESI]		= 0x00000000;
	env->GReg[OPCODE_REG_EDI]		= 0x00000000;

	env->GDTR.base				= 0x00000000;
	env->GDTR.limit				= 0xffff;

	env->IDTR.base				= 0x00000000;
	env->IDTR.limit				= 0xffff;

	env->now_opcode				= 0x00;
	env->operation_end			= False;
	env->operation_32bit			= False;

	return;
}

uint Emulator_x86_Execute(Emulator_x86_Environment *env)
{

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("Emulator-x86:EIP=0x%08X ", env->EIP);
	#endif

	env->operation_prefix_lock_repeat = OPCODE_PREFIX_NONE;
	env->operation_prefix_segment = OPCODE_SREG3_DS;
	env->operation_prefix_opsize = OPCODE_PREFIX_NONE;
	env->operation_prefix_addrsize = OPCODE_PREFIX_NONE;

	for(; !env->operation_end; ){
		env->now_opcode = Emulator_x86_FetchCode(env, 1);
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("opcode=0x%02X ", env->now_opcode);
		#endif
		if(0x40 <= env->now_opcode && env->now_opcode <= 0x47){	/*INC FullReg*/
			Emulator_x86_Operation_INC_RegOnly(env);

		} else if(0x50 <= env->now_opcode && env->now_opcode <= 0x57){	/*PUSH FullReg*/
			Emulator_x86_Operation_PUSH_RegOnly(env);

		} else if(0xB0 <= env->now_opcode && env->now_opcode <= 0xB7){	/*MOV ByteReg*/
			Emulator_x86_Operation_MOV_To_ByteReg(env);

		} else if(0xB8 <= env->now_opcode && env->now_opcode <= 0xBF){	/*MOV FullSize*/
			Emulator_x86_Operation_MOV_To_Reg_FullSize(env);

		} else if(env->now_opcode == 0x26){	/*ES Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_ES;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:ES)");
			#endif

		} else if(env->now_opcode == 0x2E){	/*CS/BranchNotTaken Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_CS;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:CS)");
			#endif

		} else if(env->now_opcode == 0x36){	/*SS Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_SS;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:SS)");
			#endif
		} else if(env->now_opcode == 0x3C){	/*CMP AL*/
			Emulator_x86_Operation_CMP_AL(env);

		} else if(env->now_opcode == 0x3E){	/*DS/BranchTaken Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_DS;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:DS)");
			#endif

		} else if(env->now_opcode == 0x60){	/*PUSHA/PUSHAD*/
			Emulator_x86_Operation_PUSHA(env);

		} else if(env->now_opcode == 0x61){	/*POPA/POPAD*/
			Emulator_x86_Operation_POPA(env);

		} else if(env->now_opcode == 0x64){	/*FS Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_FS;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:FS)");
			#endif

		} else if(env->now_opcode == 0x65){	/*GS Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_GS;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:GS)");
			#endif

		} else if(env->now_opcode == 0x66){	/*OperandSize Prefix*/
			env->operation_prefix_opsize = OPCODE_PREFIX_OPERAND_SIZE;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:OperandSize)");
			#endif

		} else if(env->now_opcode == 0x67){	/*AddressSize Prefix*/
			env->operation_prefix_addrsize = OPCODE_PREFIX_ADDRESS_SIZE;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:AddressSize)");
			#endif

		} else if(env->now_opcode == 0x74){	/*JE rel8*/
			Emulator_x86_Operation_Jcc_JE_rel8(env);

		} else if(env->now_opcode == 0x8A){	/*MOV Byte*/
			Emulator_x86_Operation_MOV_To_ByteReg_Gb_Eb(env);

		} else if(env->now_opcode == 0x8D){	/*LEA*/
			Emulator_x86_Operation_LEA(env);

		} else if(env->now_opcode == 0x8E){	/*MOV SegReg*/
			Emulator_x86_Operation_MOV_To_SegReg(env);

		} else if(env->now_opcode == 0xC3){	/*near RET*/
			Emulator_x86_Operation_RET_Near(env);

		} else if(env->now_opcode == 0xCD){	/*INT n*/
			Emulator_x86_Operation_INTn(env);

		} else if(env->now_opcode == 0xE8){	/*CALL near*/
			Emulator_x86_Operation_CALL_Near_Relative(env);

		} else if(env->now_opcode == 0xEB){	/*JMP short*/
			Emulator_x86_Operation_JMP_rel8(env);

		} else if(env->now_opcode == 0xF0){	/*LOCK Prefix*/
			env->operation_prefix_lock_repeat = OPCODE_PREFIX_LOCK;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:Lock)");
			#endif

		} else if(env->now_opcode == 0xF2){	/*REPNE/REPNZ Prefix*/
			env->operation_prefix_lock_repeat = OPCODE_PREFIX_REPNE_REPNZ;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:REPNE/REPNZ)");
			#endif

		} else if(env->now_opcode == 0xF3){	/*REP/REPE/REPZ Prefix*/
			env->operation_prefix_lock_repeat = OPCODE_PREFIX_REP_REPE_REPZ;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(Prefix:REP/REPE/REPZ)");
			#endif

		} else{
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("Unknown opcode.\n", env->now_opcode);
			#endif
			return 0xFFFFFFFF;
			//env->operation_end = True;
		}
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("\n");
	#endif

	env->operation_end = False;

	return 0;
}

uint Emulator_x86_Execute_Auto(Emulator_x86_Environment *env)
{
	uint i;

	for(i = 0; ; i++){
		if(Emulator_x86_Execute(env) == 0xffffffff){
			break;
		}
	}
	return i + 1;
}

uint Emulator_x86_FetchCode(Emulator_x86_Environment *env, uint bytes)
{
	uint data;
	uint i;

	data = 0;
	for(i = 0; i < bytes; i++){
		data |= (*((uchar *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_CS, env->EIP)) << (i << 3));
		Emulator_x86_InstructionPointer_Increment(env);
	}

	return data;
}

void Emulator_x86_InstructionPointer_Increment(Emulator_x86_Environment *env)
{
	env->EIP++;
	if(!env->operation_32bit){
		env->EIP &= 0x0000ffff;
	}
	return;
}

uint Emulator_x86_Get_EffectivePhysicalAddress(Emulator_x86_Environment *env, uint sreg, uint offset)
{
	uint addr;

	addr = 0;

	if(sreg > 5){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(Invalid SReg).\n");
		#endif
		return 0;
	}

	if(env->operation_32bit){	/*32-bit addressing*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("32-bit addressing is not implemented.\n");
		#endif
	} else{	/*16-bit addressing*/
		addr = (((env->SReg[sreg].selector << 4) + offset) & 0x000fffff);
	}
	return addr;
}

void Emulator_x86_MoveToGReg(Emulator_x86_Environment *env, uint reg, uint data, uint fullsize)
{
	if(reg > 7){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(Move to Invalid GReg).\n");
		#endif
		return;
	}

	if(fullsize){
		if(env->operation_32bit){	/*DoubleWord*/
			env->GReg[reg] = data;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveToE%s:0x%X)", Emulator_x86_RegNames[reg], data);
			#endif
		} else{	/*Word*/
			env->GReg[reg] &= 0xffff0000;
			env->GReg[reg] |= 0x0000ffff & data;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveTo%s:0x%X)", Emulator_x86_RegNames[reg], data);
			#endif
		}
	} else{	/*Byte*/
		if(reg < 4){	/*Low*/
			env->GReg[reg] &= 0xffffff00;
			env->GReg[reg] |= 0x000000ff & data;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveTo%s:0x%X)", Emulator_x86_ByteRegNames[reg], data);
			#endif
		} else{	/*High*/
			reg -= 4;
			env->GReg[reg] &= 0xffff00ff;
			env->GReg[reg] |= 0x0000ff00 & (data << 8);
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveTo%s:0x%X)", Emulator_x86_ByteRegNames[reg + 4], data);
			#endif
		}
	}
	return;
}

uint Emulator_x86_MoveFromGReg(Emulator_x86_Environment *env, uint reg, uint fullsize)
{
	uint data;

	if(reg > 7){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(Move from Invalid GReg).\n");
		#endif
		return 0;
	}

	if(fullsize){
		if(env->operation_32bit){	/*DoubleWord*/
			data = env->GReg[reg];
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveFromE%s:0x%X)", Emulator_x86_RegNames[reg], data);
			#endif
		} else{	/*Word*/
			data = env->GReg[reg];
			data &= 0x0000ffff;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveFrom%s:0x%X)", Emulator_x86_RegNames[reg], data);
			#endif
		}
	} else{	/*Byte*/
		if(reg < 4){	/*Low*/
			data = env->GReg[reg];
			data &= 0x000000ff;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveFrom%s:0x%X)", Emulator_x86_ByteRegNames[reg], data);
			#endif
		} else{	/*High*/
			reg -= 4;
			data = env->GReg[reg];
			data &= 0x0000ff00;
			data = data >> 8;
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("(MoveFrom%s:0x%X)", Emulator_x86_ByteRegNames[reg + 4], data);
			#endif
		}
	}
	return data;
}

void Emulator_x86_MoveToSReg(Emulator_x86_Environment *env, uint sreg, ushort selector)
{
	if(sreg > 7){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(Move to Invalid SReg).\n");
		#endif
		return;
	}

	if(env->operation_32bit){	/*32-bit Segmentation*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("32-bit segmentation is not implemented.\n");
		#endif
	} else{	/*16-bit Segmentation*/
		env->SReg[sreg].selector = selector;
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("(MoveTo%s:0x%X)", Emulator_x86_SRegNames[sreg], selector);
		#endif
	}
	return;
}

void Emulator_x86_Push_Data_To_Stack(Emulator_x86_Environment *env, uint data, uint size_dword)
{
	void *addr;

	if(env->GReg[OPCODE_REG_ESP] < 4){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(No more Stack).\n");
		#endif
		return;
	}
	if(size_dword){	/*DoubleWord*/
		Emulator_x86_MoveToGReg(env, OPCODE_REG_ESP, env->GReg[OPCODE_REG_ESP] - 4, True);
		addr = (void *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]);
		*((uint *)addr) = data;
	} else{	/*Word*/
		Emulator_x86_MoveToGReg(env, OPCODE_REG_ESP, env->GReg[OPCODE_REG_ESP] - 2, True);
		addr = (void *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]);
		*((ushort *)addr) = data;
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("(PushTo[0x%08X]:0x%X)", Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]), data);
	#endif

	return;
}

uint Emulator_x86_Pop_Data_From_Stack(Emulator_x86_Environment *env, uint size_dword)
{
	uint data;

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("(PopFrom[0x%08X]:", Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]));
	#endif

	if(size_dword){	/*DoubleWord*/
		data = *((uint *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]));
		Emulator_x86_MoveToGReg(env, OPCODE_REG_ESP, env->GReg[OPCODE_REG_ESP] + 4, True);
	} else{	/*Word*/
		data = *((ushort *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]));
		Emulator_x86_MoveToGReg(env, OPCODE_REG_ESP, env->GReg[OPCODE_REG_ESP] + 2, True);
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("0x%X)", data);
	#endif

	return data;
}

void Emulator_x86_Operation_MOV_To_Reg_FullSize(Emulator_x86_Environment *env)
{
	uint imm;

	imm = Emulator_x86_FetchCode(env, 2);

	Emulator_x86_MoveToGReg(env, env->now_opcode - 0xB8, imm, True);

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("MOV %s, 0x%X\n", Emulator_x86_RegNames[env->now_opcode - 0xB8], imm);
	#endif

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_To_SegReg(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);

	if(modrm.bit.Mod == OPCODE_MOD_INDEXONLY){
	} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_BYTE){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("Move to SReg From Index+Disp_Byte is not implemented.\n");
		#endif
	} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_FULL){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("Move to SReg From Index+Disp_Full is not implemented.\n");
		#endif
	} else if(modrm.bit.Mod == OPCODE_MOD_REGISTER){
		Emulator_x86_MoveToSReg(env, modrm.bit.Reg, Emulator_x86_MoveFromGReg(env, modrm.bit.RM, True));
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("MOV %s, %s\n", Emulator_x86_SRegNames[modrm.bit.Reg], Emulator_x86_RegNames[modrm.bit.RM]);
		#endif
	}
	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_To_ByteReg(Emulator_x86_Environment *env)
{
	uchar imm8;

	imm8 = Emulator_x86_FetchCode(env, 1);

	Emulator_x86_MoveToGReg(env, env->now_opcode - 0xB0, imm8, False);

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("MOV %s, 0x%X\n", Emulator_x86_ByteRegNames[env->now_opcode - 0xB0], imm8);
	#endif

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_INTn(Emulator_x86_Environment *env)
{
//Not implemented
	uchar intn;

	intn = Emulator_x86_FetchCode(env, 1);

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("INT 0x%X\n", intn);
	#endif

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_LEA(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint disp;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);

	if(modrm.bit.Mod == OPCODE_MOD_INDEXONLY){
		if(modrm.bit.RM == OPCODE_RM16_MOD00_ADDR_DISP16){
			disp = Emulator_x86_FetchCode(env, 2);
			Emulator_x86_MoveToGReg(env, modrm.bit.Reg, disp, True);
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("LEA %s, 0x%X\n", Emulator_x86_RegNames[modrm.bit.Reg], disp);
			#endif
		} else{
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("LEA INDEXONLY is not implemented.\n");
			#endif
		}
	} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_BYTE){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("LEA DISP_BYTE is not implemented.\n");
		#endif
	} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_FULL){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("LEA DISP_FULLis not implemented.\n");
		#endif
	} else if(modrm.bit.Mod == OPCODE_MOD_REGISTER){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("LEA Invalid.\n");
		#endif
	}
	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_CALL_Near_Relative(Emulator_x86_Environment *env)
{
	short relative_addr;
	uchar sign;

	sign = False;

	relative_addr = Emulator_x86_FetchCode(env, 2);

	if(relative_addr < 0){
		sign = True;
		relative_addr = -relative_addr;
	}

	Emulator_x86_Push_Data_To_Stack(env, env->EIP, False);

	if(sign){
		env->EIP -= (uint)relative_addr;
	} else{
		env->EIP += (uint)relative_addr;
	}

	if(!env->operation_32bit){
		env->EIP &= 0x0000ffff;
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("near CALL 0x%X\n", env->EIP);
	#endif

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_PUSHA(Emulator_x86_Environment *env)
{
	uint Temp;
	uint i;

	Temp = env->GReg[OPCODE_REG_ESP];

	if(env->operation_32bit){	/*PUSHAD*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("PUSHAD\n");
		#endif
		for(i = 0; i < 4; i++){
			Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, i, True), True);
		}
		Emulator_x86_Push_Data_To_Stack(env, Temp, False);
		for(i = 5; i < 8; i++){
			Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, i, True), True);
		}
	} else{	/*PUSHA*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("PUSHA\n");
		#endif
		for(i = 0; i < 4; i++){
			Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, i, True), False);
		}
		Emulator_x86_Push_Data_To_Stack(env, Temp, False);
		for(i = 5; i < 8; i++){
			Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, i, True), False);
		}
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_POPA(Emulator_x86_Environment *env)
{
	int i;

	if(env->operation_32bit){	/*PUSHAD*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("POPAD\n");
		#endif
		for(i = 7; i > 4; i--){
			Emulator_x86_MoveToGReg(env, i, Emulator_x86_Pop_Data_From_Stack(env, True), True);
		}
		Emulator_x86_Pop_Data_From_Stack(env, True);
		for(i = 3; i > -1; i--){
			Emulator_x86_MoveToGReg(env, i, Emulator_x86_Pop_Data_From_Stack(env, True), True);
		}
	} else{	/*PUSHA*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("POPA\n");
		#endif
		for(i = 7; i > 4; i--){
			Emulator_x86_MoveToGReg(env, i, Emulator_x86_Pop_Data_From_Stack(env, False), True);
		}
		Emulator_x86_Pop_Data_From_Stack(env, False);
		for(i = 3; i > -1; i--){
			Emulator_x86_MoveToGReg(env, i, Emulator_x86_Pop_Data_From_Stack(env, False), True);
		}
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_To_ByteReg_Gb_Eb(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);
	addr = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg only*/
			Emulator_x86_MoveToGReg(env, modrm.bit.Reg, Emulator_x86_MoveFromGReg(env, modrm.bit.RM, False), False);
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("MOV %s, %s\n", Emulator_x86_ByteRegNames[modrm.bit.Reg], Emulator_x86_ByteRegNames[modrm.bit.RM]);
			#endif
		} else{	/*From MemoryAddress*/
			if(modrm.bit.RM == OPCODE_RM16_ADDR_BX_SI){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBX, True);
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_ESI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BX_SI){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBX, True);
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EDI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BX_DI){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBP, True);
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_ESI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BP_SI){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBP, True);
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EDI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_SI){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_ESI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_DI){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EDI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_MOD00_ADDR_DISP16){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBP, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BX){
				addr += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBX, True);
			}
			if(modrm.bit.Mod == OPCODE_MOD_INDEXONLY){
				if(modrm.bit.RM == OPCODE_RM16_MOD00_ADDR_DISP16){
					addr = Emulator_x86_FetchCode(env, 2);
				}
			} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_BYTE){
				addr += Emulator_x86_FetchCode(env, 1);
			} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_FULL){
				addr += Emulator_x86_FetchCode(env, 2);
			}
			addr &= 0x0000ffff;
			addr = Emulator_x86_Get_EffectivePhysicalAddress(env, env->operation_prefix_segment, addr);
			Emulator_x86_MoveToGReg(env, modrm.bit.Reg, *((uchar *)addr), False);
			#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
				debug("MOV %s, [0x%X]\n", Emulator_x86_ByteRegNames[modrm.bit.Reg], addr);
			#endif
		}
	} else{	/*32-bit Addressing*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("32-bit addressing is not implemented.\n");
		#endif
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_CMP_AL(Emulator_x86_Environment *env)
{
	CPU_EFlags eflags;
	uint imm;

	imm = Emulator_x86_FetchCode(env, 1);

	eflags.eflags = asm_Emulator_x86_Get_EFlags_Compare(Emulator_x86_MoveFromGReg(env, OPCODE_REG_BYTE_AL, False), imm);
	env->EFLAGS.bit.CF = eflags.bit.CF;
	env->EFLAGS.bit.PF = eflags.bit.PF;
	env->EFLAGS.bit.AF = eflags.bit.AF;
	env->EFLAGS.bit.ZF = eflags.bit.ZF;
	env->EFLAGS.bit.SF = eflags.bit.SF;
	env->EFLAGS.bit.OF = eflags.bit.OF;
	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("CMP AL, 0x%X CF:%d PF:%d AF:%d ZF:%d SF:%d OF:%d\n", imm, env->EFLAGS.bit.CF, env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);
	#endif

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_Jcc_JE_rel8(Emulator_x86_Environment *env)
{
	char relative_addr;
	uchar sign;
	uint new_eip;

	sign = False;
	relative_addr = Emulator_x86_FetchCode(env, 1);
	new_eip = env->EIP;

	if(relative_addr < 0){
		sign = True;
		relative_addr = -relative_addr;
	}
	if(sign){
		new_eip -= (uint)relative_addr;
	} else{
		new_eip += (uint)relative_addr;
	}

	if(!env->operation_32bit){
		new_eip &= 0x0000ffff;
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("JE 0x%X ", new_eip);
	#endif

	if(env->EFLAGS.bit.ZF){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("Taken.\n");
		#endif
		env->EIP = new_eip;
	} else{
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("NotTaken.\n");
		#endif
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_INC_RegOnly(Emulator_x86_Environment *env)
{
	uint operand;
	CPU_EFlags eflags;

	if((env->now_opcode - 0x40) > 7){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(Invalid GReg From OpCode).\n");
		#endif
	}

	operand = Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x40, True);
	Emulator_x86_MoveToGReg(env, env->now_opcode - 0x40, operand + 1, True);

	eflags.eflags = asm_Emulator_x86_Get_EFlags_Increment(operand);
	env->EFLAGS.bit.PF = eflags.bit.PF;
	env->EFLAGS.bit.AF = eflags.bit.AF;
	env->EFLAGS.bit.ZF = eflags.bit.ZF;
	env->EFLAGS.bit.SF = eflags.bit.SF;
	env->EFLAGS.bit.OF = eflags.bit.OF;
	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("INC %s PF:%d AF:%d ZF:%d SF:%d OF:%d\n", Emulator_x86_RegNames[env->now_opcode - 0x40], env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);
	#endif

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_JMP_rel8(Emulator_x86_Environment *env)
{
	char relative_addr;
	uchar sign;
	uint new_eip;

	sign = False;
	relative_addr = Emulator_x86_FetchCode(env, 1);
	new_eip = env->EIP;

	if(relative_addr < 0){
		sign = True;
		relative_addr = -relative_addr;
	}
	if(sign){
		new_eip -= (uint)relative_addr;
	} else{
		new_eip += (uint)relative_addr;
	}

	if(!env->operation_32bit){
		new_eip &= 0x0000ffff;
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("JMP 0x%X\n", new_eip);
	#endif

	env->EIP = new_eip;

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_RET_Near(Emulator_x86_Environment *env)
{
	uint new_eip;

	new_eip = Emulator_x86_Pop_Data_From_Stack(env, False);

	if(!env->operation_32bit){
		new_eip &= 0x0000ffff;
	}

	#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
		debug("near RET to 0x%X\n", new_eip);
	#endif

	env->EIP = new_eip;

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_PUSH_RegOnly(Emulator_x86_Environment *env)
{
	if((env->now_opcode - 0x50) > 7){
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("\nEmulator-x86:Internal Error(Invalid GReg From OpCode).\n");
		#endif
	}
	if(!env->operation_32bit){	/*16-bit*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("PUSH %s\n", Emulator_x86_RegNames[env->now_opcode - 0x50]);
		#endif
		Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x50, True), False);
	} else{	/*32-bit*/
		#ifdef CHNOSPROJECT_DEBUG_EMULATOR_X86
			debug("PUSH E%s\n", Emulator_x86_RegNames[env->now_opcode - 0x50]);
		#endif
		Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x50, True), True);
	}

	env->operation_end = True;

	return;
}
