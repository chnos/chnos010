
[INSTRSET "i486p"]

; BOOT_INFO_ADDR
CYLS    equ             0x0ff0

	org		0x7c00			; ���̃v���O�������ǂ��ɓǂݍ��܂��̂�

; �ȉ��͕W���I��fat12�t�H�[�}�b�g�t���b�s�[�f�B�X�N�̂��߂̋L�q

	jmp		entry
	db		0x90
	db		"chnos   "		; �u�[�g�Z�N�^�̖��O�����R�ɏ����Ă悢�i8�o�C�g�j
	dw		512			; 1�Z�N�^�̑傫���i512�ɂ��Ȃ���΂����Ȃ��j
	db		1			; �N���X�^�̑傫���i1�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
	dw		1			; fat���ǂ�����n�܂邩�i���ʂ�1�Z�N�^�ڂ���ɂ���j
	db		2			; fat�̌��i2�ɂ��Ȃ���΂����Ȃ��j
	dw		224			; ���[�g�f�B���N�g���̈�̑傫���i���ʂ�224�G���g���ɂ���j
	dw		2880			; ���̃h���C�u�̑傫���i2880�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
	db		0xf0			; ���f�B�A�̃^�C�v�i0xf0�ɂ��Ȃ���΂����Ȃ��j
	dw		9			; fat�̈�̒����i9�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
	dw		18			; 1�g���b�N�ɂ����̃Z�N�^�����邩�i18�ɂ��Ȃ���΂����Ȃ��j
	dw		2			; �w�b�h�̐��i2�ɂ��Ȃ���΂����Ȃ��j
	dd		0			; �p�[�e�B�V�������g���ĂȂ��̂ł����͕K��0
	dd		2880			; ���̃h���C�u�傫����������x����
	db		0, 0, 0x29		; �悭�킩��Ȃ����ǂ��̒l�ɂ��Ă����Ƃ����炵��
	dd		0xffffffff		; ���Ԃ�{�����[���V���A���ԍ�
	db		"chnosboot  "		; �f�B�X�N�̖��O�i11�o�C�g�j
	db		"fat12   "		; �t�H�[�}�b�g�̖��O�i8�o�C�g�j
	resb	18				; �Ƃ肠����18�o�C�g�����Ă���

; �v���O�����{��

entry:
	mov	ax, 0			; ���W�X�^������
	mov	ss, ax
	mov	sp, 0x7c00
	mov	ds, ax

; �f�B�X�N��ǂ�

        mov     ax, 0x0820
        mov     es, ax
        mov     ch, 0            ; �V�����_0
        mov     dh, 0            ; �w�b�h0
        mov     cl, 2            ; �Z�N�^2
        mov     bx, 18*2-1       ; �ǂݍ��݂������v�Z�N�^�� (2006.12.23 �o�O�C��)
        call    readfast        ; �����ǂݍ���

        mov     bx, 4312+0x8200+2
fatloop:
        add     bx, -2
        cmp     word [bx], 0
        jz      fatloop
        mov     ax, bx
        add     ax, -0x8200-5+54+53
        mov     bl, 54
        div     bl              ; al = ax / bl

        cmp     al, 0
        jnz     fatskip1
        mov     al, 1
fatskip1:
        cmp     al, 33
        jbe     fatskip2
        mov     al, 33
fatskip2:
        mov     [CYLS], al     ; ipl���ǂ��܂œǂ񂾂̂�������
        add     al, -1
        jz      fatskip3
        mov     ah, 0
        imul    bx, ax, 18*2

        mov     ax, 0x0c80
        mov     es, ax
        mov     ch, 1            ; �V�����_1
        mov     dh, 0            ; �w�b�h0
        mov     cl, 1            ; �Z�N�^1
        call    readfast        ; �����ǂݍ���
fatskip3:

; �ǂݏI������̂�haribote.sys�����s���I

        jmp     0xc200


error:
	mov		ax, 0
	mov		es, ax
	mov		si, msg
putloop:
	mov		al, [si]
	add		si, 1			; si��1�𑫂�
	cmp		al, 0
	je		fin
	mov		ah, 0x0e			; �ꕶ���\���t�@���N�V����
	mov		bx, 15			; �J���[�R�[�h
	int		0x10			; �r�f�Ibios�Ăяo��
	jmp		putloop
fin:
	hlt					; ��������܂�cpu���~������
	jmp		fin			; �������[�v

msg:	db		0x0a, 0x0a		; ���s��2��
	db		"chnipl load error"
	db		0x0a			; ���s
	db		0

readfast:	; al���g���Ăł��邾���܂Ƃ߂ēǂݏo��
		; es:�ǂݍ��ݔԒn, ch:�V�����_, dh:�w�b�h, cl:�Z�N�^, bx:�ǂݍ��݃Z�N�^��

	mov		ax, es			; < es����al�̍ő�l���v�Z >
	shl		ax, 3			; ax��32�Ŋ����āA���̌��ʂ�ah�ɓ��ꂽ���ƂɂȂ� �ishl�͍��V�t�g���߁j
	and		ah, 0x7f			; ah��ah��128�Ŋ������]��i512*128=64k�j
	mov		al, 128			; al = 128 - ah; ��ԋ߂�64kb���E�܂ōő剽�Z�N�^���邩
	sub		al, ah

	mov		ah, bl			; < bx����al�̍ő�l��ah�Ɍv�Z >
	cmp		bh, 0			; if (bh != 0) { ah = 18; }
	je		.skip1
	mov		ah, 18
.skip1:
	cmp		al, ah			; if (al > ah) { al = ah; }
	jbe		.skip2
	mov		al, ah
.skip2:

	mov		ah, 19			; < cl����al�̍ő�l��ah�Ɍv�Z >
	sub		ah, cl			; ah = 19 - cl;
	cmp		al, ah			; if (al > ah) { al = ah; }
	jbe		.skip3
	mov		al, ah
.skip3:

	push	bx
	mov		si, 0			; ���s�񐔂𐔂��郌�W�X�^
retry:
	mov		ah, 0x02			; ah=0x02 : �f�B�X�N�ǂݍ���
	mov		bx, 0
	mov		dl, 0x00			; a�h���C�u
	push	es
	push	dx
	push	cx
	push	ax
	int		0x13			; �f�B�X�Nbios�Ăяo��
	jnc		next			; �G���[�������Ȃ����next��
	add		si, 1			; si��1�𑫂�
	cmp		si, 5			; si��5���r
	jae		error			; si >= 5 ��������error��
	mov		ah, 0x00
	mov		dl, 0x00			; a�h���C�u
	int		0x13			; �h���C�u�̃��Z�b�g
	pop		ax
	pop		cx
	pop		dx
	pop		es
	jmp		retry
next:
	pop		ax
	pop		cx
	pop		dx
	pop		bx			; es�̓��e��bx�Ŏ󂯎��
	shr		bx, 5			; bx��16�o�C�g�P�ʂ���512�o�C�g�P�ʂ�
	mov		ah, 0
	add		bx, ax			; bx += al;
	shl		bx, 5			; bx��512�o�C�g�P�ʂ���16�o�C�g�P�ʂ�
	mov		es, bx			; ����� es += al * 0x20; �ɂȂ�
	pop		bx
	sub		bx, ax
	jz		.ret
	add		cl, al			; cl��al�𑫂�
	cmp		cl, 18			; cl��18���r
	jbe		readfast		; cl <= 18 ��������readfast��
	mov		cl, 1
	add		dh, 1
	cmp		dh, 2
	jb		readfast		; dh < 2 ��������readfast��
	mov		dh, 0
	add		ch, 1
	jmp		readfast
.ret:
	ret

	resb	0x7dfe-$		; 0x7dfe�܂ł�0x00�Ŗ��߂閽��
	db		0x55, 0xaa
