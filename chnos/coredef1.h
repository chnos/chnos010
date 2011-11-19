
/*システムデータ型宣言*/

/*new object types*/
typedef enum _bool { false, true} bool;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef enum _col_text { black, blue, green, skyblue, red, purple, brown, white} col_text;

/*structs*/
/*CPU structs*/
typedef union CPU_CONTROL_REGISTER0 {
	uint cr0;
	struct CPU_CONTROL_REGISTER0_BIT {
		unsigned PE : 1;
		unsigned MP : 1;
		unsigned EM : 1;
		unsigned TS : 1;
		unsigned ET : 1;
		unsigned NE : 1;
		unsigned bit6 : 1;
		unsigned bit7 : 1;
		unsigned bit8 : 1;
		unsigned bit9 : 1;
		unsigned bit10 : 1;
		unsigned bit11 : 1;
		unsigned bit12 : 1;
		unsigned bit13 : 1;
		unsigned bit14 : 1;
		unsigned bit15 : 1;
		unsigned WP : 1;
		unsigned bit17 : 1;
		unsigned AM : 1;
		unsigned bit19 : 1;
		unsigned bit20 : 1;
		unsigned bit21 : 1;
		unsigned bit22 : 1;
		unsigned bit23 : 1;
		unsigned bit24 : 1;
		unsigned bit25 : 1;
		unsigned bit26 : 1;
		unsigned bit27 : 1;
		unsigned bit28 : 1;
		unsigned NW : 1;
		unsigned CD : 1;
		unsigned PG : 1;
	} bit;
} CPU_ControlRegister0;

typedef union CPU_CONTROL_REGISTER3 {
	uint cr3;
	struct CPU_CONTROL_REGISTER3_BIT {
		unsigned bit0 : 1;
		unsigned bit1 : 1;
		unsigned bit2 : 1;
		unsigned PWT : 1;
		unsigned PCD : 1;
		unsigned bit5 : 1;
		unsigned bit6 : 1;
		unsigned bit7 : 1;
		unsigned bit8 : 1;
		unsigned bit9 : 1;
		unsigned bit10 : 1;
		unsigned bit11 : 1;
		unsigned PDB : 20;
	} bit;
} CPU_ControlRegister3;

typedef union CPU_CONTROL_REGISTER4 {
	uint cr4;
	struct CPU_CONTROL_REGISTER4_BIT {
		unsigned VME : 1;
		unsigned PVI : 1;
		unsigned TSD : 1;
		unsigned DE : 1;
		unsigned PSE : 1;
		unsigned PAE : 1;
		unsigned MCE : 1;
		unsigned PGE : 1;
		unsigned PCE : 1;
		unsigned OSFXSR : 1;
		unsigned OSXMMEXCPT : 1;
		unsigned bit11 : 1;
		unsigned bit12 : 1;
		unsigned VMXE : 1;
		unsigned SMXE : 1;
		unsigned bit15 : 1;
		unsigned bit16 : 1;
		unsigned PCIDE : 1;
		unsigned OSXSAVE : 1;
		unsigned bit19 : 1;
		unsigned SMEP : 1;
		unsigned bit21 : 1;
		unsigned bit22 : 1;
		unsigned bit23 : 1;
		unsigned bit24 : 1;
		unsigned bit25 : 1;
		unsigned bit26 : 1;
		unsigned bit27 : 1;
		unsigned bit28 : 1;
		unsigned bit29 : 1;
		unsigned bit30 : 1;
		unsigned bit31 : 1;
	} bit;
} CPU_ControlRegister4;

typedef union CPU_EFLAGS {
	uint eflags;
	struct CPU_EFLAGS_BIT {
		unsigned CF : 1;
		unsigned bit1 : 1;	//常にTrue、これ以外の予約済みビットは常にFalse
		unsigned PF : 1;
		unsigned bit3 : 1;
		unsigned AF : 1;
		unsigned bit5 : 1;
		unsigned ZF : 1;
		unsigned SF : 1;
		unsigned TF : 1;
		unsigned IF : 1;
		unsigned DF : 1;
		unsigned OF : 1;
		unsigned IOPL : 2;
		unsigned NT : 1;
		unsigned bit15 : 1;
		unsigned RF : 1;
		unsigned VM : 1;
		unsigned AC : 1;
		unsigned VIF : 1;
		unsigned VIP : 1;
		unsigned ID : 1;
		unsigned bit22 : 1;
		unsigned bit23 : 1;
		unsigned bit24 : 1;
		unsigned bit25 : 1;
		unsigned bit26 : 1;
		unsigned bit27 : 1;
		unsigned bit28 : 1;
		unsigned bit29 : 1;
		unsigned bit30 : 1;
		unsigned bit31 : 1;
	} bit;
} CPU_EFlags;

typedef struct TASK_STATE_SEGMENT {
	ushort backlink, reserve00;
	uint esp0;
	ushort ss0, reserve01;
	uint esp1;
	ushort ss1, reserve02;
	uint esp2;
	ushort ss2, reserve03;
	uint cr3;
	uint eip;
	CPU_EFlags eflags;
	uint eax;
	uint ecx;
	uint edx;
	uint ebx;
	uint esp;
	uint ebp;
	uint esi;
	uint edi;
	ushort es, reserve04;
	ushort cs, reserve05;
	ushort ss, reserve06;
	ushort ds, reserve07;
	ushort fs, reserve08;
	ushort gs, reserve09;
	ushort ldtr, reserve10;
	unsigned flag_trap : 1;
	unsigned reserve11 : 15;
	ushort iomap_base;
} CPU_TaskStateSegment;

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

/*memory*/
typedef struct IO_MEMORYCONTROLTAG {
	void *addr;
	uint size;
} IO_MemoryControlTag;

typedef IO_MemoryControlTag* IO_MemoryControl;

/*FIFO*/
typedef struct FIFO32 {
	uint *buf;
	uint p, q, size, free;
	union FIFO32_FLAGS {
		uint flags;
		struct FIFO32_FLAGS_BITS {
			unsigned overflow : 1;
		} bit;
	} flags;
} DATA_FIFO32;

/*task*/
typedef struct UI_TASK {
	uint selector;
	struct UI_TASK *next;
	uint count;
	DATA_FIFO32 *fifo;
	CPU_TaskStateSegment *tss;
	struct UI_TASK_STATE_FLAGS {
		unsigned initialized : 1;
		unsigned linked : 1;
	} flags;
} UI_Task;

typedef struct UI_TASK_CONTROL {
	struct UI_TASK *start;
	struct UI_TASK *now;
	IO_MemoryControl sysmemctrl;
} UI_TaskControl;

/*callbios*/
typedef struct CALL_BIOS_CONTROL {
	UI_Task *CallBIOS_Task;
	uint codesize;
	DATA_FIFO32 *fifo;
	uint endsignal;
} IO_CallBIOSControl;

/*cfunction*/
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

/*emu86*/
typedef struct EMULATOR_X86_ENVIRONMENT_SEGMENT_REGISTER {
	ushort selector;
	ushort type;
	uint base;
	uint limit;
} Emulator_x86_Environment_SegmentRegister;

typedef struct EMULATOR_X86_ENVIRONMENT_SYSTEM_TABLE_REGISTER {
	uint base;
	ushort limit;
} Emulator_x86_Environment_SystemTableRegister;

typedef struct EMULATOR_X86_ENVIRONMENT {
	CPU_EFlags EFLAGS;
	uint EIP;

	uint GReg[8];

	Emulator_x86_Environment_SystemTableRegister GDTR;
	Emulator_x86_Environment_SystemTableRegister IDTR;

	Emulator_x86_Environment_SegmentRegister SReg[8];

	CPU_ControlRegister0 CR0;
	uint CR2;
	CPU_ControlRegister3 CR3;
	CPU_ControlRegister4 CR4;

	uchar now_opcode;
	uchar operation_end;
	uchar operation_32bit;
	uchar operation_prefix_lock_repeat;
	uchar operation_prefix_segment;
	uchar operation_prefix_opsize;
	uchar operation_prefix_addrsize;
} Emulator_x86_Environment;

typedef union EMULATOR_X86_OPCODE_MODRM {
	uchar modrm;
	struct EMULATOR_X86_OPCODE_MODRM_BIT {
		unsigned RM : 3;
		unsigned Reg : 3;
		unsigned Mod : 2;
	} bit;
} Emulator_x86_OperationCode_ModRM;

typedef struct EMULATOR_X86_FAR_POINTER {
	ushort offset;
	ushort selector;
} Emulator_x86_FarPointer;

/*drawing*/
typedef struct DATA_LOCATION_2D {
	int x;
	int y;
} DATA_Location2D;
