
/*TaskControlMessage�錾*/

//�^�X�N�ɕW���t����FIFO�ɑ��M�����A�V�X�e������̒ʒm�B
//0x80000000����n�܂�B=31�r�b�g�ڂ�True���^�X�N��FIFO�ł���΁ATCM�ł���B
//�^�X�N���s�����N���ɂȂ��^�X�N�ɂ͑��M����Ȃ����A�X���[�v���̃^�X�N�ɂ͑��M�����B

#define TCM_OFFSET							0x80000000
#define TCM_NULL							0x80000000
#define TCM_INFO_DISPLAY_UPDATE_RESOLUTION	0x00000001

//���̑��A���͂Ɏg����input�o�b�t�@�ł́A
//INPUTSIGNAL_OFFSET����SIGNAL_KEY_OFFSET-1�́A�e����̓`�B�Ɏg�����߂ɃV�X�e���ŗ\�񂳂�Ă���B
#define INPUTSIGNAL_OFFSET		0x08000

#define INPUTSIGNAL_NULL		0x00000
#define INPUTSIGNAL_FOCUS_GOT	0x00001
#define INPUTSIGNAL_FOCUS_LOST	0x00002
#define INPUTSIGNAL_MAX			0x07fff

//�܂��ASIGNAL_KEY_OFFSET����SIGNAL_KEY_OFFSET+0xffff�́Akeyid�̒ʒm�Ɏg����B
//�ڍׂ́Akeyid.h���Q�ƁB
#define SIGNAL_KEY_OFFSET 0x10000

