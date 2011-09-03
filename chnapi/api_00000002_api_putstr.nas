[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_putstr

[SECTION .text]

_api_putstr:
	push	ebx
	mov	edx,2
	mov	ebx,[esp+8]
	int	0x40
	pop	ebx
	ret
