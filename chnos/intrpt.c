
#include "core.h"

void Initialise_ProgrammableInterruptController(void)
{
	/*
	IRR：インタラプトリクエストレジスタ 
		.IRQピンの状態を表す。ポートを読んだときに、IRRになるかISRになるかは、OCW3で選択する。
		.1になっているビットは、現在要求が来ている（もしくは処理中の）割り込み。
	ISR：インサービスレジスタ 
		.現在処理中の割り込みがどれであるかを示す。ポートを読んだときに、IRRになるかISRになるかは、OCW3で選択する。
		.1になっているビットは、現在処理中の割り込み。処理中というのは、CPUに対してINT命令を発行したが、EOI（割り込み終了命令）を受け取っていない割り込み。
	IMR：インタラプトマスクレジスタ 
		.これが1になっている割り込みは、IRRが1になっていても、反応しない。 
	*/

	IO_Out8(PIC0_IMR, 0xff);	/*割り込み全部無視（マスタ）*/
	IO_Out8(PIC1_IMR, 0xff);	/*割り込み全部無視（スレーブ）*/

	IO_Out8(PIC0_ICW1, 0x11);	/*エッジトリガモードに設定（マスタ）*/
	IO_Out8(PIC0_ICW2, 0x20);	/*割り込み番号を、20~27に設定。（マスタ）*/
	IO_Out8(PIC0_ICW3, 1 << 2);	/*00000100 つまり、スレーブはIRQ2につながってますよということ。*/
	IO_Out8(PIC0_ICW4, 0x01);	/*ノンバッファモード（マスタ）*/

	IO_Out8(PIC1_ICW1, 0x11);	/*エッジトリガモードに設定（スレーブ）*/
	IO_Out8(PIC1_ICW2, 0x28);	/*割り込み番号を、28~2fに設定。（スレーブ）*/
	IO_Out8(PIC1_ICW3, 2);		/*自分はマスタのIRQ2番につながってますということ。*/
	IO_Out8(PIC1_ICW4, 0x01);	/*ノンバッファモード（スレーブ）*/

	IO_Out8(PIC0_IMR, 0xff);	/*割り込み全部無視（マスタ）*/
	IO_Out8(PIC1_IMR, 0xff);	/*割り込み全部無視（スレーブ）*/

	System_GateDescriptor_Set(0x27, (uint)asm_InterruptHandler27, 0x02, AR_INTGATE32);	/*IRQ-07対策*/
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
