
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

//文字列から数値を生成します。
//base=0:C言語の定数表記ルールに従う。("0x..."=16, "0..."=8, else=10)
//base=2-36(数字10+アルファベット26):各数字を基数として文字列を理解しようとする。
//アルファベットの大文字・小文字は問わない。
//endptr=NULLも可。有効な数字が一つもなければ、*endptr=s, retv=0となる。
//出力のオーバーフローについては考慮しないので注意。
//現状では負数は扱えない(マイナス記号は変換不能とされる)
//sの例:"0xFEC375"(16), "0423"(8), "12345"(10)
//文字列全体の前には空白があっても無視される。数字部分開始後は空白は変換不能とされる。
//*endptr[0]==0x00のとき、文字列sのすべてが変換されたことを示す。
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

//空白を読み飛ばす
	for(i = 0; s[i] != 0x00; i++){
		if(s[i] != ' '){
			break;
		}
	}
//base=0の時の基数決定
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
//数値変換
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

//文字列内のn番目の単語(スペース区切りによる)の先頭アドレスを取得する。
//戻り値は、n番目の単語が存在するかどうかを返す。
//存在しなかった場合の*wordptrはsの末端を返す。
//wordptrはNull許容。
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

//source番地からの、destination_sizeを超えないsource_sizeバイトを、destination番地へ移動させる。
//メモリ範囲が重なっていてもデータは破壊されない(範囲が重なっている場合、sourceの内容が変更される可能性はあるが、移動されるデータ自体は保持される)。
//同一番地へコピーする場合は何もしない。
//戻り値は、コピーしたバイト数。
uint CFunction_MemoryMove(void *destination, uint destination_size, const void *source, uint source_size)
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
		//sourceを上書きしてしまう場合、一度バッファに退避させた後コピーする。
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
		temp = (uchar *)source;
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

//引数(uchar s[], uint n, const uchar format[], ...)
//	s	:結果を書き込む文字列の先頭アドレスを指定します。
//	n	:s[]の大きさを指定します。(n - 1)番目以降の文字は書き込まれません。
//		:ただし、ゼロを指定した場合は、何も出力しません。この場合、s[]は0(null)を指定することもできます。
//	format	:書式指定文字列の先頭アドレスを指定します。終端は0x00である必要があります。
//	...	:可変長引数です。
//
//誤ったフォーマットが指定された場合、その部分のフォーマット解釈を終了し、
//解釈できなかった部分から通常文字として出力します
//
//%[フラグ][最小フィールド幅][精度][変換修飾子]フォーマット指定子"
//	text	:平文は%を除き、そのまま出力されます。
//	使用可能なフォーマット指定子:
//	%%	:%文字を出力します。
//	o	:データを8進数で出力します。
//	d, i	:データを10進数で出力します。
//	x, X	:データを16進数で出力します。x:アルファベット小文字。X:アルファベット大文字。
//	u	:データを符号なし10進数で出力します。
//	c	:データをASCIIコードと解釈して一文字を出力します。
//	s	:データを文字列へのポインタと解釈して文字列を出力します。文字列の終端は0(null)である必要があります。
//	f	:データを浮動小数点数として出力します。float/doubleの区別はしません。標準の精度は6桁です。
//	e, E	:データを指数形式で出力します。e:アルファベット小文字。E:アルファベット大文字。標準の精度は6桁です。
//	g, G	:通常はfフォーマット指定子と同じ動作をしますが、
//		:指数部が-5以下か、有効精度以上のときはe,Eフォーマット指定子と同じ動作をします。
//		:この書式の目的は、なるべく短い表現で出力することです。
//	p	:データを何らかのポインタと解釈して、その指し示すアドレスを出力します。
//	n	:このフォーマット指定子を含むフォーマット指定に達するまで、今回出力した文字数を、
//		:データをuint *として解釈し、ポインタが指し示す先のuint型変数に代入します。

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
		if(work.format_phase > 0){	/*書式指定中*/
			if(c == '%'){		/*%文字を出力します。*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, '%');
				work.format_phase = 0;
			} else if(c == 'o'){	/*データを8進数で出力します。*/
			} else if(c == 'd'){	/*データを10進数で出力します。*/
				/*標準精度は一桁以上、ゼロフィルです。*/
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
			} else if(c == 'i'){	/*データを10進数で出力します。*/
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
			} else if(c == 'x'){	/*データを16進数で出力します。x:アルファベット小文字。*/
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
			} else if(c == 'X'){	/*データを16進数で出力します。X:アルファベット大文字。*/
				/*標準精度は一桁以上、ゼロフィルです。*/
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
			} else if(c == 'u'){	/*データを符号なし10進数で出力します。*/
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
			} else if(c == 'c'){	/*データをASCIIコードと解釈して一文字を出力します。*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				work.format_phase = 0;
			} else if(c == 's'){	/*データを文字列へのポインタと解釈して文字列を出力します。文字列の終端は0(null)である必要があります。*/
				d = (const uchar *)CFunction_vsnprintf_Get_NextArgument(&work);
				for (; *d != 0x00; d++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, *d);
				}
				work.format_phase = 0;
			} else if(c == 'f'){	/*データを浮動小数点数として出力します。float/doubleの区別はしません。標準の精度は6桁です。可変長引数の場合、浮動小数点数は常にdoubleに拡張されるからです。*/
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
			} else if(c == 'e'){	/*データを指数形式で出力します。e:アルファベット小文字。標準の精度は6桁です。*/
				
			} else if(c == 'E'){	/*データを指数形式で出力します。E:アルファベット大文字。標準の精度は6桁です。*/
			} else if(c == 'g'){	/*通常はfフォーマット指定子と同じ動作をしますが、指数部が-5以下か、有効精度以上のときは*/
			} else if(c == 'G'){	/*e,Eフォーマット指定子と同じ動作をします。この書式の目的は、なるべく短い表現で出力することです。*/
			} else if(c == 'p'){	/*データを何らかのポインタと解釈して、その指し示すアドレスを出力します。*/
			} else if(c == 'n'){	/*このフォーマット指定子を含むフォーマット指定に達するまで、今回出力した文字数を、データをuint *として解釈し、ポインタが指し示す先のuint型変数に代入します。*/
			} else if(0x30 <= c && c <= 0x39){	/*数字*/
				c -= 0x30;
				if(fill_length == 0xffffffff && c == 0){	/*最初のゼロ:ゼロ充填を要求*/
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
		} else{	/*一般文字かも*/
			if(c == '%'){	/*次からは書式指定*/
				flag_fill_zero = False;
				fill_length = 0xffffffff;
				work.format_phase = 1;
			} else{	/*一般文字出力中*/
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
/*テンポラリデータに、右詰め、8桁固定、空白充填、終端null、大文字*/
	uint i;

	for(i = 0; i < 8; i++){	/*値の分配*/
		work->temporary_data[7 - i] = (hex >> (i << 2)) & 0x0000000f;
	}
	work->temporary_data[8] = 0x00;	/*終端null*/
	for(i = 0; i < 8; i++){	/*空白充填*/
		if(work->temporary_data[i] != 0x00){
			break;
		}
		work->temporary_data[i] = ' ';
	}
	for(; i < 8; i++){	/*ASCIIコードへ変換（大文字）*/
		if(work->temporary_data[i] > 9){	/*アルファベット大文字へ変換*/
			work->temporary_data[i] += 55;
		} else{	/*数字へ変換*/
			work->temporary_data[i] += 48;
		}
	}
	return;
}

void CFunction_vsnprintf_To_String_From_Hex_Lower(CFunction_vsnprintf_WorkArea *work, uint hex)
{
/*テンポラリデータに、右詰め、8桁固定、空白充填、終端null、小文字*/
	uint i;

	for(i = 0; i < 8; i++){	/*値の分配*/
		work->temporary_data[7 - i] = (hex >> (i << 2)) & 0x0000000f;
	}
	work->temporary_data[8] = 0x00;	/*終端null*/
	for(i = 0; i < 8; i++){	/*空白充填*/
		if(work->temporary_data[i] != 0x00){
			break;
		}
		work->temporary_data[i] = ' ';
	}
	for(; i < 8; i++){	/*ASCIIコードへ変換（小文字）*/
		if(work->temporary_data[i] > 9){	/*アルファベット小文字へ変換*/
			work->temporary_data[i] += 87;
		} else{	/*数字へ変換*/
			work->temporary_data[i] += 48;
		}
	}
	return;
}

void CFunction_vsnprintf_To_String_From_Decimal_Unsigned(CFunction_vsnprintf_WorkArea *work, uint d)
{
/*テンポラリデータに、右詰め、10桁固定、空白充填、終端null*/
	uint i;

	for(i = 0; i < 10; i++){	/*値の分配*/
		work->temporary_data[9 - i] = d % 10;
		d = d / 10;
	}
	work->temporary_data[10] = 0x00;	/*終端null*/
	for(i = 0; i < 10; i++){	/*空白充填*/
		if(work->temporary_data[i] != 0x00){
			break;
		}
		work->temporary_data[i] = ' ';
	}
	for(; i < 10; i++){	/*ASCIIコードへ変換*/
		work->temporary_data[i] += 48;
	}
	return;
}

