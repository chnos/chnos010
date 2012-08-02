
#include "core.h"

uint system_seed;

#define RAND_A	((10 << 3) + 5)
#define RAND_B	((15 << 1) + 1)
#define RAND_C	(1 << 24)

void srand(uint seed)
{
	system_seed = seed;
	return;
}

uint rand(void)
{
	uint seed2;

	system_seed = ((system_seed * RAND_A) + RAND_B) % RAND_C;
	seed2 = ((system_seed * RAND_A) + RAND_B) % RAND_C;
	system_seed = (system_seed & 0xff5688) | ((seed2 >> 8) & 0xa977) | ((system_seed & 0x000ff000) << 12);
	return system_seed;
}

uint isqrt(uint n)
{
	uint x;

//Overflow Check
	if(n >= (uint)0xffff * (uint)0xffff){
		return 0xffff;
	}

	x = (1 << 15);

	if(n > x * x){
		x += (1 << 14);
	} else{
		x -= (1 << 14);
	}
	if(n > x * x){
		x += (1 << 13);
	} else{
		x -= (1 << 13);
	}
	if(n > x * x){
		x += (1 << 12);
	} else{
		x -= (1 << 12);
	}
	if(n > x * x){
		x += (1 << 11);
	} else{
		x -= (1 << 11);
	}
	if(n > x * x){
		x += (1 << 10);
	} else{
		x -= (1 << 10);
	}
	if(n > x * x){
		x += (1 << 9);
	} else{
		x -= (1 << 9);
	}
	if(n > x * x){
		x += (1 << 8);
	} else{
		x -= (1 << 8);
	}
	if(n > x * x){
		x += (1 << 7);
	} else{
		x -= (1 << 7);
	}
	if(n > x * x){
		x += (1 << 6);
	} else{
		x -= (1 << 6);
	}
	if(n > x * x){
		x += (1 << 5);
	} else{
		x -= (1 << 5);
	}
	if(n > x * x){
		x += (1 << 4);
	} else{
		x -= (1 << 4);
	}
	if(n > x * x){
		x += (1 << 3);
	} else{
		x -= (1 << 3);
	}
	if(n > x * x){
		x += (1 << 2);
	} else{
		x -= (1 << 2);
	}
	if(n > x * x){
		x += (1 << 1);
	} else{
		x -= (1 << 1);
	}
	if(n > x * x){
		x += (1 << 0);
	} else{
		x -= (1 << 0);
	}

	if(n > x * x){
		x++;
	}
	if(n < x * x){
		x--;
	}

	return x;
}

//�����񂩂琔�l�𐶐����܂��B
//base=0:C����̒萔�\�L���[���ɏ]���B("0x..."=16, "0..."=8, else=10)
//base=2-36(����10+�A���t�@�x�b�g26):�e��������Ƃ��ĕ�����𗝉����悤�Ƃ���B
//�A���t�@�x�b�g�̑啶���E�������͖��Ȃ��B
//endptr=NULL���B�L���Ȑ���������Ȃ���΁A*endptr=s, retv=0�ƂȂ�B
//�o�͂̃I�[�o�[�t���[�ɂ��Ă͍l�����Ȃ��̂Œ��ӁB
//����ł͕����͈����Ȃ�(�}�C�i�X�L���͕ϊ��s�\�Ƃ����)
//s�̗�:"0xFEC375"(16), "0423"(8), "12345"(10)
//������S�̂̑O�ɂ͋󔒂������Ă����������B���������J�n��͋󔒂͕ϊ��s�\�Ƃ����B
//*endptr[0]==0x00�̂Ƃ��A������s�̂��ׂĂ��ϊ����ꂽ���Ƃ������B
uint strtol(const uchar s[], uchar *endptr[], uint base)
{
	uint i, v;

	v = 0;

	if(s == Null || base > 36 || base == 1){
		if(endptr != Null){
			*endptr = Null;
		}
		return 0;
	}

//�󔒂�ǂݔ�΂�
	for(i = 0; s[i] != 0x00; i++){
		if(s[i] != ' '){
			break;
		}
	}
//base=0�̎��̊����
	if(base == 0){
		if(s[i] == '0'){
			i++;
			if(s[i] == 'x'){
				base = 16;
				i++;
			} else{
				base = 8;
			}
		} else{
			base = 10;
		}
	}
//���l�ϊ�
	for(; s[i] != 0x00; i++){
		if('0' <= s[i] <= '9' && s[i] - '0' < base){
			v *= base;
			v += s[i] - '0';
		} else if('A' <= s[i] <= 'Z' && s[i] - 'A' + 10 < base){
			v *= base;
			v += s[i] - 'A' + 10;
		} else if('a' <= s[i] <= 'z' && s[i] - 'a' + 10 < base){
			v *= base;
			v += s[i] - 'a' + 10;
		} else{
			break;
		}
	}
	if(endptr != Null){
		*endptr = (uchar *)&s[i];
	}
	return v;
}

bool CFunction_CompareStrings(const uchar s1[], const uchar s2[])
{
	uint i;

	for(i = 0; s1[i] == s2[i]; i++){
		if(s1[i] == 0x00){
			return True;
		}
	}
	return False;
}

bool CFunction_CompareStrings_n(const uchar s1[], const uchar s2[], unsigned int n)
{
	uint i;

	for(i = 0; i < n; i++){
		if(s1[i] != s2[i]){
			return False;
		}
		if(s1[i] == 0x00){
			return True;
		}
	}
	return True;
}

uint CFunction_ExtractBits(uint source, uint start, uint end)
{
	return (source >> start) & ~(0xffffffff << (end - start + 1));
}

//���������n�Ԗڂ̒P��(�X�y�[�X��؂�ɂ��)�̐擪�A�h���X���擾����B
//�߂�l�́An�Ԗڂ̒P�ꂪ���݂��邩�ǂ�����Ԃ��B
//���݂��Ȃ������ꍇ��*wordptr��s�̖��[��Ԃ��B
//wordptr��Null���e�B
bool CFunction_String_GetWord(const uchar s[], uchar *wordptr[], uint n)
{
	uint i, count;

	if(s == Null){
		if(wordptr != Null){
			*wordptr = Null;
		}
		return False;
	}

	if(n == 0){
		if(wordptr != Null){
			*wordptr = (uchar *)s;
		}
		return True;
	}

	count = 0;
	for(i = 0; s[i] != 0x00; i++){
		if(s[i] == ' '){
			count++;
			for(; s[i] != 0x00; i++){
				if(s[i] != ' '){
					break;
				}
			}
			if(s[i] != 0x00 && count == n){
				if(wordptr != Null){
					*wordptr = (uchar *)&s[i];
				}
				return True;
			}
		}
	}

	if(wordptr != Null){
		*wordptr = (uchar *)&s[i];
	}
	return False;
}

//source�Ԓn����́Adestination_size�𒴂��Ȃ�source_size�o�C�g���Adestination�Ԓn�ֈړ�������B
//�������͈͂��d�Ȃ��Ă��Ă��f�[�^�͔j�󂳂�Ȃ��B
//����Ԓn�փR�s�[����ꍇ�͉������Ȃ��B
//�߂�l�́A�R�s�[�����o�C�g���B
uint CFunction_MemoryMove(void *destination, uint destination_size, void *source, uint source_size)
{
	uint move_size, i;
	void *temp;

	if(destination == source){
		return 0;
	}

	if(destination_size <= source_size){
		move_size = destination_size;
	} else{
		move_size = source_size;
	}

	if((uint)source < (uint)destination && (uint)source + source_size > (uint)destination){
		//source���㏑�����Ă��܂��ꍇ�A��x�o�b�t�@�ɑޔ���������R�s�[����B
		temp = System_Memory_Allocate(move_size);
		i = 0;
		if(move_size >= 4){
			for(; i < move_size - (4 - 1); i += 4){
				((uint *)temp)[i >> 2] = ((uint *)source)[i >> 2];
			}
		}
		if(i != move_size){
			for(; i < move_size; i++){
				((uchar *)temp)[i] = ((uchar *)source)[i];
			}
		}
	} else{
		temp = source;
	}

	i = 0;
	if(move_size >= 4){
		for(; i < move_size - (4 - 1); i += 4){
			((uint *)destination)[i >> 2] = ((uint *)temp)[i >> 2];
		}
	}
	if(i != move_size){
		for(; i < move_size; i++){
			((uchar *)destination)[i] = ((uchar *)temp)[i];
		}
	}

	if(temp != source){
		System_Memory_Free(temp, move_size);
	}

	return move_size;
}

//����(uchar s[], uint n, const uchar format[], ...)
//	s	:���ʂ��������ޕ�����̐擪�A�h���X���w�肵�܂��B
//	n	:s[]�̑傫�����w�肵�܂��B(n - 1)�Ԗڈȍ~�̕����͏������܂�܂���B
//		:�������A�[�����w�肵���ꍇ�́A�����o�͂��܂���B���̏ꍇ�As[]��0(null)���w�肷�邱�Ƃ��ł��܂��B
//	format	:�����w�蕶����̐擪�A�h���X���w�肵�܂��B�I�[��0x00�ł���K�v������܂��B
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

int snprintf(uchar s[], uint n, const uchar format[], ...)
{
	return CFunction_vsnprintf(s, n, format, (uint *)(&format + 1));
}

int vsnprintf(uchar s[], uint n, const uchar format[], uint vargs[])
{
	return CFunction_vsnprintf(s, n, format, vargs);
}

int CFunction_vsnprintf(uchar s[], uint n, const uchar format[], uint vargs[])
{
	uchar c;
	uint i, j;
	const uchar *d;
	uint flag_fill_zero;
	uint fill_length;

	CFunction_vsnprintf_WorkArea work;

	CFunction_vsnprintf_Initialize_WorkArea(&work, s, format, n, vargs);	

	flag_fill_zero = False;
	fill_length = 0xffffffff;

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
				/*�W�����x�͈ꌅ�ȏ�A�[���t�B���ł��B*/
				if(fill_length == 0xffffffff){
					fill_length = 1;
					flag_fill_zero = True;
				}
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
				}
				if(i == 10){
					i--;
					work.temporary_data[10 - 1] = '0';
				}

				if((10 - i) < fill_length && fill_length != 0xffffffff){
					if(flag_fill_zero){
						for(j = 0; j < (fill_length - (10 - i)); j++){
							CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
						}
					} else{
						for(j = 0; j < (fill_length - (10 - i)); j++){
							CFunction_vsnprintf_Write_DestinationBuffer(&work, ' ');
						}
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
				/*�W�����x�͈ꌅ�ȏ�A�[���t�B���ł��B*/
				if(fill_length == 0xffffffff){
					fill_length = 1;
					flag_fill_zero = True;
				}
				CFunction_vsnprintf_To_String_From_Hex_Upper(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				if((8 - i) < fill_length && fill_length != 0xffffffff){
					if(flag_fill_zero){
						for(j = 0; j < (fill_length - (8 - i)); j++){
							CFunction_vsnprintf_Write_DestinationBuffer(&work, '0');
						}
					} else{
						for(j = 0; j < (fill_length - (8 - i)); j++){
							CFunction_vsnprintf_Write_DestinationBuffer(&work, ' ');
						}
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
				c -= 0x30;
				if(fill_length == 0xffffffff && c == 0){	/*�ŏ��̃[��:�[���[�U��v��*/
					flag_fill_zero = True;
					fill_length = 0;
				} else{
					if(fill_length == 0xffffffff){
						fill_length = c;
					} else{
						fill_length = (fill_length * 10) + c;
					}
				}
			} else{
				CFunction_vsnprintf_Write_DestinationBuffer(&work, c);
				work.format_phase = 0;
			}
		} else{	/*��ʕ�������*/
			if(c == '%'){	/*������͏����w��*/
				flag_fill_zero = False;
				fill_length = 0xffffffff;
				work.format_phase = 1;
			} else{	/*��ʕ����o�͒�*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, c);
			}
		}
	}

	CFunction_vsnprintf_End(&work);

	return work.index_destination_buf;
}

void CFunction_vsnprintf_Initialize_WorkArea(CFunction_vsnprintf_WorkArea *work, uchar s[], const uchar format[], uint n, uint vargs[])
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

