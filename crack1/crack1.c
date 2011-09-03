
#include "../chnapi.h"

void CHNMain(void)
{
	api_putchar('C');
	*((uchar *)0x00102600) = 0;
	api_end();
}
