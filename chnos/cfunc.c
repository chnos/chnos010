//����(uchar s[], const uchar format[], uint n, ...)
//	s	:���ʂ��������ޕ�����̐擪�A�h���X���w�肵�܂��B
//	format	:�����w�蕶����̐擪�A�h���X���w�肵�܂��B�I�[��0x00�ł���K�v������܂��B
//	n	:s[]�̑傫�����w�肵�܂��B(n - 1)�Ԗڈȍ~�̕����͏������܂�܂���B
//		:�������A�[�����w�肵���ꍇ�́A�����o�͂��܂���B���̏ꍇ�As[]��0(null)���w�肷�邱�Ƃ��ł��܂��B
//	...	:�ϒ������ł��B
//
//������t�H�[�}�b�g���w�肳�ꂽ�ꍇ�A���̕����̃t�H�[�}�b�g���߂��I�����A
//���߂ł��Ȃ�������������ʏ핶���Ƃ��ďo�͂��܂�
//
//%[�t���O][�ŏ��t�B�[���h��][���x][�ϊ��C���q]�t�H�[�}�b�g�w��q"
//	text	:������%�������A���̂܂܏o�͂���܂��B
//	�g�p�\�ȃt�H�[�}�b�g�w��q:
//	%%	:%�������o�͂��܂��B
//	o	:�f�[�^��8�i���ŏo�͂��܂��B
//	d, i	:�f�[�^��10�i���ŏo�͂��܂��B
//	x, X	:�f�[�^��16�i���ŏo�͂��܂��Bx:�A���t�@�x�b�g�������BX:�A���t�@�x�b�g�啶���B
//	u	:�f�[�^�𕄍��Ȃ�10�i���ŏo�͂��܂��B
//	c	:�f�[�^��ASCII�R�[�h�Ɖ��߂��Ĉꕶ�����o�͂��܂��B
//	s	:�f�[�^�𕶎���ւ̃|�C���^�Ɖ��߂��ĕ�������o�͂��܂��B������̏I�[��0(null)�ł���K�v������܂��B
//	f	:�f�[�^�𕂓������_���Ƃ��ďo�͂��܂��Bfloat/double�̋�ʂ͂��܂���B�W���̐��x��6���ł��B
//	e, E	:�f�[�^���w���`���ŏo�͂��܂��Be:�A���t�@�x�b�g�������BE:�A���t�@�x�b�g�啶���B�W���̐��x��6���ł��B
//	g, G	:�ʏ��f�t�H�[�}�b�g�w��q�Ɠ�����������܂����A
//		:�w������-5�ȉ����A�L�����x�ȏ�̂Ƃ���e,E�t�H�[�}�b�g�w��q�Ɠ�����������܂��B
//		:���̏����̖ړI�́A�Ȃ�ׂ��Z���\���ŏo�͂��邱�Ƃł��B
//	p	:�f�[�^�����炩�̃|�C���^�Ɖ��߂��āA���̎w�������A�h���X���o�͂��܂��B
//	n	:���̃t�H�[�}�b�g�w��q���܂ރt�H�[�}�b�g�w��ɒB����܂ŁA����o�͂������������A
//		:�f�[�^��uint *�Ƃ��ĉ��߂��A�|�C���^���w���������uint�^�ϐ��ɑ�����܂��B

#include "core.h"

int snprintf(uchar s[], const uchar format[], uint n, ...)
{
	return CFunction_vsnprintf(s, format, n, (uint *)(&n + 1));
}

int vsnprintf(uchar s[], const uchar format[], uint n, uint vargs[])
{
	return CFunction_vsnprintf(s, format, n, vargs);
}

int CFunction_vsnprintf(uchar s[], const uchar format[], uint n, uint vargs[])
{
	uchar c;
	uint i;
	const uchar *d;

	CFunction_vsnprintf_WorkArea work;

	CFunction_vsnprintf_Initialise_WorkArea(&work, s, format, n, vargs);

	for(;;){
		if(CFunction_vsnprintf_Check_FormatBuffer(&work) == -1){
			break;
		}
		c = CFunction_vsnprintf_Read_FormatBuffer(&work);
		if(work.format_phase > 0){	/*�����w�蒆*/
			if(c == '%'){		/*%�������o�͂��܂��B*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, '%');
				work.format_phase = 0;
			} else if(c == 'o'){	/*�f�[�^��8�i���ŏo�͂��܂��B*/
			} else if(c == 'd'){	/*�f�[�^��10�i���ŏo�͂��܂��B*/
				i = CFunction_vsnprintf_Get_NextArgument(&work);
				if((i & 0x80000000) != 0){
					i--;
					i = ~i;
					CFunction_vsnprintf_Write_DestinationBuffer(&work, '-');
				}
				CFunction_vsnprintf_To_String_From_Decimal_Unsigned(&work, i);
				for(i = 0; i < 10; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
					if(i >= 10 - 1){
						CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
					}
				}
				for(; i < 10; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'i'){	/*�f�[�^��10�i���ŏo�͂��܂��B*/
				i = CFunction_vsnprintf_Get_NextArgument(&work);
				if((i & 0x80000000) != 0){
					i--;
					i = ~i;
					CFunction_vsnprintf_Write_DestinationBuffer(&work, '-');
				}
				CFunction_vsnprintf_To_String_From_Decimal_Unsigned(&work, i);
				for(i = 0; i < 10; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
					if(i >= 10 - 1){
						CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
					}
				}
				for(; i < 10; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'x'){	/*�f�[�^��16�i���ŏo�͂��܂��Bx:�A���t�@�x�b�g�������B*/
				CFunction_vsnprintf_To_String_From_Hex_Lower(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
					if(i >= 8 - 1){
						CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
					}
				}
				for(; i < 8; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
			} else if(c == 'X'){	/*�f�[�^��16�i���ŏo�͂��܂��BX:�A���t�@�x�b�g�啶���B*/
				CFunction_vsnprintf_To_String_From_Hex_Upper(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
					if(i >= 8 - 1){
						CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
					}
				}
				for(; i < 8; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'u'){	/*�f�[�^�𕄍��Ȃ�10�i���ŏo�͂��܂��B*/
				CFunction_vsnprintf_To_String_From_Decimal_Unsigned(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				for(i = 0; i < 10; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
					if(i >= 10 - 1){
						CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
					}
				}
				for(; i < 10; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'c'){	/*�f�[�^��ASCII�R�[�h�Ɖ��߂��Ĉꕶ�����o�͂��܂��B*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				work.format_phase = 0;
			} else if(c == 's'){	/*�f�[�^�𕶎���ւ̃|�C���^�Ɖ��߂��ĕ�������o�͂��܂��B������̏I�[��0(null)�ł���K�v������܂��B*/
				d = (const uchar *)CFunction_vsnprintf_Get_NextArgument(&work);
				for (; *d != 0x00; d++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, *d);
				}
				work.format_phase = 0;
			} else if(c == 'f'){	/*�f�[�^�𕂓������_���Ƃ��ďo�͂��܂��Bfloat/double�̋�ʂ͂��܂���B�W���̐��x��6���ł��B�ϒ������̏ꍇ�A���������_���͏��double�Ɋg������邩��ł��B*/
				((uint *)work.temporary_data_double)[0] = CFunction_vsnprintf_Get_NextArgument(&work);
				((uint *)work.temporary_data_double)[1] = CFunction_vsnprintf_Get_NextArgument(&work);

				CFunction_vsnprintf_To_String_From_Hex_Upper(&work, ((uint *)work.temporary_data_double)[1]);
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 8; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}

				CFunction_vsnprintf_To_String_From_Hex_Upper(&work, ((uint *)work.temporary_data_double)[0]);
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 8; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'e'){	/*�f�[�^���w���`���ŏo�͂��܂��Be:�A���t�@�x�b�g�������B�W���̐��x��6���ł��B*/
				
			} else if(c == 'E'){	/*�f�[�^���w���`���ŏo�͂��܂��BE:�A���t�@�x�b�g�啶���B�W���̐��x��6���ł��B*/
			} else if(c == 'g'){	/*�ʏ��f�t�H�[�}�b�g�w��q�Ɠ�����������܂����A�w������-5�ȉ����A�L�����x�ȏ�̂Ƃ���*/
			} else if(c == 'G'){	/*e,E�t�H�[�}�b�g�w��q�Ɠ�����������܂��B���̏����̖ړI�́A�Ȃ�ׂ��Z���\���ŏo�͂��邱�Ƃł��B*/
			} else if(c == 'p'){	/*�f�[�^�����炩�̃|�C���^�Ɖ��߂��āA���̎w�������A�h���X���o�͂��܂��B*/
			} else if(c == 'n'){	/*���̃t�H�[�}�b�g�w��q���܂ރt�H�[�}�b�g�w��ɒB����܂ŁA����o�͂������������A�f�[�^��uint *�Ƃ��ĉ��߂��A�|�C���^���w���������uint�^�ϐ��ɑ�����܂��B*/
			} else if(0x30 <= c && c <= 0x39){	/*����*/
			} else{
				CFunction_vsnprintf_Write_DestinationBuffer(&work, c);
				work.format_phase = 0;
			}
		} else{	/*��ʕ�������*/
			if(c == '%'){	/*������͏����w��*/
				work.format_phase = 1;
			} else{	/*��ʕ����o�͒�*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, c);
			}
		}
	}

	CFunction_vsnprintf_End(&work);

	return work.index_destination_buf;
}

void CFunction_vsnprintf_Initialise_WorkArea(CFunction_vsnprintf_WorkArea *work, uchar s[], const uchar format[], uint n, uint vargs[])
{
	work->destination_buf = s;
	work->length_destination_buf = n;
	work->index_destination_buf = 0;

	work->format_buf = format;
	work->index_format_buf = 0;

	work->temporary_data[15] = 0x00;

	work->vargs = vargs;
	work->index_vargs = 0;

	work->format_phase = 0;

	return;
}

int CFunction_vsnprintf_Check_FormatBuffer(CFunction_vsnprintf_WorkArea *work)
{
	if(work->format_buf[work->index_format_buf] == 0x00){
		return -1;
	}
	return 0;
}

int CFunction_vsnprintf_Check_DestinationBuffer(CFunction_vsnprintf_WorkArea *work)
{
	if(work->index_destination_buf >= work->length_destination_buf){
		return -1;
	}
	return 0;
}

uchar CFunction_vsnprintf_Read_FormatBuffer(CFunction_vsnprintf_WorkArea *work)
{
	if(CFunction_vsnprintf_Check_FormatBuffer(work) == -1){
		return 0;
	}
	work->index_format_buf++;
	return work->format_buf[work->index_format_buf - 1];
}

void CFunction_vsnprintf_Write_DestinationBuffer(CFunction_vsnprintf_WorkArea *work, uchar c)
{
	if(CFunction_vsnprintf_Check_DestinationBuffer(work) != -1){
		if(work->destination_buf != 0){
			work->destination_buf[work->index_destination_buf] = c;
		}
	}
	work->index_destination_buf++;
	return;
}

void CFunction_vsnprintf_End(CFunction_vsnprintf_WorkArea *work)
{
	if(work->destination_buf != 0){
		work->destination_buf[work->index_destination_buf] = 0x00;
	}
	return;
}

uint CFunction_vsnprintf_Get_NextArgument(CFunction_vsnprintf_WorkArea *work)
{
	work->index_vargs++;
	return work->vargs[work->index_vargs - 1];
}

void CFunction_vsnprintf_To_String_From_Hex_Upper(CFunction_vsnprintf_WorkArea *work, uint hex)
{
/*�e���|�����f�[�^�ɁA�E�l�߁A8���Œ�A�󔒏[�U�A�I�[null�A�啶��*/
	uint i;

	for(i = 0; i < 8; i++){	/*�l�̕��z*/
		work->temporary_data[7 - i] = (hex >> (i << 2)) & 0x0000000f;
	}
	work->temporary_data[8] = 0x00;	/*�I�[null*/
	for(i = 0; i < 8; i++){	/*�󔒏[�U*/
		if(work->temporary_data[i] != 0x00){
			break;
		}
		work->temporary_data[i] = ' ';
	}
	for(; i < 8; i++){	/*ASCII�R�[�h�֕ϊ��i�啶���j*/
		if(work->temporary_data[i] > 9){	/*�A���t�@�x�b�g�啶���֕ϊ�*/
			work->temporary_data[i] += 55;
		} else{	/*�����֕ϊ�*/
			work->temporary_data[i] += 48;
		}
	}
	return;
}

void CFunction_vsnprintf_To_String_From_Hex_Lower(CFunction_vsnprintf_WorkArea *work, uint hex)
{
/*�e���|�����f�[�^�ɁA�E�l�߁A8���Œ�A�󔒏[�U�A�I�[null�A������*/
	uint i;

	for(i = 0; i < 8; i++){	/*�l�̕��z*/
		work->temporary_data[7 - i] = (hex >> (i << 2)) & 0x0000000f;
	}
	work->temporary_data[8] = 0x00;	/*�I�[null*/
	for(i = 0; i < 8; i++){	/*�󔒏[�U*/
		if(work->temporary_data[i] != 0x00){
			break;
		}
		work->temporary_data[i] = ' ';
	}
	for(; i < 8; i++){	/*ASCII�R�[�h�֕ϊ��i�������j*/
		if(work->temporary_data[i] > 9){	/*�A���t�@�x�b�g�������֕ϊ�*/
			work->temporary_data[i] += 87;
		} else{	/*�����֕ϊ�*/
			work->temporary_data[i] += 48;
		}
	}
	return;
}

void CFunction_vsnprintf_To_String_From_Decimal_Unsigned(CFunction_vsnprintf_WorkArea *work, uint d)
{
/*�e���|�����f�[�^�ɁA�E�l�߁A10���Œ�A�󔒏[�U�A�I�[null*/
	uint i;

	for(i = 0; i < 10; i++){	/*�l�̕��z*/
		work->temporary_data[9 - i] = d % 10;
		d = d / 10;
	}
	work->temporary_data[10] = 0x00;	/*�I�[null*/
	for(i = 0; i < 10; i++){	/*�󔒏[�U*/
		if(work->temporary_data[i] != 0x00){
			break;
		}
		work->temporary_data[i] = ' ';
	}
	for(; i < 10; i++){	/*ASCII�R�[�h�֕ϊ�*/
		work->temporary_data[i] += 48;
	}
	return;
}



