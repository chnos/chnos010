
[INSTRSET "i486p"]
BOTPAK  equ             0x00280000
DSKCAC  equ             0x00100000
DSKCAC0 equ             0x00008000

; BOOT_INFO_ADDR
CYLS    equ             0x0ff0	; uchar
LEDS    equ             0x0ff1	; uchar
VMODE   equ             0x0ff2	; uchar
				; uchar Padding
SCRNX   equ             0x0ff4	; ushort
SCRNY   equ             0x0ff6	; ushort
VRAM    equ             0x0ff8	; uchar*
VESAVER	equ		0x0ffc	; ushort
APMVER	equ		0x0ffe	; ushort
ACPI_MEMMAPS	equ	0x1000	; uint
ACPI_MEMMAP	equ	0x1004	; 0x18(24)*16=0x180(384)bytes
APM_FLAGS	equ	0x1184	; ushort

; INFO_ADDR
ADR_VESA_BIOS_INFO	equ	0x0d00	;0x0d00-0x0dff
ADR_VESA_MODE_INFO	equ	0x0e00	;0x0e00-0x0eff

XRESOLUTION	equ	0x12
YRESOLUTION	equ	0x14
BITSPERPIXEL	equ	0x19
PHYSBASEPTR	equ	0x28

[BITS 16]

        org     0xc200
asmhead:
	mov	ax, 0
	mov	ss, ax
	mov	sp, 0xc200
	mov	ds, ax
	mov	es, ax

	mov	ah, 0x00
	mov	al, 0x03
	int	0x10

	lea	di, [msg000]
	call	text_putstr

	call	a20_try_loop

	call	chk_acpi_memmap

	call	chk_apm

	call	chk_keyled

        call    pmode

halt_loop:
	hlt
	jmp	halt_loop

; サブルーチン

chk_acpi_memmap:
	pushad
	mov	edi, ACPI_MEMMAP
	mov	ebx, 0
chk_acpi_memmap_loop:
	mov	eax, 0xe820
	mov	ecx, 24
	mov	edx, 0x534d4150	; edx="SMAP"
	int	0x15
	jc	chk_acpi_memmap_err
	add	edi, 24
	or	ebx, ebx
	jnz	chk_acpi_memmap_loop
	sub	edi, ACPI_MEMMAP
	mov	eax, edi
	xor	edx, edx
	mov	ecx, 24
	div	ecx
	mov	[ACPI_MEMMAPS], eax
	lea	di, [msg017]
	call	text_putstr
	call	text_newline
	jmp	chk_acpi_memmap_end

chk_acpi_memmap_err:
	lea	di, [msg016]
	call	text_putstr
	call	text_newline
	mov	dword [ACPI_MEMMAPS], 0

chk_acpi_memmap_end:
	popad
	ret

chk_keyled:
	mov	ah, 0x02
	int	0x16
	mov	[LEDS], al
	ret

A20_TEST_LOOPS		equ	32
A20_ENABLE_LOOPS	equ	255
A20_TEST_ADDR		equ	4*0x80


a20_try_loop:
a20_none:
	call	a20_test
	jnz	a20_done
a20_bios:
	mov	ax, 0x2401
	pushfd
	int	0x15
	popfd
	call	a20_test
	jnz	a20_done
a20_kbc:
	call	empty_8042
	call	a20_test
	jnz	a20_done
	mov	al, 0xD1
	out	0x64, al
	call	empty_8042
	mov	al, 0xDF
	out	0x60, al
	call	empty_8042
a20_kbc_wait:
	xor	cx, cx
a20_kbc_wait_loop:
	call	a20_test
	jnz	a20_done
	loop	a20_kbc_wait_loop
a20_fast:
	in	al, 0x92
	or	al, 0x02

	and	al, 0xFE
	out	0x92, al
a20_fast_wait:
	xor	cx, cx
a20_fast_wait_loop:
	call	a20_test
	jnz	a20_done
	loop	a20_fast_wait_loop
	dec	byte [a20_tries]
	jnz	a20_try_loop
a20_die:
	lea	di, [msg014]
	call	text_putstr
	jmp	halt_loop

a20_tries:
	db	A20_ENABLE_LOOPS

a20_done:
	lea	di, [msg015]
	call	text_putstr
	ret

a20_test:
	push	cx
	push	ax
	xor	cx, cx
	mov	fs, cx
	dec	cx
	mov	gs, cx
	mov	cx, A20_TEST_LOOPS
	mov	ax, word [fs:A20_TEST_ADDR]
	push	ax
a20_test_wait:
	inc	ax
	mov	word [fs:A20_TEST_ADDR], ax
	call	delay
	cmp	ax, word [gs:A20_TEST_ADDR + 0x10]
	loop	a20_test_wait

	pop	word [fs:A20_TEST_ADDR]
	pop	ax
	pop	cx
	ret

empty_8042:
	push	ecx
	mov	ecx, 100000

empty_8042_loop:
	dec	ecx
	jz	empty_8042_end_loop

	call	delay

	in	al, 0x64
	test	al, 1			
	jz	no_output

	call	delay
;	in	al, 0x60

	jmp	empty_8042_loop

no_output:
	test	al, 2
	jnz	empty_8042_loop

empty_8042_end_loop:
	pop	ecx
	ret

delay:
	out	0x80, al
	ret

chk_apm:
	pusha
	lea	di, [msg006]
	call	text_putstr
	lea	di, [msg002]
	call	text_putstr
	mov	ax, 0x5300
	mov	bx, 0x0000
	pusha
	int	0x15
	jc	chk_apm_err_nosup
	mov	[APMVER], ax
	mov	[APM_FLAGS], cx
	popa

	lea	di, [msg004]
	call	text_putstr
	mov	ax, [APMVER]
	shr	ax, 8
	call	text_puthex_char
	mov	al, '.'
	mov	ah, 0x0e
	int	0x10
	mov	ax, [APMVER]
	shr	ax, 4
	call	text_puthex_char
	mov	ax, [APMVER]
	call	text_puthex_char
	jmp	chk_apm_end

chk_apm_err_nosup:
	mov	word [APMVER], 0x0000
	mov	word [APM_FLAGS], 0x0000
	popa
	lea	di, [msg006]
	call	text_putstr
	lea	di, [msg003]
	call	text_putstr
chk_apm_end:
	popa
	ret

hex2bcd_16:	;axをBCDに変換。最大9999まで。cxに新axを保存
	push	cx
	push	bx
	push	dx
	and	eax, 0x0000ffff
	mov	cx, 0x000000

	mov	dx, 0x0000
	mov	bx, 1000
	div	bx
	shl	ax, 12
	or	cx, ax

	mov	ax, dx
	mov	dx, 0x0000
	mov	bx, 100
	div	bx
	shl	ax, 8
	or	cx, ax

	mov	ax, dx
	mov	dx, 0x0000
	mov	bx, 10
	div	bx
	shl	ax, 4
	or	cx, ax

	or	cx, dx

	mov	ax, cx

	pop	dx
	pop	bx
	pop	cx
	ret

text_puthex_str_16:	; axを出力。0xの付加あり。
	push	ax
	push	cx
	mov	cx, ax
	mov	ah, 0x0e
	mov	al, '0'
	int	0x10
	mov	al, 'x'
	int	0x10
	mov	ax, cx
	shr	ax, 12
	call	text_puthex_char
	mov	ax, cx
	shr	ax, 8
	call	text_puthex_char
	mov	ax, cx
	shr	ax, 4
	call	text_puthex_char
	mov	ax, cx
	call	text_puthex_char
	pop	cx
	pop	ax
	ret

text_puthex_str_16_no_0x:	; axを出力。0xの付加なし。BCD表示用
	push	ax
	push	cx
	mov	cx, ax
	shr	ax, 12
	call	text_puthex_char
	mov	ax, cx
	shr	ax, 8
	call	text_puthex_char
	mov	ax, cx
	shr	ax, 4
	call	text_puthex_char
	mov	ax, cx
	call	text_puthex_char
	pop	cx
	pop	ax
	ret

text_puthex_str_08_no_0x:	; alを出力。0xの付加なし。BCD表示用
	push	ax
	push	cx
	mov	cx, ax
	shr	al, 4
	call	text_puthex_char
	mov	ax, cx
	call	text_puthex_char
	pop	cx
	pop	ax
	ret

text_puthex_char:	; alの下位4bit分出力。0xの付加はなし。
	pusha
	and	al, 0x0f
	cmp	al, 9
	ja	text_puthex_char_alphabet
	add	al, 0x30
	jmp	text_puthex_char_end
text_puthex_char_alphabet:
	add	al, 0x37
text_puthex_char_end:
	mov	ah, 0x0e
	int	0x10
	popa
	ret

text_putstr:	; di=char*
	pusha
	mov	cx, 0
	mov	ah, 0x0e
text_putstr_loop:
	mov	al, [es:di]
	cmp	al, 0
	je	text_putstr_end
	inc	di
	int	0x10
	jmp	text_putstr_loop
text_putstr_end:
	popa
	ret

text_newline:
	pusha
	mov	ah, 0x0e
	mov	al, 0x0d
	int	0x10
	mov	al, 0x0a
	int	0x10
	popa
	ret

; 以下、プロテクトモード移行、bootpack実行関数群

pmode:
	mov	al, 0xff
	out	0x21, al	; pic0-imr = 11111111
	nop
	out	0xa1, al	; pic1-imr = 11111111
	cli

	lgdt	[GDTR0]

	mov	eax, cr0
	and	eax, 0x7fffffff	; PG = 0
	or	eax, 0x00000001	; PE = 1
	mov	cr0, eax

	jmp	pipelineflush
pipelineflush:

	mov	ax, 1 * 8
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	esi, bootpack
	mov	edi, BOTPAK
	mov	ecx, 512 * 1024 / 4
	call	memcpy

	mov	esi, 0x7c00
	mov	edi, DSKCAC
	mov	ecx, 512 / 4
	call	memcpy

	mov	esi, DSKCAC0 + 512
	mov	edi, DSKCAC + 512
	mov	ecx, 0
	mov	cl, byte[CYLS]
	imul	ecx, 512 * 18 * 2 / 4
	sub	ecx, 512 / 4
	call	memcpy

	mov	ebx, BOTPAK
	mov	ecx, [ebx + 16]
	add	ecx, 3
	shr	ecx, 2
	jz	skip
	mov	esi, [ebx + 20]
	add	esi, ebx
	mov	edi, [ebx + 12]
	call	memcpy

skip:
	mov	esp, [ebx + 12]
	jmp	dword 2 * 8:0x0000001b

memcpy:
        mov     eax,[esi]
        add     esi,4
        mov     [edi],eax
        add     edi,4
        sub     ecx,1
        jnz     memcpy
        ret

; データ

msg000:	db	"Welcome to CHNOS Project.", 0x0d, 0x0a, 0x00
msg001:	db	"VESA BIOS Extention ", 0x00
msg002:	db	"Checking...", 0x0d, 0x0a, 0x00
msg003:	db	"is not supported by this computer.", 0x0d, 0x0a, 0x00
msg004:	db	"Version:", 0x00
msg005:	db	"Video Mode Numbers...", 0x0d, 0x0a, 0x00
msg006:	db	"Advanced Power Management BIOS ", 0x00
msg007:	db	"Please select the Video Mode Number.", 0x0d, 0x0a, 0x00
msg008:	db	"VGA mode is 0x0000.", 0x0d, 0x0a, 0x00
msg009:	db	">0x", 0x00
msg010:	db	"ModeInfo:", 0x00
msg011:	db	"Function Error...", 0x0d, 0x0a, 0x00
msg012:	db	"bpp:", 0x00
msg013:	db	"Do you want to start in this screen mode?[Y/N]", 0x0d, 0x0a, ">", 0x00
msg014:	db	"A20GateLine Failed.", 0x0d, 0x0a, 0x00
msg015:	db	"A20GateLine Passed.", 0x0d, 0x0a, 0x00
msg016:	db	"ACPI 0xe820 Failed.", 0x0d, 0x0a, 0x00
msg017:	db	"ACPI 0xe820 Passed.", 0x0d, 0x0a, 0x00

GDTR0:
	dw	8 * 3 - 1	; GDTリミット = 8 * セレクタ数 - 1
	dd	GDT0		; GDT開始アドレス

	alignb	16

GDT0:	; 仮GDT
	resb	8		; ヌル・セレクター
	dw	0xffff, 0x0000, 0x9200, 0x00cf	; 読み書き可能32ビット
	dw	0xffff, 0x0000, 0x9a28, 0x0047	; 実行可能32ビット
	dw	0

	alignb	16

bootpack:
