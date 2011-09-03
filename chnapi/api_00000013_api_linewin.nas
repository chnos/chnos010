[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_linewin

[SECTION .text]


_api_linewin:
	push	edi
	push	esi
	push	ebp
	push	ebx
	mov	edx,13
	mov	ebx,[esp+20]
	mov	eax,[esp+24]
	mov	ecx,[esp+28]
	mov	esi,[esp+32]
	mov	edi,[esp+36]
	mov	ebp,[esp+40]
	int	0x40
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret
