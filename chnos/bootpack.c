
#include "core.h"

uint SystemPhase;

uchar *SystemPhaseText[] = {
	" 0:Initialising Hardware (Protected 32bit Text Mode)",
	" 1:Initialising Software (Protected 32bit Text Mode)"
};

void CHNMain(void)
{
	uchar s[128];
	int n;

	IO_CLI();

	TextMode_Clear_Screen();
	Error_Set_Enable_Display_TextMode(true);

	TextMode_Put_String("Welcome to CHNOSProject!\n", green);

	CHNOSProject_Set_SystemPhase(0);

	TextMode_Put_String("\tInitialising SerialPort...\n", white);
	Initialise_SerialPort();
	Error_Set_Enable_SerialPort(true);

	TextMode_Put_String("\tInitialising GDT...\n", white);
	Initialise_GlobalDescriptorTable();

	TextMode_Put_String("\tInitialising IDT...\n", white);
	Initialise_InterruptDescriptorTable();

	TextMode_Put_String("\tInitialising PIC...\n", white);
	Initialise_ProgrammableInterruptController();

	TextMode_Put_String("\tInitialising PIT...\n", white);
	Initialise_ProgrammableIntervalTimer();

	TextMode_Put_String("\tInitialising Keyboard...\n", white);
	Initialise_Keyboard();

	TextMode_Put_String("\tHardware Initialising Phase End.\n", white);

	n = CHNOSProject_snprintf(s, "Test Text %X %u %f\n", 128, 0xfec37, 1234567890, 0.5);
	TextMode_Put_String(s, white);
	sprintf(s, "n=%d\n", n);
	TextMode_Put_String(s, white);

	n = sprintf(s, "Test Text %X %u\n", 0xfec37, 1234567890);
	TextMode_Put_String(s, white);
	sprintf(s, "n=%d\n", n);
	TextMode_Put_String(s, white);

	IO_STI();

	CHNOSProject_Set_SystemPhase(1);

	for (;;) {
		IO_HLT();
	}
}

void CHNOSProject_Set_SystemPhase(uint phase)
{
	SystemPhase = phase;
	TextMode_Put_String("\nNow SystemPhase is", white);
	TextMode_Put_String(SystemPhaseText[SystemPhase], skyblue);
	TextMode_Put_String("\n", white);

	return;
}
