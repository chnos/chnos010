//引数(uchar s[], const uchar format[], uint n, ...)
//	s	:結果を書き込む文字列の先頭アドレスを指定します。
//	format	:書式指定文字列の先頭アドレスを指定します。終端は0x00である必要があります。
//	n	:s[]の大きさを指定します。(n - 1)番目以降の文字は書き込まれません。
//		:ただし、ゼロを指定した場合は、何も出力しません。この場合、s[]は0(null)を指定することもできます。
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
		if(work.format_phase > 0){	/*書式指定中*/
			if(c == '%'){		/*%文字を出力します。*/
				CFunction_vsnprintf_Write_DestinationBuffer(&work, '%');
				work.format_phase = 0;
			} else if(c == 'o'){	/*データを8進数で出力します。*/
			} else if(c == 'd'){	/*データを10進数で出力します。*/
			} else if(c == 'i'){	/*データを10進数で出力します。*/
			} else if(c == 'x'){	/*データを16進数で出力します。x:アルファベット小文字。*/
				CFunction_vsnprintf_To_String_From_Hex_Lower(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
					}
				}
				for(; i < 8; i++){
					CFunction_vsnprintf_Write_DestinationBuffer(&work, work.temporary_data[i]);
				}
			} else if(c == 'X'){	/*データを16進数で出力します。X:アルファベット大文字。*/
				CFunction_vsnprintf_To_String_From_Hex_Upper(&work, CFunction_vsnprintf_Get_NextArgument(&work));
				for(i = 0; i < 8; i++){
					if(work.temporary_data[i] != ' '){
						break;
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
			} else{
				CFunction_vsnprintf_Write_DestinationBuffer(&work, c);
				work.format_phase = 0;
			}
		} else{	/*一般文字かも*/
			if(c == '%'){	/*次からは書式指定*/
				work.format_phase = 1;
			} else{	/*一般文字出力中*/
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



