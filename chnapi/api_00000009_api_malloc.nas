[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_malloc

[SECTION .text]

_api_malloc:
	push	ebx
	mov	edx,9
	mov	ebx,[cs:0x0020]
	mov	ecx,[esp+8]
	int	0x40
	pop	ebx
	ret
