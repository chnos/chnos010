[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_getkey

[SECTION .text]


_api_getkey:
	mov	edx,15
	mov	eax,[esp+4]
	int	0x40
	ret
