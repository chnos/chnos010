[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_closewin

[SECTION .text]


_api_closewin:
	push	ebx
	mov	edx,14
	mov	ebx,[esp+8]
	int	0x40
	pop	ebx
	ret
