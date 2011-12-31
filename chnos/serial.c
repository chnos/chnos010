
#include "core.h"

void Initialise_SerialPort(void)
{
	IO_Out8(COM1_CTRL_LINE, 0x80);	//ボーレート設定開始
	IO_Out8(COM1_BAUD_LSB, 0x06);	//0x06 = 19.2bps
	IO_Out8(COM1_CTRL_LINE, 0x03);	//ボーレート設定終了、送受信データ8bit
	IO_Out8(COM1_CTRL_MODEM, 0x0b);	//割り込み有効、RTS、DTRピンをアクティブ化
//	IO_Out8(COM1_INTR_ENBL, 0x04);	//ラインステータス割り込み
	IO_Out8(COM1_INTR_ENBL, 0x00);	//割り込みなし

	return;
}

void SerialPort_Send(const uchar s[])
{
	for (; *s != 0x00; s++){
		if(*s == '\n'){
			IO_Out8(COM1_TX, '\r');
			for(; (IO_In8(COM1_STA_LINE) & 0x40) == 0; ){

			}
		}
		IO_Out8(COM1_TX, *s);
		for(; (IO_In8(COM1_STA_LINE) & 0x40) == 0; ){

		}
	}

	return;
}

#ifdef CHNOSPROJECT_DEBUG
void debug(const uchar format[], ...)
{
	uchar s[256];

	vsnprintf(s, sizeof(s), format, (uint *)(&format + 1));
	SerialPort_Send(s);
	return;
}
#endif
