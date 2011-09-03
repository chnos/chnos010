[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_refreshwin

[SECTION .text]


_api_refreshwin:
	push	edi
	push	esi
	push	ebx
	mov	edx,12
	mov	ebx,[esp+16]
	mov	eax,[esp+20]
	mov	ecx,[esp+24]
	mov	esi,[esp+28]
	mov	edi,[esp+32]
	int	0x40
	pop	ebx
	pop	esi
	pop	edi
	ret
