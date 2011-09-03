[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]

	GLOBAL	_CHNMain

[SECTION .text]

_CHNMain:
	cli
fin:
	hlt
	jmp	fin

