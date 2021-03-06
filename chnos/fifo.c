
#include "core.h"

//指定サイズは、バイト単位。4バイト切り捨て。
//内部サイズは、エントリ(uint=4byte)単位。

DATA_FIFO32 *FIFO32_Initialize(IO_MemoryControl memctrl, uint size)
{
	DATA_FIFO32 *fifo;

	//fifo = Memory_Allocate(memctrl, sizeof(DATA_FIFO32));
	fifo = (DATA_FIFO32 *)System_CommonStruct_Allocate(SYSTEM_STRUCTID_FIFO32);

	fifo->size = size >> 2;
	fifo->buf = Memory_Allocate(memctrl, fifo->size << 2);
	fifo->free = fifo->size;	/*freesize*/
	fifo->flags.initialized = False;
	fifo->flags.overflow = False;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/
	fifo->flags.initialized = True;
	return fifo;
}

int FIFO32_Put(DATA_FIFO32 *fifo, uint data)
{
	uint eflags;

	if(fifo == Null){
		return -1;
	}

	if(!fifo->flags.initialized){
		return -2;
	}

	eflags = IO_Load_EFlags();
	IO_CLI();

	if(fifo->free == 0){
		#ifdef CHNOSPROJECT_DEBUG_FIFO
			debug("FIFO32_Put:Overflow data.\n");
		#endif
		fifo->flags.overflow = True;
		Error_Report(ERROR_FIFO_BUFFER_OVERFLOW, fifo);
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if(fifo->p == fifo->size){
		fifo->p = 0;
	}
	fifo->free--;


	if(fifo->task != Null){
		if(fifo->task->flags.linked && !fifo->task->flags.running){
			#ifdef CHNOSPROJECT_DEBUG_FIFO
				debug("FIFO32_Put:Task run start.\n");
			#endif
			System_MultiTask_Task_Run(fifo->task);
			#ifdef CHNOSPROJECT_DEBUG_FIFO
				debug("FIFO32_Put:Task run end.\n");
			#endif

		}
	}

	IO_Store_EFlags(eflags);

	return 0;
}

int FIFO32_Put_Arguments(DATA_FIFO32 *fifo, uint args, ...)
{
	int i;
	uint *vargs;
	uint eflags;

	if(fifo == Null){
		return -1;
	}

	if(!fifo->flags.initialized){
		return -2;
	}

	if(args == 0){
		return 0;
	}

	eflags = IO_Load_EFlags();
	IO_CLI();

	vargs = (uint *)(&args + 1);

	for(i = 0; i < args; i++){
		if(fifo->free <= 1){
			i = -1;
			break;
		}
		FIFO32_Put(fifo, vargs[i]);
	}
	FIFO32_Put(fifo, SIGNAL_ARGUMENTS_END);

	IO_Store_EFlags(eflags);

	return i; 
}

void FIFO32_Set_Task(DATA_FIFO32 *fifo, UI_Task *task)
{
	if(fifo == Null){
		return;
	}

	fifo->task = task;
	return;
}

uint FIFO32_Get(DATA_FIFO32 *fifo)
{
	int data;

	if(fifo == Null){
		return 0;
	}

	if(!fifo->flags.initialized){
		return 0;
	}

	if (fifo->free == fifo->size) {	/*バッファが空の時は、最後に読みとられたデータを繰り返し返す*/
		if(fifo->q == 0){	/*読みだしポイントが最初のときは、最後に読みとられたデータは終端にある*/
			return fifo->buf[fifo->size];
		}
		return fifo->buf[fifo->q - 1];
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {	/*読み出しポイントが終端まで行ったので、先頭に戻す*/
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

uint FIFO32_Status(DATA_FIFO32 *fifo)
{
	if(fifo == Null){
		return 0;
	}

	if(!fifo->flags.initialized){
		return 0;
	}

	return fifo->size - fifo->free;	/*バッファに存在するデータのエントリ数(エントリ数=uintの個数->エントリ数*4=使われているバイト数)*/
}

void FIFO32_Free(DATA_FIFO32 *fifo)
{
	if(fifo == Null){
		return;
	}

	if(!fifo->flags.initialized){
		return;
	}

	System_Memory_Free(fifo->buf, fifo->size << 2);

	fifo->size = 0;
	fifo->buf = 0;
	fifo->free = 0;	/*freesize*/
	fifo->flags.initialized = False;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/

	System_CommonStruct_Free(&fifo->common_tag);
	return;
}

uint FIFO32_MyTaskFIFO_Status(void)
{
	return FIFO32_Status(System_MultiTask_GetNowTask()->fifo);
}

uint FIFO32_MyTaskFIFO_Get(void)
{
	return FIFO32_Get(System_MultiTask_GetNowTask()->fifo);
}

