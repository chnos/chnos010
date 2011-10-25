
#include "core.h"

void TestTask1(void);
void TestTask2(void);

void CHNMain(void)
{
	uchar s[128];
	uint i;
	UI_Task *test1, *test2;
	UI_Task *mytask;

	Initialise_System();

	test1 = System_MultiTask_Task_Initialise();
	test1->tss->eip = (uint)&TestTask1;
	test1->tss->cs = SYSTEM_CS << 3;
	test1->tss->ss = SYSTEM_DS << 3;
	test1->tss->ds = SYSTEM_DS << 3;
	test1->tss->esp = (uint)System_Memory_Allocate(4096) + 4096;
	System_MultiTask_Task_Run(test1);

	test2 = System_MultiTask_Task_Initialise();
	test2->tss->eip = (uint)&TestTask2;
	test2->tss->cs = SYSTEM_CS << 3;
	test2->tss->ss = SYSTEM_DS << 3;
	test2->tss->ds = SYSTEM_DS << 3;
	test2->tss->esp = (uint)System_Memory_Allocate(4096) + 4096;
	System_MultiTask_Task_Run(test2);

	i = 0;

	mytask = System_MultiTask_GetNowTask();

	for (;;) {
		i++;
		snprintf(s, "MainTask0=%d=%d", sizeof(s), mytask->count, i);
		TextMode_Put_String_Absolute(s, white, 0, 1);
	}
}

void TestTask1(void)
{
	uchar s[128];
	uint i;
	UI_Task *mytask;

	i = 0;

	mytask = System_MultiTask_GetNowTask();

	for (;;) {
		i++;
		snprintf(s, "TestTask1=%d=%d", sizeof(s), mytask->count, i);
		TextMode_Put_String_Absolute(s, white, 0, 2);
	}
}

void TestTask2(void)
{
	uchar s[128];
	uint i;
	UI_Task *mytask;

	i = 0;

	mytask = System_MultiTask_GetNowTask();

	for (;;) {
		i++;
		snprintf(s, "TestTask2=%d=%d", sizeof(s), mytask->count, i);
		TextMode_Put_String_Absolute(s, white, 0, 3);
	}
}
