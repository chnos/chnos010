[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "nasfunc0.nas"]

	GLOBAL	_asm_Emulator_x86_Get_EFlags_Compare
	GLOBAL	_asm_Emulator_x86_Get_EFlags_Increment

[SECTION .text]

_asm_Emulator_x86_Get_EFlags_Compare:
	mov	eax, [esp + 4]
	cmp	eax, [esp + 8]
	pushfd
	pop	eax
	ret

_asm_Emulator_x86_Get_EFlags_Increment:
	mov	eax, [esp + 4]
	inc	eax
	pushfd
	pop	eax
	ret

