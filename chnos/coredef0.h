
/*�V�X�e���萔�錾*/

/*debug option*/
#define CHNOSPROJECT_DEBUG		/*��`����ƃf�o�b�O���[�h�Ŏ��s�B���ꂼ��̃f�o�b�O�I�v�V�������L���ɂ���K�v������*/

#ifdef CHNOSPROJECT_DEBUG
	//#define CHNOSPROJECT_DEBUG_MEMORY	/*��`����ƃ������֘A�̃f�o�b�O���I���ɂ���*/
	#define CHNOSPROJECT_DEBUG_EMULATOR_X86
	#define CHNOSPROJECT_DEBUG_CALLBIOS
#endif

/*defines*/
/*boolean*/
#define True	1
#define False	0

/*descriptors*/
#define AR_DATA32_RW	0x4092	//AR_32+AR_PRESENT+AR_CODE_OR_DATA+AR_TYPE_DATA_RW
#define AR_CODE32_ER	0x409a	//AR_32+AR_PRESENT+AR_CODE_OR_DATA+AR_TYPE_CODE_ER
#define AR_LDT		0x0082	//AR_PRESENT+AR_SYSTEM_DESCRIPTOR+AR_TYPE_LDT
#define AR_TSS32	0x0089	//AR_PRESENT+AR_SYSTEM_DESCRIPTOR+AR_TYPE_TSS32_READY
#define AR_INTGATE32	0x008e	//AR_PRESENT+AR_SYSTEM_DESCRIPTOR+AR_TYPE_INTGATE32

#define AR_GRANULARITY_BYTE	0x0000	//G=0	�֐��Ŏ����ݒ肷��̂ł��̃r�b�g�͏��0
#define AR_GRANULARITY_4KB	0x8000	//G=1
#define AR_16			0x0000	//D/B=0
#define AR_32			0x4000	//D/B=1
#define AR_NOTPRESENT		0x0000	//P=0
#define AR_PRESENT		0x0080	//P=1
#define AR_SYSTEM		0x0000	//DPL=0
#define AR_USER			0x0060	//DPL=3
#define	AR_SYSTEM_DESCRIPTOR	0x0000	//S=0
#define AR_CODE_OR_DATA		0x0010	//S=1

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

#define AR_TYPE_ACCESSED	0x0001	//�֐��ݒ莞�ɂ̓N���A�A�ǂݏo�����̓A�N�Z�X�ς݂Ȃ�Z�b�g

#define AR_TYPE_DATA_R			0x0000
#define AR_TYPE_DATA_RW			0x0002
#define AR_TYPE_DATA_R_EXPAND_DOWN	0x0004
#define AR_TYPE_DATA_RW_EXPAND_DOWN	0x0006

#define AR_TYPE_CODE_E			0x0008
#define AR_TYPE_CODE_ER			0x000a
#define AR_TYPE_CODE_E_CONFORMING	0x000c
#define AR_TYPE_CODE_ER_CONFORMING	0x000e

/*CPU Opcodes*/
#define OPCODE_REG_EAX			0
#define OPCODE_REG_ECX			1
#define OPCODE_REG_EDX			2
#define OPCODE_REG_EBX			3
#define OPCODE_REG_ESP			4
#define OPCODE_REG_EBP			5
#define OPCODE_REG_ESI			6
#define OPCODE_REG_EDI			7

#define OPCODE_W_BYTE			0
#define OPCODE_W_16BIT_OR_32BIT		1

#define OPCODE_REG_BYTE_AL		0
#define OPCODE_REG_BYTE_CL		1
#define OPCODE_REG_BYTE_DL		2
#define OPCODE_REG_BYTE_BL		3
#define OPCODE_REG_BYTE_AH		4
#define OPCODE_REG_BYTE_CH		5
#define OPCODE_REG_BYTE_DH		6
#define OPCODE_REG_BYTE_BH		7

#define OPCODE_SREG2_ES			0
#define OPCODE_SREG2_CS			1
#define OPCODE_SREG2_SS			2
#define OPCODE_SREG2_DS			3

#define OPCODE_SREG3_ES			0
#define OPCODE_SREG3_CS			1
#define OPCODE_SREG3_SS			2
#define OPCODE_SREG3_DS			3
#define OPCODE_SREG3_FS			4
#define OPCODE_SREG3_GS			5

#define OPCODE_EEE_CR0			0
#define OPCODE_EEE_CR2			2
#define OPCODE_EEE_CR3			3
#define OPCODE_EEE_CR4			4

#define OPCODE_EEE_DR0			0
#define OPCODE_EEE_DR1			1
#define OPCODE_EEE_DR2			2
#define OPCODE_EEE_DR3			3
#define OPCODE_EEE_DR6			6
#define OPCODE_EEE_DR7			7

#define OPCODE_TTTN_OVERFLOW		0
#define OPCODE_TTTN_NO_OVERFLOW		1
#define OPCODE_TTTN_BELOW		2
#define OPCODE_TTTN_NOT_BELOW		3
#define OPCODE_TTTN_EQUAL_OR_ZERO	4
#define OPCODE_TTTN_NOT_EQUAL_OR_ZERO	5
#define OPCODE_TTTN_NOT_ABOVE		6
#define OPCODE_TTTN_ABOVE		7
#define OPCODE_TTTN_SIGN		8
#define OPCODE_TTTN_NOT_SIGN		9
#define OPCODE_TTTN_PARITY		10
#define OPCODE_TTTN_NOT_PARITY		10
#define OPCODE_TTTN_LESS_THAN		10
#define OPCODE_TTTN_NOT_LESS_THAN	10
#define OPCODE_TTTN_NOT_GREATER_THAN	10
#define OPCODE_TTTN_GREATER_THAN	10

#define OPCODE_D_REG_SOURCE		0
#define OPCODE_D_REG_DESTINATION	1

#define OPCODE_MOD_INDEXONLY		0
#define OPCODE_MOD_INDEX_AND_DISP_BYTE	1
#define OPCODE_MOD_INDEX_AND_DISP_FULL	2
#define OPCODE_MOD_REGISTER		3

#define OPCODE_RM32_ADDR_EAX		0
#define OPCODE_RM32_ADDR_ECX		1
#define OPCODE_RM32_ADDR_EDX		2
#define OPCODE_RM32_ADDR_EBX		3
#define OPCODE_RM32_ADDR_SIB		4
#define OPCODE_RM32_ADDR_EBP		5
#define OPCODE_RM32_MOD00_ADDR_DISP32	5
#define OPCODE_RM32_ADDR_ESI		6
#define OPCODE_RM32_ADDR_EDI		7

#define OPCODE_RM16_ADDR_BX_SI		0
#define OPCODE_RM16_ADDR_BX_DI		1
#define OPCODE_RM16_ADDR_BP_SI		2
#define OPCODE_RM16_ADDR_BP_DI		3
#define OPCODE_RM16_ADDR_SI		4
#define OPCODE_RM16_ADDR_DI		5
#define OPCODE_RM16_ADDR_BP		6
#define OPCODE_RM16_MOD00_ADDR_DISP16	6
#define OPCODE_RM16_ADDR_BX		7

#define OPCODE_PREFIX_NONE		0

#define OPCODE_PREFIX_LOCK		0xf0
#define OPCODE_PREFIX_REPNE_REPNZ	0xf2
#define OPCODE_PREFIX_REP_REPE_REPZ	0xf3

//#define OPCODE_PREFIX_CS		0x2e
//#define OPCODE_PREFIX_SS		0x36
//#define OPCODE_PREFIX_DS		0x3e
//#define OPCODE_PREFIX_ES		0x26
//#define OPCODE_PREFIX_FS		0x64
//#define OPCODE_PREFIX_GS		0x65
#define OPCODE_PREFIX_BRANCH_NOT_TAKEN	0x2e
#define OPCODE_PREFIX_BRANCH_TAKEN	0x3e

#define OPCODE_PREFIX_OPERAND_SIZE	0x66

#define OPCODE_PREFIX_ADDRESS_SIZE	0x67

/*PIC io port*/
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

/*KBD io port*/
#define KEYB_DATA		0x0060
#define PORT_KEYSTA		0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE		0x47
#define PORT_KEYCMD		0x0064
#define KEYCMD_SENDTO_MOUSE	0xd4
#define KEYCMD_LED		0xed

/*COM1 io port*/
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

/*PIT io port*/
#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

/*VGA*/
#define VGA_CRTC_R_NUMBER		0x03d4
#define VGA_CRTC_R_DATA			0x03d5
#define VGA_CRTC_R_CURSOR_LOCATION_HIGH	0x0e
#define VGA_CRTC_R_CURSOR_LOCATION_LOW	0x0f
#define VGA_TEXTMODE_ADR		0xb8000
#define VGA_VDAC_DATA			0x03c9
#define VGA_VDAC_WRITE_ADR		0x03c8
#define VGA_VRAM_ADR			(void *)0x000a0000
#define VGA08_VRAM_XSIZE		320
#define VGA08_VRAM_YSIZE		200

/*settings*/
/*system segments*/
#define SYSTEM_DS	0x01
#define SYSTEM_CS	0x02

/*descriptor tables*/
#define ADR_IDT		0x0026f800
#define LIMIT_IDT	0x000007ff
#define ADR_GDT		0x00270000
#define LIMIT_GDT	0x0000ffff
#define ADR_BOOTPACK	0x00280000
#define LIMIT_BOOTPACK	0x0007ffff

/*error id*/
#define ERROR_CPU_EXCEPTION_00			0x00000000	//int *esp
#define ERROR_CPU_EXCEPTION_01			0x00000001	//int *esp
#define ERROR_CPU_EXCEPTION_02			0x00000002	//int *esp
#define ERROR_CPU_EXCEPTION_03			0x00000003	//int *esp
#define ERROR_CPU_EXCEPTION_04			0x00000004	//int *esp
#define ERROR_CPU_EXCEPTION_05			0x00000005	//int *esp
#define ERROR_CPU_EXCEPTION_06			0x00000006	//int *esp
#define ERROR_CPU_EXCEPTION_07			0x00000007	//int *esp
#define ERROR_CPU_EXCEPTION_08			0x00000008	//int *esp
#define ERROR_CPU_EXCEPTION_09			0x00000009	//int *esp
#define ERROR_CPU_EXCEPTION_0A			0x0000000a	//int *esp
#define ERROR_CPU_EXCEPTION_0B			0x0000000b	//int *esp
#define ERROR_CPU_EXCEPTION_0C			0x0000000c	//int *esp
#define ERROR_CPU_EXCEPTION_0D			0x0000000d	//int *esp
#define ERROR_CPU_EXCEPTION_0E			0x0000000e	//int *esp
#define ERROR_CPU_EXCEPTION_0F			0x0000000f	//int *esp
#define ERROR_CPU_EXCEPTION_10			0x00000010	//int *esp
#define ERROR_CPU_EXCEPTION_11			0x00000011	//int *esp
#define ERROR_CPU_EXCEPTION_12			0x00000012	//int *esp
#define ERROR_CPU_EXCEPTION_13			0x00000013	//int *esp
#define ERROR_CPU_EXCEPTION_14			0x00000014	//int *esp
#define ERROR_CPU_EXCEPTION_15			0x00000015	//int *esp
#define ERROR_CPU_EXCEPTION_16			0x00000016	//int *esp
#define ERROR_CPU_EXCEPTION_17			0x00000017	//int *esp
#define ERROR_CPU_EXCEPTION_18			0x00000018	//int *esp
#define ERROR_CPU_EXCEPTION_19			0x00000019	//int *esp
#define ERROR_CPU_EXCEPTION_1A			0x0000001a	//int *esp
#define ERROR_CPU_EXCEPTION_1B			0x0000001b	//int *esp
#define ERROR_CPU_EXCEPTION_1C			0x0000001c	//int *esp
#define ERROR_CPU_EXCEPTION_1D			0x0000001d	//int *esp
#define ERROR_CPU_EXCEPTION_1E			0x0000001e	//int *esp
#define ERROR_CPU_EXCEPTION_1F			0x0000001f	//int *esp
#define ERROR_CPU_EXCEPTIONS			0x0000001f

#define ERROR_NO_MORE_SEGMENT			0x00000020	//uint *retaddr
#define ERROR_NOT_ENOUGH_FREE_MEMORY		0x00000021	//IO_MemoryControl ctrl, uint size
#define ERROR_MEMORY_FREE_RANGE_OVERLAPPED	0x00000022	//IO_MemoryControl ctrl, uint tagno
#define ERROR_NO_MORE_FREE_TAG			0x00000023	//IO_MemoryControl ctrl
#define ERROR_INVALID_FREE_MEMORY_INDEX		0x00000024	//IO_MemoryControl ctrl, uint tagno

/*FIFO buffer*/
#define SIGNAL_ARGUMENTS_END	0xfefe1234

/*task*/
#define TASK_FIFOSIZE	128;

/*exceptions esp[] data with errorcode*/
#define EXCEPTION_INFO_EDI		0x00
#define EXCEPTION_INFO_ESI		0x01
#define EXCEPTION_INFO_EBP		0x02
#define EXCEPTION_INFO_ESP		0x03
#define EXCEPTION_INFO_EBX		0x04
#define EXCEPTION_INFO_EDX		0x05
#define EXCEPTION_INFO_ECX		0x06
#define EXCEPTION_INFO_EAX		0x07
#define EXCEPTION_INFO_DS		0x08
#define EXCEPTION_INFO_ES		0x09
#define EXCEPTION_INFO_ERRORCODE	0x0a
#define EXCEPTION_INFO_EIP		0x0b
#define EXCEPTION_INFO_CS		0x0c
#define EXCEPTION_INFO_EFLAGS		0x0d
#define EXCEPTION_INFO_USER_ESP		0x0e
#define EXCEPTION_INFO_USER_SS		0x0f

