[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_initmalloc

[SECTION .text]

_api_initmalloc:
	push	ebx
	mov	edx,8
	mov	ebx,[cs:0x0020]
	mov	eax,ebx
	add	eax,32*1024
	mov	ecx,[cs:0x0000]
	sub	ecx,eax
	int	0x40
	pop	ebx
	ret
