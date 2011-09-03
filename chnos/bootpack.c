
#include "core.h"

uint SystemPhase;

uchar *SystemPhaseText[] = {
	" 0:Initialising Hardware (Protected 32bit Text Mode)",
	" 1:Initialising Software (Protected 32bit Text Mode)"
};

void CHNMain(void)
{
	IO_CLI();

	IO_STI();

	TextMode_Clear_Screen();
	Error_Set_Enable_Display_TextMode(true);

	TextMode_Put_String("Welcome to CHNOSProject!\n", green);

	CHNOS_Set_SystemPhase(0);

	TextMode_Put_String("\tInitialising SerialPort...\n", white);
	Initialise_SerialPort();
	Error_Set_Enable_SerialPort(true);

	TextMode_Put_String("\tInitialising GDT...\n", white);
	Initialise_GlobalDescriptorTable();

	TextMode_Put_String("\tInitialising IDT...\n", white);
	Initialise_InterruptDescriptorTable();

	TextMode_Put_String("\tInitialising PIT...\n", white);
	Initialise_ProgrammableInterruptController();

	TextMode_Put_String("\tHardware Initialising Phase End.\n", white);

	CHNOS_Set_SystemPhase(1);

	INT_3();

	for (;;) {
		IO_HLT();
	}
}

void CHNOS_Set_SystemPhase(uint phase)
{
	SystemPhase = phase;
	TextMode_Put_String("\nNow SystemPhase is", white);
	TextMode_Put_String(SystemPhaseText[SystemPhase], skyblue);
	TextMode_Put_String("\n", white);

	return;
}
