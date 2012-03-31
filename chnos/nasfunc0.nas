[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "nasfunc0.nas"]
	GLOBAL	_IO_HLT
	GLOBAL	_IO_CLI, _IO_STI, _IO_STIHLT
	GLOBAL	_IO_In8, _IO_Out8
	GLOBAL	_IO_In16, _IO_Out16
	GLOBAL	_IO_In32, _IO_Out32
	GLOBAL	_IO_Load_EFlags, _IO_Store_EFlags
	GLOBAL	_Load_CR0, _Store_CR0
	GLOBAL	_Load_CR2, _Store_CR2
	GLOBAL	_Load_CR3, _Store_CR3
	GLOBAL	_Load_CR4, _Store_CR4
	GLOBAL	_Load_DR0, _Store_DR0
	GLOBAL	_Load_DR1, _Store_DR1
	GLOBAL	_Load_DR2, _Store_DR2
	GLOBAL	_Load_DR3, _Store_DR3
	GLOBAL	_Load_DR6, _Store_DR6
	GLOBAL	_Load_DR7, _Store_DR7
	GLOBAL	_Load_GDTR
	GLOBAL	_Load_IDTR
	GLOBAL	_Load_TR
	GLOBAL	_FarJMP
	GLOBAL	_FarCall
	GLOBAL	_CLTS
	GLOBAL	_FNSave, _FRStore
	GLOBAL	_PIT_Beep_On, _PIT_Beep_Off, _PIT_Beep_Set
	GLOBAL	_CPUID, _CPUID2
	GLOBAL	_TSC_Read
	GLOBAL	_Memory_Test_Sub
	GLOBAL	_INT_3
	GLOBAL	_DIV_64_32, _MOD_64_32
	GLOBAL	_MOVSD_ZeroFill

[SECTION .text]

_IO_HLT:
	hlt
	ret

_IO_CLI:
	cli
	ret

_IO_STI:
	sti
	ret

_IO_STIHLT:
	sti
	hlt
	ret

_IO_In8:
	mov	edx,[esp+4]
	mov	eax,0
	in	al,dx
	ret

_IO_Out8:
	mov	edx,[esp+4]
	mov	al,[esp+8]
	out	dx,al
	ret

_IO_In16:
	mov	edx,[esp+4]
	mov	eax,0
	in	ax,dx
	ret

_IO_Out16:
	mov	edx,[esp+4]
	mov	ax,[esp+8]
	out	dx,ax
	ret

_IO_In32:
	mov	edx,[esp+4]
	in	eax,dx
	ret

_IO_Out32:
	mov	edx,[esp+4]
	mov	eax,[esp+8]
	out	dx,eax
	ret

_IO_Load_EFlags:
	pushfd
	pop	eax
	ret

_IO_Store_EFlags:
	mov	eax,[esp+4]
	push	eax
	popfd
	ret

_Load_CR0:
	mov	eax,cr0
	ret

_Store_CR0:
	mov	eax,[esp+4]
	mov	cr0,eax
	ret

_Load_CR2:
	mov	eax,cr2
	ret

_Store_CR2:
	mov	eax,[esp+4]
	mov	cr2,eax
	ret

_Load_CR3:
	mov	eax,cr3
	ret

_Store_CR3:
	mov	eax,[esp+4]
	mov	cr3,eax
	ret

_Load_CR4:
	mov	eax,cr3
	ret

_Store_CR4:
	mov	eax,[esp+4]
	mov	cr4,eax
	ret

_Load_DR0:
	mov	eax,dr0
	ret

_Store_DR0:
	mov	eax,[esp+4]
	mov	dr0,eax
	ret

_Load_DR1:
	mov	eax,dr1
	ret

_Store_DR1:
	mov	eax,[esp+4]
	mov	dr1,eax
	ret

_Load_DR2:
	mov	eax,dr2
	ret

_Store_DR2:
	mov	eax,[esp+4]
	mov	dr2,eax
	ret

_Load_DR3:
	mov	eax,dr3
	ret

_Store_DR3:
	mov	eax,[esp+4]
	mov	dr3,eax
	ret

_Load_DR6:
	mov	eax,dr6
	ret

_Store_DR6:
	mov	eax,[esp+4]
	mov	dr6,eax
	ret

_Load_DR7:
	mov	eax,dr7
	ret

_Store_DR7:
	mov	eax,[esp+4]
	mov	dr7,eax
	ret

_Load_GDTR:
	mov	ax,[esp+4]
	mov	[esp+6],ax
	lgdt	[esp+6]
	ret

_Load_IDTR:
	mov	ax,[esp+4]
	mov	[esp+6],ax
	lidt	[esp+6]
	ret

_Load_TR:
	ltr	[esp+4]
	ret

_FarJMP:
	jmp	far	[esp+4]
	ret

_FarCall:
	call	far	[esp+4]
	ret

_CLTS:
        clts
        ret

_FNSave:
        mov     eax,[esp+4]     ; addr
        fnsave  [eax]
        ret

_FRStore:
        mov     eax,[esp+4]     ; addr
        frstor  [eax]
        ret

_PIT_Beep_On:
	in	al,0x61
	or	al,0x03
	and	al,0x0f
	out	0x61,al
	ret

_PIT_Beep_Off:
	in	al,0x61
	and	al,0xd
	out	0x61,al
	ret

_PIT_Beep_Set:
	mov	eax,1193180
	mov	edx,0x00000000
	mov	ecx,[esp+4]
	div	ecx
	mov	edx,eax
	mov	al,0xb6
	out	0x43,al
	mov	eax,edx
	out	0x42,al
	shr	eax,8
	out	0x42,al
	ret

_CPUID:
	pushad
	mov	esi,[esp+36]
	mov	eax,[esp+40]
	db	0x0f,0xa2
	mov	[esi   ],eax
	mov	[esi+ 4],ebx
	mov	[esi+ 8],edx
	mov	[esi+12],ecx
	popad
	ret

_CPUID2:
	pushad
	mov	esi,[esp+36]
	mov	eax,[esp+40]
	db	0x0f,0xa2
	mov	[esi   ],eax
	mov	[esi+ 4],ebx
	mov	[esi+ 8],ecx
	mov	[esi+12],edx
	popad
	ret

_TSC_Read:
	pushad
	mov	ebx,[esp+36]
	db	0x0f,0x31
	mov	[ebx],edx
	mov	[ebx+4],eax
	popad
	ret

_Memory_Test_Sub:
	push	edi
	push	esi
	push	ebx
	mov	esi,0xaa55aa55
	mov	edi,0x55aa55aa
	mov	eax,[esp+12+4]
mts_loop:
	mov	ebx,eax
	add	ebx,0xffc
	mov	edx,[ebx]
	mov	[ebx],esi
	xor	dword [ebx],0xffffffff
	cmp	edi,[ebx]
	jne	mts_fin
	xor	dword [ebx],0xffffffff
	cmp	esi,[ebx]
	jne	mts_fin
	mov	[ebx],edx
	add	eax,0x1000
	cmp	eax,[esp+12+8]
	jbe	mts_loop
	pop	ebx
	pop	esi
	pop	edi
	ret
mts_fin:
	mov	[ebx],edx
	pop	ebx
	pop	esi
	pop	edi
	ret

_INT_3:
	int	3
	ret

_DIV_64_32:
	mov	eax,[esp+4]
	mov	edx,[esp+8]
	div	dword[esp+12]
	ret

_MOD_64_32:
	mov	eax,[esp+4]
	mov	edx,[esp+8]
	div	dword[esp+12]
	mov	eax,edx
	ret

_MOVSD_ZeroFill:
	cld
	push	esi
	push	edi
	mov	ax, ds
	mov	es, ax
	mov	eax, 0x00000000
	mov	edi, [esp+8+4]
	mov	ecx, [esp+8+8]
	shr	ecx, 2
	rep stosd
	pop	edi
	pop	esi
	ret

