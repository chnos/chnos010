
/*TaskControlMessage宣言*/

//タスクに標準付属のFIFOに送信される、システムからの通知。
//0x80000000から始まる。=31ビット目がTrueかつタスクのFIFOであれば、TCMである。
//タスク実行リンク中にないタスクには送信されないが、スリープ中のタスクには送信される。

#define TCM_OFFSET							0x80000000
#define TCM_NULL							0x80000000
#define TCM_INFO_DISPLAY_UPDATE_RESOLUTION	0x00000001

//その他、入力に使われるinputバッファでは、
//INPUTSIGNAL_OFFSETからSIGNAL_KEY_OFFSET-1は、各種情報の伝達に使うためにシステムで予約されている。
#define INPUTSIGNAL_OFFSET		0x08000

#define INPUTSIGNAL_NULL		0x00000
#define INPUTSIGNAL_FOCUS_GOT	0x00001
#define INPUTSIGNAL_FOCUS_LOST	0x00002
#define INPUTSIGNAL_MAX			0x07fff

//また、SIGNAL_KEY_OFFSETからSIGNAL_KEY_OFFSET+0xffffは、keyidの通知に使われる。
//詳細は、keyid.hを参照。
#define SIGNAL_KEY_OFFSET 0x10000

