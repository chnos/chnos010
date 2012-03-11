
/*�V�X�e���f�[�^�^�錾*/

/*new object types*/
//typedef enum _bool { false, true} bool;
typedef unsigned char bool;
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
		unsigned bit1 : 1;	//���True�A����ȊO�̗\��ς݃r�b�g�͏��False
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
	struct FIFO32_FLAGS {
		unsigned initialized : 1;
		unsigned overflow : 1;
	} flags;
	struct UI_TASK *task;
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
	CPU_TaskStateSegment retvalue;
	uint esp0;
} IO_CallBIOSControl;

/*cfunction*/
typedef struct CFUNCTION_VSNPRINTF_WORKAREA {
	uchar *destination_buf;		/*�������ݐ敶����̊J�n�A�h���X*/
	uint length_destination_buf;	/*�������ݐ敶����̍ő�T�C�Y*/
	uint index_destination_buf;	/*�������ݐ敶����̃C���f�b�N�X*/

	const uchar *format_buf;	/*�����w�蕶����̊J�n�A�h���X*/
	uint index_format_buf;		/*�����w�蕶����̃C���f�b�N�X*/

	uchar temporary_data[16];	/*�ꎞ�f�[�^�̔z��*/
	uchar temporary_data_double[8];	/*64�r�b�g���������_�p�o�b�t�@*/

	uint *vargs;			/*�ϒ������̊J�n�A�h���X*/
	uint index_vargs;		/*�ϒ������̌��݂̏ꏊ*/

	uint format_phase;		/*�t�H�[�}�b�g�̒i�K�������B*/
						/*���l	:�Ӗ�*/
						/*0	:�t�H�[�}�b�g�w�蒆�ł͂Ȃ�*/
						/*1	:�t���O�ȍ~�̏�����v��*/
						/*2	:�t�B�[���h���ȍ~�̏�����v���B*/
						/*3	:���x�ȍ~�̏�����v���B*/
						/*4	:�ϊ��C���q�ȍ~�̏�����v���B*/
						/*5	:�t�H�[�}�b�g�w��q��v��*/
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

typedef struct DATA_LOCATION_2DU {
	uint x;
	uint y;
} DATA_Location2DU;

/*display*/
typedef struct _INFO_VBE_BIOS {
	uchar sign[4];
	uchar ver_minor;
	uchar ver_major;
	ushort oem_string_offset;
	ushort oem_string_segment;
	ushort flags[2];	//�p�f�B���O�̊֌W�ŁA�{����uint����ushort*2�ɂ��Ă���B
	ushort vmode_args_offset;
	ushort vmode_args_segment;
	ushort vram_supported_size_kb;
} INFO_VBE_BIOS;

typedef struct _INFO_VBE_VIDEO_MODE {
	ushort	ModeAttributes;
	uchar	WinAAttributes;
	uchar	WinBAttributes;
	ushort	WinGranularity;
	ushort	WinSize;
	ushort	WinASegment;
	ushort	WinBSegment;
	uint	WinFuncPtr;
	ushort	BytesPerScanLine;
	ushort	XResolution;
	ushort	YResolution;
	uchar	XCharSize;
	uchar	YCharSize;
	uchar	NumberOfPlanes;
	uchar	BitsPerPixel;
	uchar	NumberOfBanks;
	uchar	MemoryModel;
	uchar	BankSize;
	uchar	NumberOfImagePages;
	uchar	Reserved;
	uchar	RedMaskSize;
	uchar	RedFieldPosition;
	uchar	GreenMaskSize;
	uchar	GreenFieldPosition;
	uchar	BlueMaskSize;
	uchar	BlueFieldPosition;
	uchar	RsvdMaskSize;
	uchar	RsvdFieldPodition;
	uchar	DirectColorModeInfo;
	void*	PhysBasePtr;
} INFO_VBE_VideoMode;

typedef struct _INFO_VBE_VIDEOMODETAG {
	ushort mode_number;
	ushort xsize;
	ushort ysize;
	uchar bpp;
	uchar memory_model;	//0x00:text 0x04:256palette 0x06:direct color
	ushort attribute;	//bit3:1:color 0:monochrome
				//bit4:1:graphic 0:text
				//bit7:linear buffer support
	ushort reserve;
	void *vram;	//vram:0=not supported.
} INFO_VBE_VideoModeTag;

typedef struct IO_DISPLAY_CONTROL {
	struct IO_DISPLAY_CONTROL_VBE {
		uchar version_minor;
		uchar version_major;
		ushort vram_supported_size_kb;
		uint flags;
		uchar *oem_string;
		ushort *vmode_args;
		uint list_vmode_tags;
		INFO_VBE_VideoModeTag *list_vmode;
	} VBE;
	ushort display_mode;	//0x0000:BIOS 0x0001:VBE Window Access(version < 2.0)(not implemented) 0x0002:VBE Linear Access
	ushort vmode;	//0x0000:VGA 320x200 8bit
	ushort vmode_index;
	ushort bpp;
	ushort xsize;
	ushort ysize;
	void *vram;
	DATA_FIFO32 *bios_signal;
} IO_DisplayControl;

/*sheet*/
typedef struct UI_SHEET {
	struct UI_SHEET *parent;	//one sheet has one parent(if vramseet then:0)
	struct UI_SHEET *next;		//same level sheet link
	struct UI_SHEET *child;		//children lowest height
	DATA_Location2D location;
	DATA_Location2DU size;
	ushort bpp;	//0:Text(16col) 1:8bit 2:16bit 3:32bit
	union UI_SHEET_FLAGS {
		ushort flags;
		struct UI_SHEET_FLAGS_BITS {
			unsigned initialized : 1;
			unsigned buffer_configured : 1;
			unsigned visible : 1;
			unsigned using_map : 1;
			unsigned vram_auto_allocated : 1;
		} bit;
	} flags;
	void *vram;
	uint *map;
	uint vramsize;
	uint mapsize;
	uint (*Config_Functions)(struct UI_SHEET *sheet);
	uint (*RefreshSheet)(struct UI_SHEET *sheet, int px0, int py0, int px1, int py1);
} UI_Sheet;

/*timer*/
typedef struct UI_TIMER {
	uint tick;
	uint timeout;
	struct UI_TIMER *root_next;
	struct UI_TIMER *tree_next;
	DATA_FIFO32 *fifo;
	uint fifo_putdata;
	union UI_TIMER_FLAGS {
		uint flags;
		struct UI_TIMER_FLAGS_BITS {
			unsigned initialized : 1;
			unsigned configured : 1;
			unsigned running : 1;
			unsigned interval : 1;
		} bit;
	} flags;
} UI_Timer;

typedef struct UI_TIMER_CONTROL {
	uint tick_10ms;
	void (*TaskSwitch)(void);
	UI_Timer *timer_root;
} UI_TimerControl;

/*color*/

typedef union RGB_32BIT {
	uint c32;
	struct RGB_32BIT_RGBA {
		uchar b;
		uchar g;
		uchar r;
		uchar a;
	} bit;
} RGB32;
