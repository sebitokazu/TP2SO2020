#include <time.h>
#include <nahiveConsole.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
    ncPrintChar('.');
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}
