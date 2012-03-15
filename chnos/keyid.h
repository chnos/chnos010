
/*KeyIdentifier キーID宣言*/

//16bitを使用。つまり、0-65535,0x0000-0xffff
//bit00-06	:ASCIIコード(拡張フラグが0のとき)
//		:拡張キーID(拡張フラグが1のとき)		
//bit07		:Make,Break種別(0:Make,1:Break)(0:種別なし)
//bit08		:テンキー種別(0:フルキー,1:テンキー)(0:種別なし)
//bit09		:Shiftキー状態(0:オフ,1:オン)
//bit10		:Ctrlキー状態(0:オフ,1:オン)
//bit11		:Altキー状態(0:オフ,1:オン)
//bit12		:ScrollLock状態(0:オフ,1:オン)
//bit13		:NumLock状態(0:オフ,1:オン)
//bit14		:CapsLock状態(0:オフ,1:オン)
//bit15		:拡張フラグ(0:ASCII,1:拡張キーコード)

//つまり、FIFOで受け取って、バイアスを引いたら、KEYID_EXTENDEDフラグを調べる。
//次に、BREAKフラグを調べる。
//そして、下位7ビットのみを参照する。

#define KEYID_MASK_ID			0x007f
#define KEYID_MASK_BREAK		0x0080
#define KEYID_MASK_TENKEY		0x0100
#define KEYID_MASK_STATE_SHIFT		0x0200
#define KEYID_MASK_STATE_CTRL		0x0400
#define KEYID_MASK_STATE_ALT		0x0800
#define KEYID_MASK_STATE_LOCK_SCROOL	0x1000
#define KEYID_MASK_STATE_LOCK_NUM	0x2000
#define KEYID_MASK_STATE_LOCK_CAPS	0x4000
#define KEYID_MASK_EXTENDED		0x8000

#define KEYID_ASCII_NUL		0x0000
#define KEYID_ASCII_SOH		0x0001
#define KEYID_ASCII_STX		0x0002
#define KEYID_ASCII_ETX		0x0003
#define KEYID_ASCII_EOT		0x0004
#define KEYID_ASCII_ENQ		0x0005
#define KEYID_ASCII_ACK		0x0006
#define KEYID_ASCII_BEL		0x0007
#define KEYID_ASCII_BS		0x0008	/*0x0e*/
#define KEYID_ASCII_HT		0x0009	/*0x0f*/
#define KEYID_ASCII_LF		0x000a	/*0x1c*/
#define KEYID_ASCII_VT		0x000b
#define KEYID_ASCII_FF		0x000c
#define KEYID_ASCII_CR		0x000d
#define KEYID_ASCII_SO		0x000e
#define KEYID_ASCII_SI		0x000f
#define KEYID_ASCII_DLE		0x0010
#define KEYID_ASCII_DC1		0x0011
#define KEYID_ASCII_DC2		0x0012
#define KEYID_ASCII_DC3		0x0013
#define KEYID_ASCII_DC4		0x0014
#define KEYID_ASCII_NAK		0x0015
#define KEYID_ASCII_SYN		0x0016
#define KEYID_ASCII_ETB		0x0017
#define KEYID_ASCII_CAN		0x0018
#define KEYID_ASCII_EM		0x0019
#define KEYID_ASCII_SUB		0x001a
#define KEYID_ASCII_ESC		0x001b
#define KEYID_ASCII_FS		0x001c
#define KEYID_ASCII_GS		0x001d
#define KEYID_ASCII_RS		0x001e
#define KEYID_ASCII_US		0x001f
#define KEYID_ASCII_CHAR_START	0x0020
#define KEYID_ASCII_CHAR_END	0x007e
#define KEYID_ASCII_DEL		0x007f

#define KEYID_ESC		0x0000
#define KEYID_F1		0x0001
#define KEYID_F2		0x0002
#define KEYID_F3		0x0003
#define KEYID_F4		0x0004
#define KEYID_F5		0x0005
#define KEYID_F6		0x0006
#define KEYID_F7		0x0007
#define KEYID_F8		0x0008
#define KEYID_F9		0x0009
#define KEYID_F10		0x000a
#define KEYID_F11		0x000b
#define KEYID_F12		0x000c
#define KEYID_LOCK_NUM		0x000d
#define KEYID_LOCK_SCROOL	0x000e
#define KEYID_LOCK_CAPS		0x000f
#define KEYID_SHIFT_L		0x0010
#define KEYID_SHIFT_R		0x0011
#define KEYID_CTRL_L		0x0012
#define KEYID_CTRL_R		0x0013
#define KEYID_ALT_L		0x0014
#define KEYID_ALT_R		0x0015
#define KEYID_DELETE		0x0016
#define KEYID_INSERT		0x0017
#define KEYID_PAUSE		0x0018
#define KEYID_BREAK		0x0019
#define KEYID_PRINT_SCREEN	0x001a
#define KEYID_SYS_RQ		0x001b
#define KEYID_CURSOR_U		0x001c
#define KEYID_CURSOR_D		0x001d
#define KEYID_CURSOR_L		0x001e
#define KEYID_CURSOR_R		0x001f
#define KEYID_PAGE_UP		0x0020
#define KEYID_PAGE_DOWN		0x0021
#define KEYID_HOME		0x0022
#define KEYID_END		0x0023
#define KEYID_ICON_L		0x0024
#define KEYID_ICON_R		0x0025
#define KEYID_MENU		0x0026
#define KEYID_KANJI		0x0027
#define KEYID_HIRAGANA		0x0028
#define KEYID_HENKAN		0x0029
#define KEYID_MUHENKAN		0x002a

#define KEYID_BACKSPACE		0x0040
#define KEYID_TAB		0x0041
#define KEYID_ENTER		0x0042

#define KEYID_KBD_ERROR		0x007e
#define KEYID_UNKNOWN		0x007f

