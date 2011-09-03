
[INSTRSET "i486p"]

; BOOT_INFO_ADDR
CYLS    equ             0x0ff0

	org		0x7c00			; このプログラムがどこに読み込まれるのか

; 以下は標準的なfat12フォーマットフロッピーディスクのための記述

	jmp		entry
	db		0x90
	db		"chnos   "		; ブートセクタの名前を自由に書いてよい（8バイト）
	dw		512			; 1セクタの大きさ（512にしなければいけない）
	db		1			; クラスタの大きさ（1セクタにしなければいけない）
	dw		1			; fatがどこから始まるか（普通は1セクタ目からにする）
	db		2			; fatの個数（2にしなければいけない）
	dw		224			; ルートディレクトリ領域の大きさ（普通は224エントリにする）
	dw		2880			; このドライブの大きさ（2880セクタにしなければいけない）
	db		0xf0			; メディアのタイプ（0xf0にしなければいけない）
	dw		9			; fat領域の長さ（9セクタにしなければいけない）
	dw		18			; 1トラックにいくつのセクタがあるか（18にしなければいけない）
	dw		2			; ヘッドの数（2にしなければいけない）
	dd		0			; パーティションを使ってないのでここは必ず0
	dd		2880			; このドライブ大きさをもう一度書く
	db		0, 0, 0x29		; よくわからないけどこの値にしておくといいらしい
	dd		0xffffffff		; たぶんボリュームシリアル番号
	db		"chnosboot  "		; ディスクの名前（11バイト）
	db		"fat12   "		; フォーマットの名前（8バイト）
	resb	18				; とりあえず18バイトあけておく

; プログラム本体

entry:
	mov	ax, 0			; レジスタ初期化
	mov	ss, ax
	mov	sp, 0x7c00
	mov	ds, ax

; ディスクを読む

        mov     ax, 0x0820
        mov     es, ax
        mov     ch, 0            ; シリンダ0
        mov     dh, 0            ; ヘッド0
        mov     cl, 2            ; セクタ2
        mov     bx, 18*2-1       ; 読み込みたい合計セクタ数 (2006.12.23 バグ修正)
        call    readfast        ; 高速読み込み

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
        mov     [CYLS], al     ; iplがどこまで読んだのかをメモ
        add     al, -1
        jz      fatskip3
        mov     ah, 0
        imul    bx, ax, 18*2

        mov     ax, 0x0c80
        mov     es, ax
        mov     ch, 1            ; シリンダ1
        mov     dh, 0            ; ヘッド0
        mov     cl, 1            ; セクタ1
        call    readfast        ; 高速読み込み
fatskip3:

; 読み終わったのでharibote.sysを実行だ！

        jmp     0xc200


error:
	mov		ax, 0
	mov		es, ax
	mov		si, msg
putloop:
	mov		al, [si]
	add		si, 1			; siに1を足す
	cmp		al, 0
	je		fin
	mov		ah, 0x0e			; 一文字表示ファンクション
	mov		bx, 15			; カラーコード
	int		0x10			; ビデオbios呼び出し
	jmp		putloop
fin:
	hlt					; 何かあるまでcpuを停止させる
	jmp		fin			; 無限ループ

msg:	db		0x0a, 0x0a		; 改行を2つ
	db		"chnipl load error"
	db		0x0a			; 改行
	db		0

readfast:	; alを使ってできるだけまとめて読み出す
		; es:読み込み番地, ch:シリンダ, dh:ヘッド, cl:セクタ, bx:読み込みセクタ数

	mov		ax, es			; < esからalの最大値を計算 >
	shl		ax, 3			; axを32で割って、その結果をahに入れたことになる （shlは左シフト命令）
	and		ah, 0x7f			; ahはahを128で割った余り（512*128=64k）
	mov		al, 128			; al = 128 - ah; 一番近い64kb境界まで最大何セクタ入るか
	sub		al, ah

	mov		ah, bl			; < bxからalの最大値をahに計算 >
	cmp		bh, 0			; if (bh != 0) { ah = 18; }
	je		.skip1
	mov		ah, 18
.skip1:
	cmp		al, ah			; if (al > ah) { al = ah; }
	jbe		.skip2
	mov		al, ah
.skip2:

	mov		ah, 19			; < clからalの最大値をahに計算 >
	sub		ah, cl			; ah = 19 - cl;
	cmp		al, ah			; if (al > ah) { al = ah; }
	jbe		.skip3
	mov		al, ah
.skip3:

	push	bx
	mov		si, 0			; 失敗回数を数えるレジスタ
retry:
	mov		ah, 0x02			; ah=0x02 : ディスク読み込み
	mov		bx, 0
	mov		dl, 0x00			; aドライブ
	push	es
	push	dx
	push	cx
	push	ax
	int		0x13			; ディスクbios呼び出し
	jnc		next			; エラーがおきなければnextへ
	add		si, 1			; siに1を足す
	cmp		si, 5			; siと5を比較
	jae		error			; si >= 5 だったらerrorへ
	mov		ah, 0x00
	mov		dl, 0x00			; aドライブ
	int		0x13			; ドライブのリセット
	pop		ax
	pop		cx
	pop		dx
	pop		es
	jmp		retry
next:
	pop		ax
	pop		cx
	pop		dx
	pop		bx			; esの内容をbxで受け取る
	shr		bx, 5			; bxを16バイト単位から512バイト単位へ
	mov		ah, 0
	add		bx, ax			; bx += al;
	shl		bx, 5			; bxを512バイト単位から16バイト単位へ
	mov		es, bx			; これで es += al * 0x20; になる
	pop		bx
	sub		bx, ax
	jz		.ret
	add		cl, al			; clにalを足す
	cmp		cl, 18			; clと18を比較
	jbe		readfast		; cl <= 18 だったらreadfastへ
	mov		cl, 1
	add		dh, 1
	cmp		dh, 2
	jb		readfast		; dh < 2 だったらreadfastへ
	mov		dh, 0
	add		ch, 1
	jmp		readfast
.ret:
	ret

	resb	0x7dfe-$		; 0x7dfeまでを0x00で埋める命令
	db		0x55, 0xaa
