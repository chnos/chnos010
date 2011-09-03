[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]

	GLOBAL	_CHNMain

[SECTION .text]

_CHNMain:
	mov	edx, 2
	mov	ebx, msg
	int	0x40
	mov	edx, 4
	int	0x40
msg:
	db	"hello", 0
