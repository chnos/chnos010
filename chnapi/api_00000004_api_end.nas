[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_end

[SECTION .text]

_api_end:
	mov	edx,4
	int	0x40
