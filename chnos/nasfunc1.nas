[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "nasfunc1.nas"]

	GLOBAL	_asm_CPU_ExceptionHandler00
	EXTERN	_CPU_ExceptionHandler00

	GLOBAL	_asm_CPU_ExceptionHandler01
	EXTERN	_CPU_ExceptionHandler01

	GLOBAL	_asm_CPU_ExceptionHandler02
	EXTERN	_CPU_ExceptionHandler02

	GLOBAL	_asm_CPU_ExceptionHandler03
	EXTERN	_CPU_ExceptionHandler03

	GLOBAL	_asm_CPU_ExceptionHandler04
	EXTERN	_CPU_ExceptionHandler04

	GLOBAL	_asm_CPU_ExceptionHandler05
	EXTERN	_CPU_ExceptionHandler05

	GLOBAL	_asm_CPU_ExceptionHandler06
	EXTERN	_CPU_ExceptionHandler06

	GLOBAL	_asm_CPU_ExceptionHandler07
	EXTERN	_CPU_ExceptionHandler07

	GLOBAL	_asm_CPU_ExceptionHandler08
	EXTERN	_CPU_ExceptionHandler08

	GLOBAL	_asm_CPU_ExceptionHandler09
	EXTERN	_CPU_ExceptionHandler09

	GLOBAL	_asm_CPU_ExceptionHandler0a
	EXTERN	_CPU_ExceptionHandler0a

	GLOBAL	_asm_CPU_ExceptionHandler0b
	EXTERN	_CPU_ExceptionHandler0b

	GLOBAL	_asm_CPU_ExceptionHandler0c
	EXTERN	_CPU_ExceptionHandler0c

	GLOBAL	_asm_CPU_ExceptionHandler0d
	EXTERN	_CPU_ExceptionHandler0d

	GLOBAL	_asm_CPU_ExceptionHandler0e
	EXTERN	_CPU_ExceptionHandler0e

	GLOBAL	_asm_CPU_ExceptionHandler0f
	EXTERN	_CPU_ExceptionHandler0f

	GLOBAL	_asm_CPU_ExceptionHandler10
	EXTERN	_CPU_ExceptionHandler10

	GLOBAL	_asm_CPU_ExceptionHandler11
	EXTERN	_CPU_ExceptionHandler11

	GLOBAL	_asm_CPU_ExceptionHandler12
	EXTERN	_CPU_ExceptionHandler12

	GLOBAL	_asm_CPU_ExceptionHandler13
	EXTERN	_CPU_ExceptionHandler13

	GLOBAL	_asm_CPU_ExceptionHandler14
	EXTERN	_CPU_ExceptionHandler14

	GLOBAL	_asm_CPU_ExceptionHandler15
	EXTERN	_CPU_ExceptionHandler15

	GLOBAL	_asm_CPU_ExceptionHandler16
	EXTERN	_CPU_ExceptionHandler16

	GLOBAL	_asm_CPU_ExceptionHandler17
	EXTERN	_CPU_ExceptionHandler17

	GLOBAL	_asm_CPU_ExceptionHandler18
	EXTERN	_CPU_ExceptionHandler18

	GLOBAL	_asm_CPU_ExceptionHandler19
	EXTERN	_CPU_ExceptionHandler19

	GLOBAL	_asm_CPU_ExceptionHandler1a
	EXTERN	_CPU_ExceptionHandler1a

	GLOBAL	_asm_CPU_ExceptionHandler1b
	EXTERN	_CPU_ExceptionHandler1b

	GLOBAL	_asm_CPU_ExceptionHandler1c
	EXTERN	_CPU_ExceptionHandler1c

	GLOBAL	_asm_CPU_ExceptionHandler1d
	EXTERN	_CPU_ExceptionHandler1d

	GLOBAL	_asm_CPU_ExceptionHandler1e
	EXTERN	_CPU_ExceptionHandler1e

	GLOBAL	_asm_CPU_ExceptionHandler1f
	EXTERN	_CPU_ExceptionHandler1f

	GLOBAL	_asm_InterruptHandler27
	EXTERN	_InterruptHandler27

[SECTION .text]

_asm_CPU_ExceptionHandler00:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler00
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler01:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler01
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler02:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler02
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler03:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler03
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler04:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler04
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler05:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler05
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler06:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler06
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler07:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler07
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler08:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler08
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler09:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler09
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler0a:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler0a
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler0b:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler0b
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler0c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler0c
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler0d:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler0d
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler0e:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler0e
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler0f:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler0f
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler10:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler10
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler11:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler11
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler12:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler12
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler13:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler13
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler14:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler14
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler15:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler15
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler16:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler16
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler17:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler17
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler18:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler18
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler19:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler19
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler1a:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler1a
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler1b:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler1b
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler1c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler1c
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler1d:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler1d
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler1e:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler1e
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_CPU_ExceptionHandler1f:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_CPU_ExceptionHandler1f
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

_asm_InterruptHandler27:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_InterruptHandler27
	pop	eax
	popad
	pop	ds
	pop	es
	iretd
