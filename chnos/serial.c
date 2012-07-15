
#include "core.h"

void Initialize_SerialPort(void)
{
	IO_Out8(COM1_CTRL_LINE, 0x80);	//�{�[���[�g�ݒ�J�n
	IO_Out8(COM1_BAUD_LSB, 0x06);	//0x06 = 19.2bps
	IO_Out8(COM1_CTRL_LINE, 0x03);	//�{�[���[�g�ݒ�I���A����M�f�[�^8bit
	IO_Out8(COM1_CTRL_MODEM, 0x0b);	//���荞�ݗL���ARTS�ADTR�s�����A�N�e�B�u��
//	IO_Out8(COM1_INTR_ENBL, 0x04);	//���C���X�e�[�^�X���荞��
	IO_Out8(COM1_INTR_ENBL, 0x00);	//���荞�݂Ȃ�

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
