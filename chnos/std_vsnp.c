//
//snprintf(beta)
//author:hikarupsp
//
//����(unsigned char s[], const unsigned char format[], unsigned int n, ...)
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
//		:�f�[�^��unsigned int *�Ƃ��ĉ��߂��A�|�C���^���w���������unsigned int�^�ϐ��ɑ�����܂��B

typedef struct CHNOSPROJECT_SNPRINTF_WORKAREA {
	unsigned char *destination_buf;		/*�������ݐ敶����̊J�n�A�h���X*/
	unsigned int length_destination_buf;	/*�������ݐ敶����̍ő�T�C�Y*/
	unsigned int index_destination_buf;	/*�������ݐ敶����̃C���f�b�N�X*/

	const unsigned char *format_buf;		/*�����w�蕶����̊J�n�A�h���X*/
	unsigned int index_format_buf;		/*�����w�蕶����̃C���f�b�N�X*/

	unsigned char temporary_data[16];	/*�ꎞ�f�[�^�̔z��*/
	unsigned int temporary_data_double[2];	/*64�r�b�g���������_�p�o�b�t�@*/

	unsigned int *vargs;			/*�ϒ������̊J�n�A�h���X*/
	unsigned int index_vargs;		/*�ϒ������̌��݂̏ꏊ*/

	unsigned int format_phase;		/*�t�H�[�}�b�g�̒i�K�������B*/
						/*���l	:�Ӗ�*/
						/*0	:�t�H�[�}�b�g�w�蒆�ł͂Ȃ�*/
						/*1	:�t���O�ȍ~�̏�����v��*/
						/*2	:�t�B�[���h���ȍ~�̏�����v���B*/
						/*3	:���x�ȍ~�̏�����v���B*/
						/*4	:�ϊ��C���q�ȍ~�̏�����v���B*/
						/*5	:�t�H�[�}�b�g�w��q��v��*/
	unsigned int format_sign;		/*���������鐔�̕�����\���B*/
						/*0:�� 1:��*/
	unsigned int format_exponent;		/*���������_���̎w������*/
	unsigned int format_mantissa;		/*���������_���̉�������*/

} CHNOSProject_snprintf_WorkArea;

int CHNOSProject_vsnprintf(unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[]);
void CHNOSProject_snprintf_Initialise_WorkArea(CHNOSProject_snprintf_WorkArea *work, unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[]);
int CHNOSProject_snprintf_Check_FormatBuffer(CHNOSProject_snprintf_WorkArea *work);
int CHNOSProject_snprintf_Check_DestinationBuffer(CHNOSProject_snprintf_WorkArea *work);
unsigned char CHNOSProject_snprintf_Read_FormatBuffer(CHNOSProject_snprintf_WorkArea *work);
void CHNOSProject_snprintf_Write_DestinationBuffer(CHNOSProject_snprintf_WorkArea *work, unsigned char c);
void CHNOSProject_snprintf_End(CHNOSProject_snprintf_WorkArea *work);
unsigned int CHNOSProject_snprintf_Get_NextArgument(CHNOSProject_snprintf_WorkArea *work);
void CHNOSProject_snprintf_To_String_From_Hex(CHNOSProject_snprintf_WorkArea *work, unsigned int hex);
void CHNOSProject_snprintf_To_String_From_Decimal_Unsigned(CHNOSProject_snprintf_WorkArea *work, unsigned int d);

int CHNOSProject_snprintf(unsigned char s[], const unsigned char format[], unsigned int n, ...)
{
	return CHNOSProject_vsnprintf(s, format, n, (unsigned int *)(&n + 1));
}

int CHNOSProject_vsnprintf(unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[])
{
	unsigned char c;
	unsigned int i;

	CHNOSProject_snprintf_WorkArea work;

	CHNOSProject_snprintf_Initialise_WorkArea(&work, s, format, n, vargs);

	for(;;){
		if(CHNOSProject_snprintf_Check_FormatBuffer(&work) == -1){
			break;
		}
		c = CHNOSProject_snprintf_Read_FormatBuffer(&work);
		if(work.format_phase > 0){	/*�����w�蒆*/
			if(c == '%'){		/*%�������o�͂��܂��B*/
				CHNOSProject_snprintf_Write_DestinationBuffer(&work, '%');
				work.format_phase = 0;
			} else if(c == 'o'){	/*�f�[�^��8�i���ŏo�͂��܂��B*/
			} else if(c == 'd'){	/*�f�[�^��10�i���ŏo�͂��܂��B*/
			} else if(c == 'i'){	/*�f�[�^��10�i���ŏo�͂��܂��B*/
			} else if(c == 'x'){	/*�f�[�^��16�i���ŏo�͂��܂��Bx:�A���t�@�x�b�g�������B*/
			} else if(c == 'X'){	/*�f�[�^��16�i���ŏo�͂��܂��BX:�A���t�@�x�b�g�啶���B*/
				CHNOSProject_snprintf_To_String_From_Hex(&work, CHNOSProject_snprintf_Get_NextArgument(&work));
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 8; i++){
					CHNOSProject_snprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'u'){	/*�f�[�^�𕄍��Ȃ�10�i���ŏo�͂��܂��B*/
				CHNOSProject_snprintf_To_String_From_Decimal_Unsigned(&work, CHNOSProject_snprintf_Get_NextArgument(&work));
				for(i = 0; i < 10; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 10; i++){
					CHNOSProject_snprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'c'){	/*�f�[�^��ASCII�R�[�h�Ɖ��߂��Ĉꕶ�����o�͂��܂��B*/
			} else if(c == 's'){	/*�f�[�^�𕶎���ւ̃|�C���^�Ɖ��߂��ĕ�������o�͂��܂��B������̏I�[��0(null)�ł���K�v������܂��B*/
			} else if(c == 'f'){	/*�f�[�^�𕂓������_���Ƃ��ďo�͂��܂��Bfloat/double�̋�ʂ͂��܂���B�W���̐��x��6���ł��B�ϒ������̏ꍇ�A���������_���͏��double�Ɋg������邩��ł��B*/
				work.temporary_data_double[0] = CHNOSProject_snprintf_Get_NextArgument(&work);
				work.temporary_data_double[1] = CHNOSProject_snprintf_Get_NextArgument(&work);

				CHNOSProject_snprintf_To_String_From_Hex(&work, work.temporary_data_double[1]);
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 8; i++){
					CHNOSProject_snprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}

				CHNOSProject_snprintf_To_String_From_Hex(&work, work.temporary_data_double[0]);
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 8; i++){
					CHNOSProject_snprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
				work.format_phase = 0;
			} else if(c == 'e'){	/*�f�[�^���w���`���ŏo�͂��܂��Be:�A���t�@�x�b�g�������B�W���̐��x��6���ł��B*/
				
			} else if(c == 'E'){	/*�f�[�^���w���`���ŏo�͂��܂��BE:�A���t�@�x�b�g�啶���B�W���̐��x��6���ł��B*/
			} else if(c == 'g'){	/*�ʏ��f�t�H�[�}�b�g�w��q�Ɠ�����������܂����A�w������-5�ȉ����A�L�����x�ȏ�̂Ƃ���*/
			} else if(c == 'G'){	/*e,E�t�H�[�}�b�g�w��q�Ɠ�����������܂��B���̏����̖ړI�́A�Ȃ�ׂ��Z���\���ŏo�͂��邱�Ƃł��B*/
			} else if(c == 'p'){	/*�f�[�^�����炩�̃|�C���^�Ɖ��߂��āA���̎w�������A�h���X���o�͂��܂��B*/
			} else if(c == 'n'){	/*���̃t�H�[�}�b�g�w��q���܂ރt�H�[�}�b�g�w��ɒB����܂ŁA����o�͂������������A�f�[�^��unsigned int *�Ƃ��ĉ��߂��A�|�C���^���w���������unsigned int�^�ϐ��ɑ�����܂��B*/
			} else{
				work.format_phase = 0;
			}
		} else{	/*��ʕ�������*/
			if(c == '%'){	/*������͏����w��*/
				work.format_phase = 1;
			} else{	/*��ʕ����o�͒�*/
				CHNOSProject_snprintf_Write_DestinationBuffer(&work, c);
			}
		}
	}

	CHNOSProject_snprintf_End(&work);

	return work.index_destination_buf;
}

void CHNOSProject_snprintf_Initialise_WorkArea(CHNOSProject_snprintf_WorkArea *work, unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[])
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

int CHNOSProject_snprintf_Check_FormatBuffer(CHNOSProject_snprintf_WorkArea *work)
{
	if(work->format_buf[work->index_format_buf] == 0x00){
		return -1;
	}
	return 0;
}

int CHNOSProject_snprintf_Check_DestinationBuffer(CHNOSProject_snprintf_WorkArea *work)
{
	if(work->index_destination_buf >= work->length_destination_buf){
		return -1;
	}
	return 0;
}

unsigned char CHNOSProject_snprintf_Read_FormatBuffer(CHNOSProject_snprintf_WorkArea *work)
{
	if(CHNOSProject_snprintf_Check_FormatBuffer(work) == -1){
		return 0;
	}
	work->index_format_buf++;
	return work->format_buf[work->index_format_buf - 1];
}

void CHNOSProject_snprintf_Write_DestinationBuffer(CHNOSProject_snprintf_WorkArea *work, unsigned char c)
{
	if(CHNOSProject_snprintf_Check_DestinationBuffer(work) != -1){
		if(work->destination_buf != 0){
			work->destination_buf[work->index_destination_buf] = c;
		}
	}
	work->index_destination_buf++;
	return;
}

void CHNOSProject_snprintf_End(CHNOSProject_snprintf_WorkArea *work)
{
	if(work->destination_buf != 0){
		work->destination_buf[work->index_destination_buf] = 0x00;
	}
	return;
}

unsigned int CHNOSProject_snprintf_Get_NextArgument(CHNOSProject_snprintf_WorkArea *work)
{
	work->index_vargs++;
	return work->vargs[work->index_vargs - 1];
}

void CHNOSProject_snprintf_To_String_From_Hex(CHNOSProject_snprintf_WorkArea *work, unsigned int hex)
{
/*�e���|�����f�[�^�ɁA�E�l�߁A8���Œ�A�󔒏[�U�A�I�[null�A�啶��*/
	unsigned int i;

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

void CHNOSProject_snprintf_To_String_From_Decimal_Unsigned(CHNOSProject_snprintf_WorkArea *work, unsigned int d)
{
/*�e���|�����f�[�^�ɁA�E�l�߁A10���Œ�A�󔒏[�U�A�I�[null*/
	unsigned int i;

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

//void CHNOSProject_snprintf_To_String_From_FloatingPoint_Logarithmic(CHNOSProject_snprintf_WorkArea *work)
//{
//	work->format_sign = work->temporary_data_double[1] >> 31;
//	work->format_exponent = (work->temporary_data_double[1] >> 20) & 0x7FF;
	
//}




