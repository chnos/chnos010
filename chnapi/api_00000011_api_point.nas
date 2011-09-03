[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_point

[SECTION .text]


_api_point:
	push	edi
	push	esi
	push	ebx
	mov	edx,11
	mov	ebx,[esp+16]
	mov	esi,[esp+20]
	mov	edi,[esp+24]
	mov	eax,[esp+28]
	int	0x40
	pop	ebx
	pop	esi
	pop	edi
	ret
