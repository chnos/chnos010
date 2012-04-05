
/*TaskControlMessage宣言*/

//タスクに標準付属のFIFOに送信される、システムからの通知。
//0x80000000から始まる。=31ビット目がTrueかつタスクのFIFOであれば、TCMである。
//タスク実行リンク中にないタスクには送信されないが、スリープ中のタスクには送信される。

#define	TCM_INFO_DISPLAY_UPDATE_RESOLUTION	0x80000001
