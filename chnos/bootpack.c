
#include "core.h"

uint SystemPhase;

uchar *SystemPhaseText[] = {
	" 0:Initialising Hardware (Protected 32bit Text Mode)",
	" 1:Initialising Software (Protected 32bit Text Mode)"
};

void CHNMain(void)
{

	SystemPhase = 0;

	IO_CLI();

	IO_STI();

	TextMode_Clear_Screen();

	TextMode_Put_String("Welcome to CHNOSProject!\n", green);

	TextMode_Put_String("Now SystemPhase is", white);
	TextMode_Put_String(SystemPhaseText[SystemPhase], skyblue);
	TextMode_Put_String("\n", white);

	TextMode_Put_String("\tInitialising SerialPort...\n", white);
	Initialise_SerialPort();

	TextMode_Put_String("\tInitialising GDT...\n", white);
	Initialise_GlobalDescriptorTable();

	TextMode_Put_String("\tInitialising IDT...\n", white);
	Initialise_InterruptDescriptorTable();

	TextMode_Put_String("\tHardware Initialising Phase End.\n", white);

	SystemPhase++;

	TextMode_Put_String("Now SystemPhase is", white);
	TextMode_Put_String(SystemPhaseText[SystemPhase], skyblue);
	TextMode_Put_String("\n", white);
	
	for (;;) {
		IO_HLT();
	}
}
