[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_free

[SECTION .text]


_api_free:
	push	ebx
	mov	edx,10
	mov	ebx,[cs:0x0020]
	mov	eax,[esp+8]
	mov	ecx,[esp+12]
	int	0x40
	pop	ebx
	ret
