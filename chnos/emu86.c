
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
	Emulator_x86_Put_EmulationInformation(env, "Emulator-x86:EIP=0x%08X ", env->EIP);

	env->operation_prefix_lock_repeat = OPCODE_PREFIX_NONE;
	env->operation_prefix_segment = OPCODE_SREG3_DS;
	env->operation_prefix_opsize = OPCODE_PREFIX_NONE;
	env->operation_prefix_addrsize = OPCODE_PREFIX_NONE;

	for(; !env->operation_end; ){
		env->now_opcode = Emulator_x86_FetchCode(env, 1);
		Emulator_x86_Put_EmulationInformation(env, "opcode=0x%02X ", env->now_opcode);

		if(0x40 <= env->now_opcode && env->now_opcode <= 0x47){	/*INC FullReg*/
			Emulator_x86_Operation_INC_RegOnly(env);

		} else if(0x48 <= env->now_opcode && env->now_opcode <= 0x4F){	/*DEC FullReg*/
			Emulator_x86_Operation_DEC_RegOnly(env);

		} else if(0x50 <= env->now_opcode && env->now_opcode <= 0x57){	/*PUSH FullReg*/
			Emulator_x86_Operation_PUSH_RegOnly(env);

		} else if(0x58 <= env->now_opcode && env->now_opcode <= 0x5F){	/*POP FullReg*/
			Emulator_x86_Operation_POP_RegOnly(env);

		} else if(0xB0 <= env->now_opcode && env->now_opcode <= 0xB7){	/*MOV ByteReg*/
			Emulator_x86_Operation_MOV_To_ByteReg(env);

		} else if(0xB8 <= env->now_opcode && env->now_opcode <= 0xBF){	/*MOV FullSize*/
			Emulator_x86_Operation_MOV_To_Reg_FullSize(env);

		} else if(env->now_opcode == 0x1B){	/*SBB Gv, Ev*/
			Emulator_x86_Operation_SBB_Gv_Ev(env);

		} else if(env->now_opcode == 0x1E){	/*PUSH DS*/
			Emulator_x86_Push_SReg_To_Stack(env, OPCODE_SREG3_DS);
			env->operation_end = True;

		} else if(env->now_opcode == 0x1F){	/*POP DS*/
			Emulator_x86_Pop_SReg_From_Stack(env, OPCODE_SREG3_DS);
			env->operation_end = True;

		} else if(env->now_opcode == 0x26){	/*ES Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_ES;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:ES)");

		} else if(env->now_opcode == 0x2E){	/*CS/BranchNotTaken Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_CS;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:CS)");

		} else if(env->now_opcode == 0x31){	/*XOR Eb, Gb*/
			Emulator_x86_Operation_XOR_Eb_Gb(env);

		} else if(env->now_opcode == 0x36){	/*SS Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_SS;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:SS)");

		} else if(env->now_opcode == 0x3B){	/*CMP Gv, Ev*/
			Emulator_x86_Operation_CMP_Gv_Ev(env);

		} else if(env->now_opcode == 0x3C){	/*CMP AL*/
			Emulator_x86_Operation_CMP_AL(env);

		} else if(env->now_opcode == 0x3E){	/*DS/BranchTaken Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_DS;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:DS)");

		} else if(env->now_opcode == 0x60){	/*PUSHA/PUSHAD*/
			Emulator_x86_Operation_PUSHA(env);

		} else if(env->now_opcode == 0x61){	/*POPA/POPAD*/
			Emulator_x86_Operation_POPA(env);

		} else if(env->now_opcode == 0x64){	/*FS Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_FS;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:FS)");

		} else if(env->now_opcode == 0x65){	/*GS Prefix*/
			env->operation_prefix_segment = OPCODE_SREG3_GS;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:GS)");

		} else if(env->now_opcode == 0x66){	/*OperandSize Prefix*/
			env->operation_prefix_opsize = OPCODE_PREFIX_OPERAND_SIZE;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:OperandSize)");

		} else if(env->now_opcode == 0x67){	/*AddressSize Prefix*/
			env->operation_prefix_addrsize = OPCODE_PREFIX_ADDRESS_SIZE;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:AddressSize)");

		} else if(env->now_opcode == 0x6A){	/*AddressSize Prefix*/
			Emulator_x86_Operation_PUSH_Ib(env);

		} else if(env->now_opcode == 0x74){	/*JE rel8*/
			Emulator_x86_Operation_Jcc_JE_rel8(env);

		} else if(env->now_opcode == 0x75){	/*JE rel8*/
			Emulator_x86_Operation_Jcc_JNE_rel8(env);

		} else if(env->now_opcode == 0x89){	/*MOV Ev, Gv*/
			Emulator_x86_Operation_MOV_Ev_Gv(env);

		} else if(env->now_opcode == 0x8A){	/*MOV Byte*/
			Emulator_x86_Operation_MOV_To_ByteReg_Gb_Eb(env);

		} else if(env->now_opcode == 0x8D){	/*LEA*/
			Emulator_x86_Operation_LEA(env);

		} else if(env->now_opcode == 0x8E){	/*MOV SegReg*/
			Emulator_x86_Operation_MOV_To_SegReg(env);

		} else if(env->now_opcode == 0x8F){	/*POP Ev*/
			Emulator_x86_Operation_POP_Ev(env);

		} else if(env->now_opcode == 0xA1){	/*MOV eAX, Ov*/
			Emulator_x86_Operation_MOV_eAX_Ov(env);

		} else if(env->now_opcode == 0xA3){	/*MOV Ov, eAX*/
			Emulator_x86_Operation_MOV_Ov_eAX(env);

		} else if(env->now_opcode == 0xC3){	/*near RET*/
			Emulator_x86_Operation_RET_Near(env);

		} else if(env->now_opcode == 0xCD){	/*INT n*/
			Emulator_x86_Operation_INTn(env);

		} else if(env->now_opcode == 0xE2){	/*LOOP Jv*/
			Emulator_x86_Operation_LOOP_Jv(env);

		} else if(env->now_opcode == 0xE6){	/*OUT AL*/
			Emulator_x86_Operation_OUT_AL(env);

		} else if(env->now_opcode == 0xE8){	/*CALL near*/
			Emulator_x86_Operation_CALL_Near_Relative(env);

		} else if(env->now_opcode == 0xEB){	/*JMP short*/
			Emulator_x86_Operation_JMP_rel8(env);

		} else if(env->now_opcode == 0xF0){	/*LOCK Prefix*/
			env->operation_prefix_lock_repeat = OPCODE_PREFIX_LOCK;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:LOCK)");

		} else if(env->now_opcode == 0xF2){	/*REPNE/REPNZ Prefix*/
			env->operation_prefix_lock_repeat = OPCODE_PREFIX_REPNE_REPNZ;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:REPNE/REPNZ)");

		} else if(env->now_opcode == 0xF3){	/*REP/REPE/REPZ Prefix*/
			env->operation_prefix_lock_repeat = OPCODE_PREFIX_REP_REPE_REPZ;
			Emulator_x86_Put_EmulationInformation(env, "(Prefix:REP/REPE/REPZ)");

		} else if(env->now_opcode == 0xFB){	/*STI*/
			Emulator_x86_Operation_STI(env);

		} else if(env->now_opcode == 0xFC){	/*CLD*/
			Emulator_x86_Operation_CLD(env);

		} else{
			Emulator_x86_Put_EmulationInformation(env, "Unknown OperationCode.\n");
			return 0xFFFFFFFF;
		}
	}

	Emulator_x86_Put_EmulationInformation(env, "\n");

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

int Emulator_x86_Put_EmulationInformation(Emulator_x86_Environment *env, const uchar format[], ...)
{
	int i;
	uchar s[256];

	i = vsnprintf(s, sizeof(s), format, (uint *)(&format + 1));
	SerialPort_Send(s);

	return i;
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
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid SReg).");
		return 0;
	}

	if(env->operation_32bit){	/*32-bit addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit mode is not implemented.");
	} else{	/*16-bit addressing*/
		addr = (env->SReg[sreg].selector << 4) + offset;
	}
	return addr;
}

uint Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(Emulator_x86_Environment *env, uint mrm)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint offset;
	uint address;

	modrm.modrm = mrm;
	offset = 0;
	address = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg only*/
			Emulator_x86_Put_EmulationInformation(env, "Not Address, From Register.");
			return 0;
		} else{	/*From MemoryAddress*/
			if(modrm.bit.RM == OPCODE_RM16_ADDR_BX_SI){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBX, True);
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_ESI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BX_SI){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBX, True);
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EDI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BX_DI){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBP, True);
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_ESI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BP_SI){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBP, True);
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EDI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_SI){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_ESI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_DI){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EDI, True);
			} else if(modrm.bit.RM == OPCODE_RM16_MOD00_ADDR_DISP16){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBP, True);
			} else if(modrm.bit.RM == OPCODE_RM16_ADDR_BX){
				offset += Emulator_x86_MoveFromGReg(env, OPCODE_REG_EBX, True);
			}
			if(modrm.bit.Mod == OPCODE_MOD_INDEXONLY){
				if(modrm.bit.RM == OPCODE_RM16_MOD00_ADDR_DISP16){
					offset = Emulator_x86_FetchCode(env, 2);
				}
			} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_BYTE){
				offset += Emulator_x86_FetchCode(env, 1);
			} else if(modrm.bit.Mod == OPCODE_MOD_INDEX_AND_DISP_FULL){
				offset += Emulator_x86_FetchCode(env, 2);
			}
			offset &= 0x0000ffff;
			address = Emulator_x86_Get_EffectivePhysicalAddress(env, env->operation_prefix_segment, offset);
			Emulator_x86_Put_EmulationInformation(env, "(Address Generated %s:0x%X = [0x%X])", Emulator_x86_SRegNames[env->operation_prefix_segment], offset, address);
		}
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit mode is not implemented.");
	}

	return address;
}

void Emulator_x86_MoveToGReg(Emulator_x86_Environment *env, uint reg, uint data, uint fullsize)
{
	if(reg > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid GReg).");
		return;
	}

	if(fullsize){
		if(env->operation_32bit){	/*DoubleWord*/
			env->GReg[reg] = data;
			Emulator_x86_Put_EmulationInformation(env, "(MoveToE%s:0x%X)", Emulator_x86_RegNames[reg], data);
		} else{	/*Word*/
			env->GReg[reg] &= 0xffff0000;
			env->GReg[reg] |= 0x0000ffff & data;
			Emulator_x86_Put_EmulationInformation(env, "(MoveTo%s:0x%X)", Emulator_x86_RegNames[reg], data);
		}
	} else{	/*Byte*/
		if(reg < 4){	/*Low*/
			env->GReg[reg] &= 0xffffff00;
			env->GReg[reg] |= 0x000000ff & data;
			Emulator_x86_Put_EmulationInformation(env, "(MoveTo%s:0x%X)", Emulator_x86_ByteRegNames[reg], data);
		} else{	/*High*/
			reg -= 4;
			env->GReg[reg] &= 0xffff00ff;
			env->GReg[reg] |= 0x0000ff00 & (data << 8);
			Emulator_x86_Put_EmulationInformation(env, "(MoveTo%s:0x%X)", Emulator_x86_ByteRegNames[reg + 4], data);
		}
	}
	return;
}

uint Emulator_x86_MoveFromGReg(Emulator_x86_Environment *env, uint reg, uint fullsize)
{
	uint data;

	if(reg > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid GReg).");
		return 0;
	}

	if(fullsize){
		if(env->operation_32bit){	/*DoubleWord*/
			data = env->GReg[reg];
			Emulator_x86_Put_EmulationInformation(env, "(MoveFromE%s:0x%X)", Emulator_x86_RegNames[reg], data);
		} else{	/*Word*/
			data = env->GReg[reg];
			data &= 0x0000ffff;
			Emulator_x86_Put_EmulationInformation(env, "(MoveFrom%s:0x%X)", Emulator_x86_RegNames[reg], data);
		}
	} else{	/*Byte*/
		if(reg < 4){	/*Low*/
			data = env->GReg[reg];
			data &= 0x000000ff;
			Emulator_x86_Put_EmulationInformation(env, "(MoveFrom%s:0x%X)", Emulator_x86_ByteRegNames[reg], data);
		} else{	/*High*/
			reg -= 4;
			data = env->GReg[reg];
			data &= 0x0000ff00;
			data = data >> 8;
			Emulator_x86_Put_EmulationInformation(env, "(MoveFrom%s:0x%X)", Emulator_x86_ByteRegNames[reg + 4], data);
		}
	}
	return data;
}

void Emulator_x86_MoveToSReg(Emulator_x86_Environment *env, uint sreg, ushort selector)
{
	if(sreg > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid SReg).");
		return;
	}

	if(env->operation_32bit){	/*32-bit Segmentation*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit mode is not implemented.");
	} else{	/*16-bit Segmentation*/
		env->SReg[sreg].selector = selector;
		Emulator_x86_Put_EmulationInformation(env, "(MoveTo%s:0x%X)", Emulator_x86_SRegNames[sreg], selector);
	}
	return;
}

void Emulator_x86_Push_Data_To_Stack(Emulator_x86_Environment *env, uint data, uint size_dword)
{
	void *addr;

	if(env->GReg[OPCODE_REG_ESP] < 4){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(No More Stack).");
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

	Emulator_x86_Put_EmulationInformation(env, "(PushTo[0x%08X]:0x%X)", Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]), data);

	return;
}

uint Emulator_x86_Pop_Data_From_Stack(Emulator_x86_Environment *env, uint size_dword)
{
	uint data;

	Emulator_x86_Put_EmulationInformation(env, "(PopFrom[0x%08X]:", Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]));

	if(size_dword){	/*DoubleWord*/
		data = *((uint *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]));
		Emulator_x86_MoveToGReg(env, OPCODE_REG_ESP, env->GReg[OPCODE_REG_ESP] + 4, True);
	} else{	/*Word*/
		data = *((ushort *)Emulator_x86_Get_EffectivePhysicalAddress(env, OPCODE_SREG3_SS, env->GReg[OPCODE_REG_ESP]));
		Emulator_x86_MoveToGReg(env, OPCODE_REG_ESP, env->GReg[OPCODE_REG_ESP] + 2, True);
	}

	Emulator_x86_Put_EmulationInformation(env, "0x%X)", data);

	return data;
}

void Emulator_x86_Push_eIP_To_Stack(Emulator_x86_Environment *env)
{
	if(!env->operation_32bit){	/*16-bit*/
		env->EIP &= 0x0000ffff;
		Emulator_x86_Push_Data_To_Stack(env, env->EIP, False);
		Emulator_x86_Put_EmulationInformation(env, "(Push IP[0x%X])", env->EIP);
	} else{	/*32-bit*/
		Emulator_x86_Push_Data_To_Stack(env, env->EIP, True);
		Emulator_x86_Put_EmulationInformation(env, "(Push EIP[0x%X])", env->EIP);
	}

	return;
}

void Emulator_x86_Pop_eIP_From_Stack(Emulator_x86_Environment *env)
{
	if(!env->operation_32bit){	/*16-bit*/
		env->EIP = Emulator_x86_Pop_Data_From_Stack(env, False);
		Emulator_x86_Put_EmulationInformation(env, "(Pop IP[0x%X])", env->EIP);
	} else{	/*32-bit*/
		env->EIP = Emulator_x86_Pop_Data_From_Stack(env, True);
		Emulator_x86_Put_EmulationInformation(env, "(Pop EIP[0x%X])", env->EIP);
	}

	return;
}

void Emulator_x86_Push_eFLAGS_To_Stack(Emulator_x86_Environment *env)
{
	if(!env->operation_32bit){	/*16-bit*/
		env->EFLAGS.eflags &= 0x0000ffff;
		Emulator_x86_Push_Data_To_Stack(env, env->EFLAGS.eflags, False);
		Emulator_x86_Put_EmulationInformation(env, "(Push FLAGS(0x%X))", env->EFLAGS.eflags);
	} else{	/*32-bit*/
		Emulator_x86_Push_Data_To_Stack(env, env->EIP, True);
		Emulator_x86_Put_EmulationInformation(env, "(Push EFLAGS(0x%X))", env->EFLAGS.eflags);
	}

	return;
}

void Emulator_x86_Pop_eFLAGS_From_Stack(Emulator_x86_Environment *env)
{
	if(!env->operation_32bit){	/*16-bit*/
		env->EFLAGS.eflags = Emulator_x86_Pop_Data_From_Stack(env, False);
		Emulator_x86_Put_EmulationInformation(env, "(Pop FLAGS(0x%X))", env->EFLAGS.eflags);
	} else{	/*32-bit*/
		env->EFLAGS.eflags = Emulator_x86_Pop_Data_From_Stack(env, True);
		Emulator_x86_Put_EmulationInformation(env, "(Pop EFLAGS(0x%X))", env->EFLAGS.eflags);
	}

	return;
}

void Emulator_x86_Push_SReg_To_Stack(Emulator_x86_Environment *env, uint sreg)
{
	//16bit/32bit

	if(sreg > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid SReg).");
		return;
	}

	if(!env->operation_32bit){	/*16-bit*/
		Emulator_x86_Push_Data_To_Stack(env, env->SReg[sreg].selector, False);
	} else{	/*32-bit*/
		Emulator_x86_Push_Data_To_Stack(env, env->SReg[sreg].selector, True);
	}

	Emulator_x86_Put_EmulationInformation(env, "(Push %s(0x%X))", Emulator_x86_SRegNames[sreg], env->SReg[sreg].selector);

	return;
}

void Emulator_x86_Pop_SReg_From_Stack(Emulator_x86_Environment *env, uint sreg)
{
	//16bit/32bit

	if(sreg > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid SReg).");
		return;
	}

	if(!env->operation_32bit){	/*16-bit*/
		Emulator_x86_MoveToSReg(env, sreg, Emulator_x86_Pop_Data_From_Stack(env, False));
	} else{	/*32-bit*/
		Emulator_x86_MoveToSReg(env, sreg, Emulator_x86_Pop_Data_From_Stack(env, True));
	}

	Emulator_x86_Put_EmulationInformation(env, "(Pop %s(0x%X))", Emulator_x86_SRegNames[sreg], env->SReg[sreg].selector);

	return;
}

void Emulator_x86_Operation_MOV_To_Reg_FullSize(Emulator_x86_Environment *env)
{
	uint imm;

	imm = Emulator_x86_FetchCode(env, 2);

	Emulator_x86_MoveToGReg(env, env->now_opcode - 0xB8, imm, True);

	Emulator_x86_Put_EmulationInformation(env, "MOV %s, 0x%X", Emulator_x86_RegNames[env->now_opcode - 0xB8], imm);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_To_SegReg(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);

	if(modrm.bit.Mod == OPCODE_MOD_REGISTER){
		Emulator_x86_MoveToSReg(env, modrm.bit.Reg, Emulator_x86_MoveFromGReg(env, modrm.bit.RM, True));
		Emulator_x86_Put_EmulationInformation(env, "MOV %s, %s", Emulator_x86_SRegNames[modrm.bit.Reg], Emulator_x86_RegNames[modrm.bit.RM]);
	} else{
		addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
		Emulator_x86_MoveToSReg(env, modrm.bit.Reg, *((ushort *)addr));
		Emulator_x86_Put_EmulationInformation(env, "MOV %s, [0x%X]", Emulator_x86_SRegNames[modrm.bit.Reg], addr);
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_To_ByteReg(Emulator_x86_Environment *env)
{
	uchar imm8;

	imm8 = Emulator_x86_FetchCode(env, 1);

	Emulator_x86_MoveToGReg(env, env->now_opcode - 0xB0, imm8, False);

	Emulator_x86_Put_EmulationInformation(env, "MOV %s, 0x%X", Emulator_x86_ByteRegNames[env->now_opcode - 0xB0], imm8);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_INTn(Emulator_x86_Environment *env)
{
	//16-bit only

	uchar intn;
	Emulator_x86_FarPointer *vector;

	intn = Emulator_x86_FetchCode(env, 1);

	Emulator_x86_Put_EmulationInformation(env, "INT 0x%X", intn);

	vector = (Emulator_x86_FarPointer *)(intn << 2);

	Emulator_x86_Put_EmulationInformation(env, "[0x%X:0x%X] = [0x%X]", vector->selector, vector->offset, (vector->selector << 4) + vector->offset);

	Emulator_x86_Push_eFLAGS_To_Stack(env);
	env->EFLAGS.bit.IF = False;
	env->EFLAGS.bit.TF = False;
	env->EFLAGS.bit.AC = False;
	Emulator_x86_Push_SReg_To_Stack(env, OPCODE_SREG3_CS);
	Emulator_x86_Push_eIP_To_Stack(env);

	Emulator_x86_MoveToSReg(env, OPCODE_SREG3_CS, vector->selector);
	env->EIP = vector->offset;

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_LEA(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint disp;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);

	if(!env->operation_32bit){
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){
			Emulator_x86_Put_EmulationInformation(env, "LEA Invalid.");
		} else{
			disp = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			disp -= (env->SReg[env->operation_prefix_segment].selector << 4);
			Emulator_x86_MoveToGReg(env, modrm.bit.Reg, disp, True);
			Emulator_x86_Put_EmulationInformation(env, "LEA %s, 0x%X", Emulator_x86_RegNames[modrm.bit.Reg], disp);
		}
	} else{
		Emulator_x86_Put_EmulationInformation(env, "32-bit mode is not implemented.");
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

	Emulator_x86_Push_eIP_To_Stack(env);

	if(sign){
		env->EIP -= (uint)relative_addr;
	} else{
		env->EIP += (uint)relative_addr;
	}

	if(!env->operation_32bit){
		env->EIP &= 0x0000ffff;
	}

	Emulator_x86_Put_EmulationInformation(env, "near CALL 0x%X", env->EIP);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_PUSHA(Emulator_x86_Environment *env)
{
	uint Temp;
	uint i;

	Temp = env->GReg[OPCODE_REG_ESP];

	if(env->operation_32bit){	/*PUSHAD*/
		Emulator_x86_Put_EmulationInformation(env, "PUSHAD");
		for(i = 0; i < 4; i++){
			Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, i, True), True);
		}
		Emulator_x86_Push_Data_To_Stack(env, Temp, False);
		for(i = 5; i < 8; i++){
			Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, i, True), True);
		}
	} else{	/*PUSHA*/
		Emulator_x86_Put_EmulationInformation(env, "PUSHA");
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

	if(env->operation_32bit){	/*POPAD*/
		Emulator_x86_Put_EmulationInformation(env, "POPAD");
		for(i = 7; i > 4; i--){
			Emulator_x86_MoveToGReg(env, i, Emulator_x86_Pop_Data_From_Stack(env, True), True);
		}
		Emulator_x86_Pop_Data_From_Stack(env, True);
		for(i = 3; i > -1; i--){
			Emulator_x86_MoveToGReg(env, i, Emulator_x86_Pop_Data_From_Stack(env, True), True);
		}
	} else{	/*POPA*/
		Emulator_x86_Put_EmulationInformation(env, "POPA");
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
			Emulator_x86_Put_EmulationInformation(env, "MOV %s, %s", Emulator_x86_ByteRegNames[modrm.bit.Reg], Emulator_x86_ByteRegNames[modrm.bit.RM]);
		} else{	/*From MemoryAddress*/
			addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			Emulator_x86_MoveToGReg(env, modrm.bit.Reg, *((uchar *)addr), False);
			Emulator_x86_Put_EmulationInformation(env, "MOV %s, [0x%X]", Emulator_x86_ByteRegNames[modrm.bit.Reg], addr);
		}
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit mode is not implemented.");
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
	Emulator_x86_Put_EmulationInformation(env, "CMP AL, 0x%X (CF:%d PF:%d AF:%d ZF:%d SF:%d OF:%d)", imm, env->EFLAGS.bit.CF, env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);

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

	Emulator_x86_Put_EmulationInformation(env, "JE 0x%X ", new_eip);

	if(env->EFLAGS.bit.ZF){
		Emulator_x86_Put_EmulationInformation(env, "Taken.");
		env->EIP = new_eip;
	} else{
		Emulator_x86_Put_EmulationInformation(env, "NotTaken.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_INC_RegOnly(Emulator_x86_Environment *env)
{
	uint operand;
	CPU_EFlags eflags;

	if((env->now_opcode - 0x40) > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid GReg From OpCode).");
	}

	operand = Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x40, True);
	Emulator_x86_MoveToGReg(env, env->now_opcode - 0x40, operand + 1, True);

	eflags.eflags = asm_Emulator_x86_Get_EFlags_Increment(operand);
	env->EFLAGS.bit.PF = eflags.bit.PF;
	env->EFLAGS.bit.AF = eflags.bit.AF;
	env->EFLAGS.bit.ZF = eflags.bit.ZF;
	env->EFLAGS.bit.SF = eflags.bit.SF;
	env->EFLAGS.bit.OF = eflags.bit.OF;
	Emulator_x86_Put_EmulationInformation(env, "INC %s (PF:%d AF:%d ZF:%d SF:%d OF:%d)", Emulator_x86_RegNames[env->now_opcode - 0x40], env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);

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

	Emulator_x86_Put_EmulationInformation(env, "JMP 0x%X", new_eip);

	env->EIP = new_eip;

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_RET_Near(Emulator_x86_Environment *env)
{
	Emulator_x86_Pop_eIP_From_Stack(env);
	Emulator_x86_Put_EmulationInformation(env, "near RET to 0x%X", env->EIP);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_PUSH_RegOnly(Emulator_x86_Environment *env)
{
	if((env->now_opcode - 0x50) > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid GReg From OpCode).");
	}
	if(!env->operation_32bit){	/*16-bit*/
		Emulator_x86_Put_EmulationInformation(env, "PUSH %s", Emulator_x86_RegNames[env->now_opcode - 0x50]);
		Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x50, True), False);
	} else{	/*32-bit*/
		Emulator_x86_Put_EmulationInformation(env, "PUSH E%s", Emulator_x86_RegNames[env->now_opcode - 0x50]);
		Emulator_x86_Push_Data_To_Stack(env, Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x50, True), True);
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_XOR_Eb_Gb(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;
	CPU_EFlags eflags;
	uint dest_op, source_op;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);
	addr = 0;
	dest_op = 0;
	source_op = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		source_op = Emulator_x86_MoveFromGReg(env, modrm.bit.Reg, True);
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg*/
			dest_op = Emulator_x86_MoveFromGReg(env, modrm.bit.RM, True);
			Emulator_x86_MoveToGReg(env, modrm.bit.Reg, dest_op ^ source_op, True);
			Emulator_x86_Put_EmulationInformation(env, "XOR %s, %s", Emulator_x86_RegNames[modrm.bit.RM], Emulator_x86_RegNames[modrm.bit.Reg]);
		} else{	/*MemoryAddress*/
			addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			dest_op = *((ushort *)addr);
			Emulator_x86_Put_EmulationInformation(env, "XOR [0x%X], %s", addr, Emulator_x86_RegNames[modrm.bit.Reg]);
			*((ushort *)addr) = dest_op ^ source_op;
		}
		eflags.eflags = asm_Emulator_x86_Get_EFlags_XOR(dest_op, source_op);
		env->EFLAGS.bit.CF = False;
		env->EFLAGS.bit.PF = eflags.bit.PF;
		env->EFLAGS.bit.AF = eflags.bit.AF;
		env->EFLAGS.bit.ZF = eflags.bit.ZF;
		env->EFLAGS.bit.SF = eflags.bit.SF;
		env->EFLAGS.bit.OF = False;
		Emulator_x86_Put_EmulationInformation(env, " (CF:%d PF:%d AF:%d ZF:%d SF:%d OF:%d)", env->EFLAGS.bit.CF, env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_DEC_RegOnly(Emulator_x86_Environment *env)
{
	uint operand;
	CPU_EFlags eflags;

	if((env->now_opcode - 0x48) > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid GReg From OpCode).");
	}

	operand = Emulator_x86_MoveFromGReg(env, env->now_opcode - 0x48, True);
	Emulator_x86_MoveToGReg(env, env->now_opcode - 0x48, operand - 1, True);

	eflags.eflags = asm_Emulator_x86_Get_EFlags_Decrement(operand);
	env->EFLAGS.bit.PF = eflags.bit.PF;
	env->EFLAGS.bit.AF = eflags.bit.AF;
	env->EFLAGS.bit.ZF = eflags.bit.ZF;
	env->EFLAGS.bit.SF = eflags.bit.SF;
	env->EFLAGS.bit.OF = eflags.bit.OF;
	Emulator_x86_Put_EmulationInformation(env, "DEC %s (PF:%d AF:%d ZF:%d SF:%d OF:%d)", Emulator_x86_RegNames[env->now_opcode - 0x48], env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_eAX_Ov(Emulator_x86_Environment *env)
{
	uint offset;
	uint addr;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		offset = Emulator_x86_FetchCode(env, 2);
		addr = Emulator_x86_Get_EffectivePhysicalAddress(env, env->operation_prefix_segment, offset);
		Emulator_x86_MoveToGReg(env, OPCODE_REG_EAX, *((ushort *)addr), True);
		Emulator_x86_Put_EmulationInformation(env, "MOV AX, [0x%X]", addr);
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_Ov_eAX(Emulator_x86_Environment *env)
{
	uint offset;
	uint addr;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		offset = Emulator_x86_FetchCode(env, 2);
		addr = Emulator_x86_Get_EffectivePhysicalAddress(env, env->operation_prefix_segment, offset);
		*((ushort *)addr) = Emulator_x86_MoveFromGReg(env, OPCODE_REG_EAX, True);
		Emulator_x86_Put_EmulationInformation(env, "MOV [0x%X], AX", addr);
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_OUT_AL(Emulator_x86_Environment *env)
{
	uint port;

	port = Emulator_x86_FetchCode(env, 1);

	Emulator_x86_Put_EmulationInformation(env, "OUT 0x%X, AL(Not implemented)", port);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_CMP_Gv_Ev(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;
	uint op0, op1;
	CPU_EFlags eflags;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);
	addr = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		op0 = Emulator_x86_MoveFromGReg(env, modrm.bit.Reg, True);
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg*/
			op1 = Emulator_x86_MoveFromGReg(env, modrm.bit.RM, True);
			Emulator_x86_Put_EmulationInformation(env, "CMP %s, %s", Emulator_x86_RegNames[modrm.bit.Reg], Emulator_x86_RegNames[modrm.bit.RM]);
		} else{	/*MemoryAddress*/
			addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			op1 = *((ushort *)addr);
			Emulator_x86_Put_EmulationInformation(env, "CMP %s, [0x%X]", Emulator_x86_RegNames[modrm.bit.Reg], addr);
		}
		eflags.eflags = asm_Emulator_x86_Get_EFlags_Compare(op0, op1);
		env->EFLAGS.bit.CF = eflags.bit.CF;
		env->EFLAGS.bit.PF = eflags.bit.PF;
		env->EFLAGS.bit.AF = eflags.bit.AF;
		env->EFLAGS.bit.ZF = eflags.bit.ZF;
		env->EFLAGS.bit.SF = eflags.bit.SF;
		env->EFLAGS.bit.OF = eflags.bit.OF;
		Emulator_x86_Put_EmulationInformation(env, " (CF:%d PF:%d AF:%d ZF:%d SF:%d OF:%d)", env->EFLAGS.bit.CF, env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_LOOP_Jv(Emulator_x86_Environment *env)
{
	char relative_addr;
	uchar sign;
	uint new_eip;
	uint operand;

	sign = False;
	relative_addr = Emulator_x86_FetchCode(env, 1);
	new_eip = env->EIP;

	operand = Emulator_x86_MoveFromGReg(env, OPCODE_REG_ECX, True);
	Emulator_x86_MoveToGReg(env, OPCODE_REG_ECX, operand - 1, True);

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

	Emulator_x86_Put_EmulationInformation(env, "LOOP 0x%X", new_eip);

	if(0 == operand - 1){
		Emulator_x86_Put_EmulationInformation(env, "(NotTaken)");
	} else{
		Emulator_x86_Put_EmulationInformation(env, "(Taken)");
		env->EIP = new_eip;
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_POP_Ev(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);
	addr = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg only*/
			Emulator_x86_MoveToGReg(env, modrm.bit.RM, Emulator_x86_Pop_Data_From_Stack(env, False), True);
			Emulator_x86_Put_EmulationInformation(env, "POP %s", Emulator_x86_RegNames[modrm.bit.RM]);
		} else{	/*From MemoryAddress*/
			addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			*((ushort *)addr) = Emulator_x86_Pop_Data_From_Stack(env, False);
			Emulator_x86_Put_EmulationInformation(env, "POP [0x%X]", addr);
		}
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;	
}

void Emulator_x86_Operation_POP_RegOnly(Emulator_x86_Environment *env)
{
	if((env->now_opcode - 0x58) > 7){
		Emulator_x86_Put_EmulationInformation(env, "\nEmulator-x86:Internal Error(Invalid GReg From OpCode).");
	}
	if(!env->operation_32bit){	/*16-bit*/
		Emulator_x86_Put_EmulationInformation(env, "POP %s", Emulator_x86_RegNames[env->now_opcode - 0x58]);
		Emulator_x86_MoveToGReg(env, env->now_opcode - 0x58, Emulator_x86_Pop_Data_From_Stack(env, False), True);
	} else{	/*32-bit*/
		Emulator_x86_Put_EmulationInformation(env, "POP E%s", Emulator_x86_RegNames[env->now_opcode - 0x58]);
		Emulator_x86_MoveToGReg(env, env->now_opcode - 0x58, Emulator_x86_Pop_Data_From_Stack(env, True), True);
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_Jcc_JNE_rel8(Emulator_x86_Environment *env)
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

	Emulator_x86_Put_EmulationInformation(env, "JNE 0x%X ", new_eip);

	if(!env->EFLAGS.bit.ZF){
		Emulator_x86_Put_EmulationInformation(env, "Taken.");
		env->EIP = new_eip;
	} else{
		Emulator_x86_Put_EmulationInformation(env, "NotTaken.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_STI(Emulator_x86_Environment *env)
{
	env->EFLAGS.bit.IF = True;
	Emulator_x86_Put_EmulationInformation(env, "STI");

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_SBB_Gv_Ev(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;
	uint op0, op1;
	CPU_EFlags eflags;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);
	addr = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		op0 = Emulator_x86_MoveFromGReg(env, modrm.bit.Reg, True);
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg*/
			op1 = Emulator_x86_MoveFromGReg(env, modrm.bit.RM, True);
			Emulator_x86_Put_EmulationInformation(env, "SBB %s, %s(CF:%d)", Emulator_x86_RegNames[modrm.bit.Reg], Emulator_x86_RegNames[modrm.bit.RM], env->EFLAGS.bit.CF);
		} else{	/*MemoryAddress*/
			addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			op1 = *((ushort *)addr);
			Emulator_x86_Put_EmulationInformation(env, "SBB %s, [0x%X](CF:%d)", Emulator_x86_RegNames[modrm.bit.Reg], addr, env->EFLAGS.bit.CF);
		}
		eflags.eflags = asm_Emulator_x86_Get_EFlags_Subtract_with_Borrow(&op0, op1, env->EFLAGS.bit.CF);
		Emulator_x86_MoveToGReg(env, modrm.bit.RM, op0, True);
		env->EFLAGS.bit.CF = eflags.bit.CF;
		env->EFLAGS.bit.PF = eflags.bit.PF;
		env->EFLAGS.bit.AF = eflags.bit.AF;
		env->EFLAGS.bit.ZF = eflags.bit.ZF;
		env->EFLAGS.bit.SF = eflags.bit.SF;
		env->EFLAGS.bit.OF = eflags.bit.OF;
		Emulator_x86_Put_EmulationInformation(env, " (CF:%d PF:%d AF:%d ZF:%d SF:%d OF:%d)", env->EFLAGS.bit.CF, env->EFLAGS.bit.PF, env->EFLAGS.bit.AF, env->EFLAGS.bit.ZF, env->EFLAGS.bit.SF, env->EFLAGS.bit.OF);
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_MOV_Ev_Gv(Emulator_x86_Environment *env)
{
	Emulator_x86_OperationCode_ModRM modrm;
	uint addr;
	uint op1;

	modrm.modrm = Emulator_x86_FetchCode(env, 1);
	addr = 0;

	if(!env->operation_32bit){	/*16-bit Addressing*/
		op1 = Emulator_x86_MoveFromGReg(env, modrm.bit.Reg, True);
		if(modrm.bit.Mod == OPCODE_MOD_REGISTER){	/*Reg*/
			Emulator_x86_MoveToGReg(env, modrm.bit.RM, op1, True);
			Emulator_x86_Put_EmulationInformation(env, "MOV %s, %s", Emulator_x86_RegNames[modrm.bit.RM], Emulator_x86_RegNames[modrm.bit.Reg]);
		} else{	/*MemoryAddress*/
			addr = Emulator_x86_Get_EffectivePhysicalAddress_FromModRM(env, modrm.modrm);
			*((ushort *)addr) = op1;
			Emulator_x86_Put_EmulationInformation(env, "MOV [0x%X], %s(0x%X)", addr, Emulator_x86_RegNames[modrm.bit.Reg], op1);
		}
	} else{	/*32-bit Addressing*/
		Emulator_x86_Put_EmulationInformation(env, "32-bit addressing is not implemented.");
	}

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_PUSH_Ib(Emulator_x86_Environment *env)
{
	//16bit/32bit?
	uint imm8;

	imm8 = Emulator_x86_FetchCode(env, 1);
	Emulator_x86_Push_Data_To_Stack(env, imm8, env->operation_32bit);
	Emulator_x86_Put_EmulationInformation(env, "PUSH 0x%X", imm8);

	env->operation_end = True;

	return;
}

void Emulator_x86_Operation_CLD(Emulator_x86_Environment *env)
{
	env->EFLAGS.bit.DF = False;
	Emulator_x86_Put_EmulationInformation(env, "CLD");

	env->operation_end = True;

	return;
}
