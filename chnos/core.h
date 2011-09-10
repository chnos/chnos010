
/*include files*/

/*definemacros*/

/*settings*/

/*new object types*/
typedef enum _bool { false, true} bool;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef enum _col_text { black, blue, green, skyblue, red, purple, brown, white} col_text;

/*structures*/
typedef struct SEGMENT_DESCRIPTOR { 
	ushort limit_low, base_low;
	uchar base_mid, access_right;
	uchar limit_high, base_high;
} IO_SegmentDescriptor;

typedef struct GATE_DESCRIPTOR { 
	ushort offset_low, selector;
	uchar dw_count, access_right;
	ushort offset_high;
} IO_GateDescriptor;

/*externs*/
extern uint SystemPhase;

/*functions*/
/*bootpack.c 基幹部分*/
void CHNOSProject_Set_SystemPhase(uint phase);

/*cfunc.c vsnprintfの独自実装等*/
typedef struct CFUNCTION_VSNPRINTF_WORKAREA {
	uchar *destination_buf;		/*書き込み先文字列の開始アドレス*/
	uint length_destination_buf;	/*書き込み先文字列の最大サイズ*/
	uint index_destination_buf;	/*書き込み先文字列のインデックス*/

	const uchar *format_buf;	/*書式指定文字列の開始アドレス*/
	uint index_format_buf;		/*書式指定文字列のインデックス*/

	uchar temporary_data[16];	/*一時データの配列*/
	uchar temporary_data_double[8];	/*64ビット浮動小数点用バッファ*/

	uint *vargs;			/*可変長引数の開始アドレス*/
	uint index_vargs;		/*可変長引数の現在の場所*/

	uint format_phase;		/*フォーマットの段階を示す。*/
						/*数値	:意味*/
						/*0	:フォーマット指定中ではない*/
						/*1	:フラグ以降の書式を要求*/
						/*2	:フィールド幅以降の書式を要求。*/
						/*3	:精度以降の書式を要求。*/
						/*4	:変換修飾子以降の書式を要求。*/
						/*5	:フォーマット指定子を要求*/
} CFunction_vsnprintf_WorkArea;
//
int snprintf(uchar s[], const uchar format[], uint n, ...);
int vsnprintf(uchar s[], const uchar format[], uint n, uint vargs[]);
//
int CFunction_vsnprintf(uchar s[], const uchar format[], uint n, uint vargs[]);
void CFunction_vsnprintf_Initialise_WorkArea(CFunction_vsnprintf_WorkArea *work, uchar s[], const uchar format[], uint n, uint vargs[]);
int CFunction_vsnprintf_Check_FormatBuffer(CFunction_vsnprintf_WorkArea *work);
int CFunction_vsnprintf_Check_DestinationBuffer(CFunction_vsnprintf_WorkArea *work);
uchar CFunction_vsnprintf_Read_FormatBuffer(CFunction_vsnprintf_WorkArea *work);
void CFunction_vsnprintf_Write_DestinationBuffer(CFunction_vsnprintf_WorkArea *work, uchar c);
void CFunction_vsnprintf_End(CFunction_vsnprintf_WorkArea *work);
uint CFunction_vsnprintf_Get_NextArgument(CFunction_vsnprintf_WorkArea *work);
void CFunction_vsnprintf_To_String_From_Hex_Upper(CFunction_vsnprintf_WorkArea *work, uint hex);
void CFunction_vsnprintf_To_String_From_Hex_Lower(CFunction_vsnprintf_WorkArea *work, uint hex);
void CFunction_vsnprintf_To_String_From_Decimal_Unsigned(CFunction_vsnprintf_WorkArea *work, uint d);


/*dsctbl.c セグメント・ゲートディスクリプタ関連*/
#define ADR_IDT		0x0026f800
#define LIMIT_IDT	0x000007ff
#define ADR_GDT		0x00270000
#define LIMIT_GDT	0x0000ffff
#define ADR_BOOTPACK	0x00280000
#define LIMIT_BOOTPACK	0x0007ffff
//
#define AR_DATA32_RW	0x4092	//AR_32+AR_PRESENT+AR_CODE_OR_DATA+AR_TYPE_DATA_RW
#define AR_CODE32_ER	0x409a	//AR_32+AR_PRESENT+AR_CODE_OR_DATA+AR_TYPE_CODE_ER
#define AR_LDT		0x0082	//AR_PRESENT+AR_SYSTEM_DESCRIPTOR+AR_TYPE_LDT
#define AR_TSS32	0x0089	//AR_PRESENT+AR_SYSTEM_DESCRIPTOR+AR_TYPE_TSS32_READY
#define AR_INTGATE32	0x008e	//AR_PRESENT+AR_SYSTEM_DESCRIPTOR+AR_TYPE_INTGATE32
//
#define AR_GRANULARITY_BYTE	0x0000	//G=0	関数で自動設定するのでこのビットは常に0
#define AR_GRANULARITY_4KB	0x8000	//G=1
#define AR_16			0x0000	//D/B=0
#define AR_32			0x4000	//D/B=1
#define AR_NOTPRESENT		0x0000	//P=0
#define AR_PRESENT		0x0080	//P=1
#define AR_SYSTEM		0x0000	//DPL=0
#define AR_USER			0x0060	//DPL=3
#define	AR_SYSTEM_DESCRIPTOR	0x0000	//S=0
#define AR_CODE_OR_DATA		0x0010	//S=1
//S=0
#define AR_TYPE_TSS16_READY	0x0001
#define AR_TYPE_LDT		0x0002
#define AR_TYPE_TSS16_BUSY	0x0003
#define AR_TYPE_CALLGATE16	0x0004
#define AR_TYPE_TASKGATE	0x0005
#define AR_TYPE_INTGATE16	0x0006
#define AR_TYPE_TRAPGATE16	0x0007
#define AR_TYPE_TSS32_READY	0x0009
#define AR_TYPE_TSS32_BUSY	0x000b
#define AR_TYPE_CALLGATE32	0x000c
#define AR_TYPE_INTGATE32	0x000e
#define AR_TYPE_TRAPGATE32	0x000f
//S=1
#define AR_TYPE_ACCESSED	0x0001	//関数設定時にはクリア、読み出し時はアクセス済みならセット
//DATA
#define AR_TYPE_DATA_R			0x0000
#define AR_TYPE_DATA_RW			0x0002
#define AR_TYPE_DATA_R_EXPAND_DOWN	0x0004
#define AR_TYPE_DATA_RW_EXPAND_DOWN	0x0006
//CODE
#define AR_TYPE_CODE_E			0x0008
#define AR_TYPE_CODE_ER			0x000a
#define AR_TYPE_CODE_E_CONFORMING	0x000c
#define AR_TYPE_CODE_ER_CONFORMING	0x000e
void Initialise_GlobalDescriptorTable(void);
void Initialise_InterruptDescriptorTable(void);
void SegmentDescriptor_Set(IO_SegmentDescriptor *seg_desc, uint limit, uint base, uint ar);
uint SegmentDescriptor_Get_Base(IO_SegmentDescriptor *seg_desc);
uint SegmentDescriptor_Get_Limit(IO_SegmentDescriptor *seg_desc);
uint SegmentDescriptor_Get_AccessRight(IO_SegmentDescriptor *seg_desc);
void GateDescriptor_Set(IO_GateDescriptor *gate_desc, uint offset, uint selector, uint ar);
void System_SegmentDescriptor_Set_Absolute(uint selector, uint limit, uint base, uint ar);
uint System_SegmentDescriptor_Get_Base(uint selector);
uint System_SegmentDescriptor_Get_Limit(uint selector);
uint System_SegmentDescriptor_Get_AccessRight(uint selector);
uint System_SegmentDescriptor_Set(uint limit, int base, int ar);
void System_GateDescriptor_Set(uint irq, uint offset, uint selector, uint ar);

/*error.c エラー関連*/
#define ERROR_CPU_EXCEPTION_00		0x00000000
#define ERROR_CPU_EXCEPTION_01		0x00000001
#define ERROR_CPU_EXCEPTION_02		0x00000002
#define ERROR_CPU_EXCEPTION_03		0x00000003
#define ERROR_CPU_EXCEPTION_04		0x00000004
#define ERROR_CPU_EXCEPTION_05		0x00000005
#define ERROR_CPU_EXCEPTION_06		0x00000006
#define ERROR_CPU_EXCEPTION_07		0x00000007
#define ERROR_CPU_EXCEPTION_08		0x00000008
#define ERROR_CPU_EXCEPTION_09		0x00000009
#define ERROR_CPU_EXCEPTION_0A		0x0000000a
#define ERROR_CPU_EXCEPTION_0B		0x0000000b
#define ERROR_CPU_EXCEPTION_0C		0x0000000c
#define ERROR_CPU_EXCEPTION_0D		0x0000000d
#define ERROR_CPU_EXCEPTION_0E		0x0000000e
#define ERROR_CPU_EXCEPTION_0F		0x0000000f
#define ERROR_CPU_EXCEPTION_10		0x00000010
#define ERROR_CPU_EXCEPTION_11		0x00000011
#define ERROR_CPU_EXCEPTION_12		0x00000012
#define ERROR_CPU_EXCEPTION_13		0x00000013
#define ERROR_CPU_EXCEPTION_14		0x00000014
#define ERROR_CPU_EXCEPTION_15		0x00000015
#define ERROR_CPU_EXCEPTION_16		0x00000016
#define ERROR_CPU_EXCEPTION_17		0x00000017
#define ERROR_CPU_EXCEPTION_18		0x00000018
#define ERROR_CPU_EXCEPTION_19		0x00000019
#define ERROR_CPU_EXCEPTION_1A		0x0000001a
#define ERROR_CPU_EXCEPTION_1B		0x0000001b
#define ERROR_CPU_EXCEPTION_1C		0x0000001c
#define ERROR_CPU_EXCEPTION_1D		0x0000001d
#define ERROR_CPU_EXCEPTION_1E		0x0000001e
#define ERROR_CPU_EXCEPTION_1F		0x0000001f
#define ERROR_CPU_EXCEPTIONS		0x0000001f
#define ERROR_NO_MORE_SEGMENT		0x00000020
//
uint Error_Report(uint error_no, ...);
void Error_Abort(void);
void Error_Set_Enable_SerialPort(bool serial);
void Error_Set_Enable_Display_TextMode(bool tdisp);
int Error_Put_String(const uchar format[], ...);
void Error_CPU_Exception_Put_Registers_With_ErrorCode(uint *esp);
void Error_CPU_Exception_Put_Registers_Without_ErrorCode(uint *esp);

/*intrpt.c 割り込み関連*/
#define PIC0_ICW1	0x0020
#define PIC0_OCW2	0x0020
#define PIC0_IMR	0x0021
#define PIC0_ICW2	0x0021
#define PIC0_ICW3	0x0021
#define PIC0_ICW4	0x0021
#define PIC1_ICW1	0x00a0
#define PIC1_OCW2	0x00a0
#define PIC1_IMR	0x00a1
#define PIC1_ICW2	0x00a1
#define PIC1_ICW3	0x00a1
#define PIC1_ICW4	0x00a1
//
void Initialise_ProgrammableInterruptController(void);
void ProgrammableInterruptController_InterruptMask_Clear(uint irq);
void ProgrammableInterruptController_InterruptRequest_Complete(uint irq);
void InterruptHandler27(uint *esp);

/*keyboard.c キーボード関連*/
#define KEYB_DATA		0x0060
#define PORT_KEYSTA		0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE		0x47
#define PORT_KEYCMD		0x0064
#define KEYCMD_SENDTO_MOUSE	0xd4
#define KEYCMD_LED		0xed
//
void Initialise_Keyboard(void);
void InterruptHandler21(uint *esp);

/*serial.c シリアル通信関連*/
#define COM1_RX		0x03f8
#define COM1_TX		0x03f8
#define COM1_BAUD_LSB	0x03f8
#define COM1_BAUD_MSB	0x03f9
#define COM1_INTR_ENBL	0x03f9
#define COM1_INTR_ID	0x03fa
#define COM1_CTRL_FIFO	0x03fa
#define COM1_CTRL_LINE	0x03fb
#define COM1_CTRL_MODEM	0x03fc
#define COM1_STA_LINE	0x03fd
#define COM1_STA_MODEM	0x03fe
//
void Initialise_SerialPort(void);
void SerialPort_Send(const uchar s[]);

/*timer.c タイマー関連*/
#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040
//
void Initialise_ProgrammableIntervalTimer(void);
void InterruptHandler20(uint *esp);

/*vgatmode.c VGAテキストモード関連*/
#define VGA_CRTC_R_NUMBER		0x03d4
#define VGA_CRTC_R_DATA			0x03d5
#define VGA_CRTC_R_CURSOR_LOCATION_HIGH	0x0e
#define VGA_CRTC_R_CURSOR_LOCATION_LOW	0x0f
#define VGA_TEXTMODE_ADR		0xb8000
//
void TextMode_Write_TextRAM(ushort index, uchar data);
void TextMode_Put_Character_Absolute(uchar c, col_text col, ushort location);
void TextMode_Put_String_Absolute(const uchar s[], col_text col, uint x, uint y);
void TextMode_Clear_Screen(void);
ushort TextMode_Get_CursorLocation(void);
void TextMode_Set_CursorLocation(ushort location);
void TextMode_Put_Character(uchar c, col_text col);
void TextMode_Newline(void);
void TextMode_Put_String(const uchar s[], col_text col);

/*xception.c 例外関連*/
void CPU_ExceptionHandler00(int *esp);
void CPU_ExceptionHandler01(int *esp);
void CPU_ExceptionHandler02(int *esp);
void CPU_ExceptionHandler03(int *esp);
void CPU_ExceptionHandler04(int *esp);
void CPU_ExceptionHandler05(int *esp);
void CPU_ExceptionHandler06(int *esp);
void CPU_ExceptionHandler07(int *esp);
void CPU_ExceptionHandler08(int *esp);
void CPU_ExceptionHandler09(int *esp);
void CPU_ExceptionHandler0a(int *esp);
void CPU_ExceptionHandler0b(int *esp);
void CPU_ExceptionHandler0c(int *esp);
void CPU_ExceptionHandler0d(int *esp);
void CPU_ExceptionHandler0e(int *esp);
void CPU_ExceptionHandler0f(int *esp);
void CPU_ExceptionHandler10(int *esp);
void CPU_ExceptionHandler11(int *esp);
void CPU_ExceptionHandler12(int *esp);
void CPU_ExceptionHandler13(int *esp);
void CPU_ExceptionHandler14(int *esp);
void CPU_ExceptionHandler15(int *esp);
void CPU_ExceptionHandler16(int *esp);
void CPU_ExceptionHandler17(int *esp);
void CPU_ExceptionHandler18(int *esp);
void CPU_ExceptionHandler19(int *esp);
void CPU_ExceptionHandler1a(int *esp);
void CPU_ExceptionHandler1b(int *esp);
void CPU_ExceptionHandler1c(int *esp);
void CPU_ExceptionHandler1d(int *esp);
void CPU_ExceptionHandler1e(int *esp);
void CPU_ExceptionHandler1f(int *esp);

/*nasfunc0.nas 他の関数に全く依存しないアセンブラ関数群*/
void IO_HLT(void);			//CPUを停止させる。割り込みがあると再開する。特権命令。
void IO_CLI(void);			//CPUへの外部割り込みを停止させる。特権命令。NMIと例外には効かない。
void IO_STI(void);			//CPUへの外部割り込みを再開させる。特権命令。
void IO_STIHLT(void);			//CPUへの外部割り込みを再開させた後、CPUを停止させる。stiとhltの間に発生する割り込みへの対策。。
uint IO_In8(uint port);			//port番のI/Oポートからデータをバイト単位で受け取る。戻り値は受け取ったデータ。I/O特権命令。
void IO_Out8(uint port, uint data);	//port番のI/Oポートにdataをバイト単位で出力する。I/O特権命令。
uint IO_In16(uint port);		//port番のI/Oポートからデータをワード単位で受け取る。戻り値は受け取ったデータ。I/O特権命令。
void IO_Out16(uint port, uint data);	//port番のI/Oポートにdataをワード単位で出力する。I/O特権命令。
uint IO_In32(uint port);		//port番のI/Oポートからデータをダブルワード単位で受け取る。戻り値は受け取ったデータ。I/O特権命令。
void IO_Out32(uint port, uint data);	//port番のI/Oポートにdataをダブルワード単位で出力する。I/O特権命令。
uint IO_Load_EFlags(void);		//EFLAGSレジスタを読み込む。
void IO_Store_EFlags(uint eflags);	//EFLAGSレジスタをeflagsに変更する。
uint Load_CR0(void);			//コントロールレジスタ0を読み込む。特権命令。
void Store_CR0(uint cr0);		//コントロールレジスタ0をcr0に変更する。特権命令。
uint Load_CR2(void);			//コントロールレジスタ2を読み込む。特権命令。
void Store_CR2(uint cr2);		//コントロールレジスタ2をcr2に変更する。特権命令。
uint Load_CR3(void);			//コントロールレジスタ3を読み込む。特権命令。
void Store_CR3(uint cr3);		//コントロールレジスタ3をcr3に変更する。特権命令。
uint Load_CR4(void);			//コントロールレジスタ4を読み込む。特権命令。
void Store_CR4(uint cr4);		//コントロールレジスタ4をcr4に変更する。特権命令。
void Load_GDTR(uint limit, uint addr);	//グローバル・ディスクリプタ・テーブル・レジスタ(GDTR)に、addrからlimitの範囲をGDTとしてロードさせる。特権命令。
void Load_IDTR(uint limit, uint addr);	//インタラプト・ディスクリプタ・テーブル・レジスタ(IDTR)に、addrからlimitの範囲をIDTとしてロードさせる。特権命令。
void Load_TR(uint tr);			//セグメント・セレクタtrを、タスクレジスタに現在のタスクとしてロードさせる。特権命令。
void FarJMP(uint eip, uint cs);		//現在のセグメントとは異なるセグメントcsのオフセットeipにプログラムの制御を移す。
void FarCall(uint eip, uint cs);	//現在のセグメントとは異なるセグメントcsのオフセットeipにプログラムの制御を分岐させる。
void CLTS(void);			//コントロール・レジスタ0内にある、Task-Switchedフラグをクリアする。FPUの制御に利用。特権命令。
void FNSave(uint *addr);		//FPU動作環境をaddrから始まる108バイトに格納し、FPUを初期化する。詳細なチェックは行わない。
void FRStore(uint *addr);		//FPU動作環境をaddrから始まる108バイトから復元する。
void PIT_Beep_On(void);			//ビープ音を開始する。
void PIT_Beep_Off(void);		//ビープ音を停止する。
void PIT_Beep_Set(uint fq);		//ビープ音の周波数を、fqHzに変更する。
					//fqの値
					//C:262 C#:277 D:294 D#:311 E:330 F:349 F#:370 G:392 G#:415 A:440 A#:466 B:494 C:523
void CPUID(void *addr, uint id);	//addr番地のuint[4]に、CPUの識別情報id番をEAX・EBX・EDX・ECXの順番で格納する。
void CPUID2(void *addr, uint id);	//addr番地のuint[4]に、CPUの識別情報id番をEAX・EBX・ECX・EDXの順番で格納する。
					//上記二つの関数は、EFLAGS内のIDフラグ(ビット21)が変更可能な場合のみ実行できる。
void TSC_Read(uint *addr);		//addr番地のuint[2]に、マシン固有レジスタ(MSR)内にある、タイム・スタンプ・カウンタの上位・下位それぞれ32ビットを読み込む。
					//この関数は、cpuidのTSCビットが有効でなければ使用できない。
uint Memory_Test_Sub(uint start, uint end);
void INT_3(void);			//ブレークポイント例外を発生させる。
uint DIV_64_32(uint dividend_low, uint dividend_high, uint divisor);
					//=((dividend_high << 32) | dividend_low) / divisor
uint MOD_64_32(uint dividend_low, uint dividend_high, uint divisor);
					//=((dividend_high << 32) | dividend_low) % divisor
/*nasfunc0.nas 他の関数に依存するアセンブラ関数群*/
void asm_CPU_ExceptionHandler00(void);
void asm_CPU_ExceptionHandler01(void);
void asm_CPU_ExceptionHandler02(void);
void asm_CPU_ExceptionHandler03(void);
void asm_CPU_ExceptionHandler04(void);
void asm_CPU_ExceptionHandler05(void);
void asm_CPU_ExceptionHandler06(void);
void asm_CPU_ExceptionHandler07(void);
void asm_CPU_ExceptionHandler08(void);
void asm_CPU_ExceptionHandler09(void);
void asm_CPU_ExceptionHandler0a(void);
void asm_CPU_ExceptionHandler0b(void);
void asm_CPU_ExceptionHandler0c(void);
void asm_CPU_ExceptionHandler0d(void);
void asm_CPU_ExceptionHandler0e(void);
void asm_CPU_ExceptionHandler0f(void);
void asm_CPU_ExceptionHandler10(void);
void asm_CPU_ExceptionHandler11(void);
void asm_CPU_ExceptionHandler12(void);
void asm_CPU_ExceptionHandler13(void);
void asm_CPU_ExceptionHandler14(void);
void asm_CPU_ExceptionHandler15(void);
void asm_CPU_ExceptionHandler16(void);
void asm_CPU_ExceptionHandler17(void);
void asm_CPU_ExceptionHandler18(void);
void asm_CPU_ExceptionHandler19(void);
void asm_CPU_ExceptionHandler1a(void);
void asm_CPU_ExceptionHandler1b(void);
void asm_CPU_ExceptionHandler1c(void);
void asm_CPU_ExceptionHandler1d(void);
void asm_CPU_ExceptionHandler1e(void);
void asm_CPU_ExceptionHandler1f(void);
//
void asm_InterruptHandler20(void);
void asm_InterruptHandler21(void);
void asm_InterruptHandler27(void);
