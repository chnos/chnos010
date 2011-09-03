[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:
	mov	edx,1
	mov	al,[esp+4]
	int	0x40
	ret
