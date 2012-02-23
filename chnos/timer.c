
#include "core.h"

UI_TimerControl *timerctrl;

UI_TimerControl *Initialise_ProgrammableIntervalTimer(void)
{
	timerctrl = (UI_TimerControl *)System_Memory_Allocate(sizeof(UI_TimerControl));

	timerctrl->tick_10ms = 0;
	timerctrl->TaskSwitch = &Timer_TaskSwitch_Invalid;

//config watch
	timerctrl->timer_root = Timer_Initialise();
	Timer_Config(timerctrl->timer_root, 0xfffffff, Null, 0, True);
	timerctrl->timer_root->timeout = 0xffffffff;
	timerctrl->timer_root->flags.bit.running = True;

//config PIT
	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	System_GateDescriptor_Set(0x20, (uint)asm_InterruptHandler20, 0x02, AR_INTGATE32);
	ProgrammableInterruptController_InterruptMask_Clear(0x00);

	return timerctrl;
}

void InterruptHandler20(uint *esp)
{
	ProgrammableInterruptController_InterruptRequest_Complete(0x00);
	timerctrl->tick_10ms++;

	if(timerctrl->timer_root->timeout <= timerctrl->tick_10ms){
		Timer_TimeOut();
	}

	if((timerctrl->tick_10ms & 0x00000003) == 0){
		timerctrl->TaskSwitch();
	}

	return;
}

void Timer_Set_TaskSwitch(void (*TaskSwitchFunction)(void))
{
	if(TaskSwitchFunction != Null){
		timerctrl->TaskSwitch = TaskSwitchFunction;
	} else{
		timerctrl->TaskSwitch = Timer_TaskSwitch_Invalid;
	}
	return;
}

void Timer_TaskSwitch_Invalid(void)
{
	return;
}

UI_Timer *Timer_Initialise(void)
{
	UI_Timer *timer;

	timer = System_Memory_Allocate(sizeof(UI_Timer));

	timer->flags.bit.initialized = True;

	#ifdef CHNOSPROJECT_DEBUG_TIMER
		debug("Timer_Initialise:[0x%08X]\n", timer);
	#endif

	return timer;
}

uint Timer_Config(UI_Timer *timer, uint tick_ms, DATA_FIFO32 *fifo, uint fifo_putdata, bool interval)
{
	if(timer == Null){
		return 1;
	}

	if(!timer->flags.bit.initialized){
		return 2;
	}

	timer->tick = tick_ms / 10;
	timer->fifo = fifo;
	timer->fifo_putdata = fifo_putdata;
	timer->flags.bit.interval = interval;

	timer->flags.bit.configured = True;

	return 0;
}

uint Timer_Run(UI_Timer *timer)
{
	uint eflags;
	UI_Timer **search;

	if(timer == Null){
		return 1;
	}

	if(!timer->flags.bit.configured){
		return 2;
	}

	if(timer->flags.bit.running){
		return 3;
	}

	timer->timeout = timer->tick + timerctrl->tick_10ms;
	search = &timerctrl->timer_root;

	eflags = IO_Load_EFlags();

	for(;;){
		if((*search) == Null){
			#ifdef CHNOSPROJECT_DEBUG_TIMER
				debug("Timer_Run:Invalid link. Abort.\n");
			#endif
			INT_3();
		}
		if((*search)->timeout > timer->timeout){
			IO_CLI();
			timer->root_next = *search;
			*search = timer;
			timer->tree_next = Null;
			timer->flags.bit.running = True;
			break;
		}
		if((*search)->timeout == timer->timeout){
			IO_CLI();
			search = &(*search)->tree_next;
			for(;;){
				if(*search == Null){
					break;
				}
				search = &(*search)->tree_next;
			}
			timer->root_next = Null;
			*search = timer;
			timer->tree_next = Null;
			timer->flags.bit.running = True;
			break;
		}
		search = &(*search)->root_next;
	}

	IO_Store_EFlags(eflags);

	return 0;
}

uint Timer_TimeOut(void)
{
	UI_Timer *search, *old;

	search = timerctrl->timer_root;
	timerctrl->timer_root = timerctrl->timer_root->root_next;

	for(;;){
		if(search->fifo != Null){
			FIFO32_Put(search->fifo, search->fifo_putdata);
		}
		old = search;
		search = old->tree_next;
		old->tree_next = 0;
		old->flags.bit.running = False;
		if(old->flags.bit.interval){
			Timer_Run(old);
		}
		if(search == Null){
			break;
		}
	}
	return 0;
}

uint Timer_GetTick(void)
{
	return timerctrl->tick_10ms;
}

