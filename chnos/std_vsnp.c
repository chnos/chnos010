//
//snprintf(beta)
//author:hikarupsp
//
//引数(unsigned char s[], const unsigned char format[], unsigned int n, ...)
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
//		:データをunsigned int *として解釈し、ポインタが指し示す先のunsigned int型変数に代入します。

typedef struct CHNOSPROJECT_SNPRINTF_WORKAREA {
	unsigned char *destination_buf;		/*書き込み先文字列の開始アドレス*/
	unsigned int length_destination_buf;	/*書き込み先文字列の最大サイズ*/
	unsigned int index_destination_buf;	/*書き込み先文字列のインデックス*/

	const unsigned char *format_buf;		/*書式指定文字列の開始アドレス*/
	unsigned int index_format_buf;		/*書式指定文字列のインデックス*/

	unsigned char temporary_data[16];	/*一時データの配列*/
	unsigned int length_temporary_data;	/*一時データ（数字など）の大きさ*/

	unsigned int *vargs;			/*可変長引数の開始アドレス*/
	unsigned int index_vargs;		/*可変長引数の現在の場所*/

	unsigned int format_phase;		/*フォーマットの段階を示す。*/
						/*数値	:意味*/
						/*0	:フォーマット指定中ではない*/
						/*1	:フラグ以降の書式を要求*/
						/*2	:フィールド幅以降の書式を要求。*/
						/*3	:精度以降の書式を要求。*/
						/*4	:変換修飾子以降の書式を要求。*/
						/*5	:フォーマット指定子を要求*/
} CHNOSProject_snprintf_WorkArea;

int CHNOSProject_vsnprintf(unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[]);
void CHNOSProject_snprintf_Initialise_WorkArea(CHNOSProject_snprintf_WorkArea *work, unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[]);
int CHNOSProject_snprintf_Check_FormatBuffer(CHNOSProject_snprintf_WorkArea *work);
int CHNOSProject_snprintf_Check_DestinationBuffer(CHNOSProject_snprintf_WorkArea *work);
unsigned char CHNOSProject_snprintf_Read_FormatBuffer(CHNOSProject_snprintf_WorkArea *work);
void CHNOSProject_snprintf_Write_DestinationBuffer(CHNOSProject_snprintf_WorkArea *work, unsigned char c);
void CHNOSProject_snprintf_End(CHNOSProject_snprintf_WorkArea *work);
unsigned int CHNOSProject_snprintf_Get_NextArgument(CHNOSProject_snprintf_WorkArea *work);

int CHNOSProject_snprintf(unsigned char s[], const unsigned char format[], unsigned int n, ...)
{
	return CHNOSProject_vsnprintf(s, format, n, (unsigned int *)(&n + 1));
}

int CHNOSProject_vsnprintf(unsigned char s[], const unsigned char format[], unsigned int n, unsigned int vargs[])
{
	unsigned char c;

	CHNOSProject_snprintf_WorkArea work;

	CHNOSProject_snprintf_Initialise_WorkArea(&work, s, format, n, vargs);

	for(;;){
		if(CHNOSProject_snprintf_Check_FormatBuffer(&work) == -1 || CHNOSProject_snprintf_Check_DestinationBuffer(&work) == -1){
			break;
		}
		c = CHNOSProject_snprintf_Read_FormatBuffer(&work);
		if(work.format_phase > 0){	/*書式指定中*/
			if(work.format_phase <= 1){

			}
			if(c == '%'){		/*%文字を出力します。*/
				CHNOSProject_snprintf_Write_DestinationBuffer(&work, '%');
				work.format_phase = 0;
			} else if(c == 'o'){	/*データを8進数で出力します。*/
			} else if(c == 'd'){	/*データを10進数で出力します。*/
			} else if(c == 'i'){	/*データを10進数で出力します。*/
			} else if(c == 'x'){	/*データを16進数で出力します。x:アルファベット小文字。*/
			} else if(c == 'X'){	/*データを16進数で出力します。X:アルファベット大文字。*/
			} else if(c == 'u'){	/*データを符号なし10進数で出力します。*/
			} else if(c == 'c'){	/*データをASCIIコードと解釈して一文字を出力します。*/
			} else if(c == 's'){	/*データを文字列へのポインタと解釈して文字列を出力します。文字列の終端は0(null)である必要があります。*/
			} else if(c == 'f'){	/*データを浮動小数点数として出力します。float/doubleの区別はしません。標準の精度は6桁です。*/
			} else if(c == 'e'){	/*データを指数形式で出力します。e:アルファベット小文字。標準の精度は6桁です。*/
			} else if(c == 'E'){	/*データを指数形式で出力します。E:アルファベット大文字。標準の精度は6桁です。*/
			} else if(c == 'g'){	/*通常はfフォーマット指定子と同じ動作をしますが、指数部が-5以下か、有効精度以上のときは*/
			} else if(c == 'G'){	/*e,Eフォーマット指定子と同じ動作をします。この書式の目的は、なるべく短い表現で出力することです。*/
			} else if(c == 'p'){	/*データを何らかのポインタと解釈して、その指し示すアドレスを出力します。*/
			} else if(c == 'n'){	/*このフォーマット指定子を含むフォーマット指定に達するまで、今回出力した文字数を、データをunsigned int *として解釈し、ポインタが指し示す先のunsigned int型変数に代入します。*/
			}
		} else{	/*一般文字かも*/
			if(c == '%'){	/*次からは書式指定*/
				work.format_phase = 1;
			} else{	/*一般文字出力中*/
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
	work->length_temporary_data = 0;

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



