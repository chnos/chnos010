[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_putstrwin

[SECTION .text]

_api_putstrwin:
;lenは無視されます。文字列は終端にnullを置くこと。
	push	edi
	push	esi
	push	ebp
	push	ebx
	mov	edx,6
	mov	ebx,[esp+20]
	mov	esi,[esp+24]
	mov	edi,[esp+28]
	mov	eax,[esp+32]
	mov	ecx,[esp+36]
	mov	ebp,[esp+40]
	int	0x40
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret
