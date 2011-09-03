
#include "core.h"

void Initialise_ProgrammableInterruptController(void)
{
	/*
	IRR�F�C���^���v�g���N�G�X�g���W�X�^ 
		.IRQ�s���̏�Ԃ�\���B�|�[�g��ǂ񂾂Ƃ��ɁAIRR�ɂȂ邩ISR�ɂȂ邩�́AOCW3�őI������B
		.1�ɂȂ��Ă���r�b�g�́A���ݗv�������Ă���i�������͏������́j���荞�݁B
	ISR�F�C���T�[�r�X���W�X�^ 
		.���ݏ������̊��荞�݂��ǂ�ł��邩�������B�|�[�g��ǂ񂾂Ƃ��ɁAIRR�ɂȂ邩ISR�ɂȂ邩�́AOCW3�őI������B
		.1�ɂȂ��Ă���r�b�g�́A���ݏ������̊��荞�݁B�������Ƃ����̂́ACPU�ɑ΂���INT���߂𔭍s�������AEOI�i���荞�ݏI�����߁j���󂯎���Ă��Ȃ����荞�݁B
	IMR�F�C���^���v�g�}�X�N���W�X�^ 
		.���ꂪ1�ɂȂ��Ă��銄�荞�݂́AIRR��1�ɂȂ��Ă��Ă��A�������Ȃ��B 
	*/

	IO_Out8(PIC0_IMR, 0xff);	/*���荞�ݑS�������i�}�X�^�j*/
	IO_Out8(PIC1_IMR, 0xff);	/*���荞�ݑS�������i�X���[�u�j*/

	IO_Out8(PIC0_ICW1, 0x11);	/*�G�b�W�g���K���[�h�ɐݒ�i�}�X�^�j*/
	IO_Out8(PIC0_ICW2, 0x20);	/*���荞�ݔԍ����A20~27�ɐݒ�B�i�}�X�^�j*/
	IO_Out8(PIC0_ICW3, 1 << 2);	/*00000100 �܂�A�X���[�u��IRQ2�ɂȂ����Ă܂���Ƃ������ƁB*/
	IO_Out8(PIC0_ICW4, 0x01);	/*�m���o�b�t�@���[�h�i�}�X�^�j*/

	IO_Out8(PIC1_ICW1, 0x11);	/*�G�b�W�g���K���[�h�ɐݒ�i�X���[�u�j*/
	IO_Out8(PIC1_ICW2, 0x28);	/*���荞�ݔԍ����A28~2f�ɐݒ�B�i�X���[�u�j*/
	IO_Out8(PIC1_ICW3, 2);		/*�����̓}�X�^��IRQ2�ԂɂȂ����Ă܂��Ƃ������ƁB*/
	IO_Out8(PIC1_ICW4, 0x01);	/*�m���o�b�t�@���[�h�i�X���[�u�j*/

	IO_Out8(PIC0_IMR, 0xff);	/*���荞�ݑS�������i�}�X�^�j*/
	IO_Out8(PIC1_IMR, 0xff);	/*���荞�ݑS�������i�X���[�u�j*/

	System_GateDescriptor_Set(0x27, (uint)asm_InterruptHandler27, 0x02, AR_INTGATE32);	/*IRQ-07�΍�*/
	ProgrammableInterruptController_InterruptMask_Clear(0x07);

	return;
}

void ProgrammableInterruptController_InterruptMask_Clear(uint irq)
{
	uint mask;

	mask = 1;

	if(irq >= 16){
		return;
	}

	if(irq < 8){	/*For Master*/
		IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & ~(mask << irq));
	} else{	/*For Slave*/
		irq -= 8;
		IO_Out8(PIC1_IMR, IO_In8(PIC1_IMR) & ~(mask << irq));
	}

	return;
}

void ProgrammableInterruptController_InterruptRequest_Complete(uint irq)
{
	if(irq >= 16){
		return;
	}

	if(irq < 8){	/*For Master*/
		IO_Out8(PIC0_OCW2, 0x60 + irq);
	} else{	/*For Slave*/
		IO_Out8(PIC0_OCW2, 0x60 + 0x02);
		irq -= 8;
		IO_Out8(PIC1_OCW2, 0x60 + irq);
	}
	return;
}

void InterruptHandler27(uint *esp)
{
	ProgrammableInterruptController_InterruptRequest_Complete(0x07);
	return;
}
