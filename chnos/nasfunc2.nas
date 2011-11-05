[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 16]
[FILE "nasfunc2.nas"]

	GLOBAL	_asm_16bit_CallBIOSTask

[SECTION .text]
_asm_16bit_CallBIOSTask:
	org	0xc200
_asm_16bit_CallBIOSTask_Code:
	int	0x0d
_asm_16bit_CallBIOSTask_Code_End:
	db	0x0f, 0x0b	;ud2
