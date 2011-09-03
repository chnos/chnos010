[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]

	GLOBAL	_CHNMain

[SECTION .text]

_CHNMain:
	mov	eax,1*8
	mov	ds,ax
	mov	byte[0x102600],0
	mov	edx, 4
	int	0x40

