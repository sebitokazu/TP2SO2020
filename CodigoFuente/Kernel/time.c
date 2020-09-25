#include <time.h>
#include <naiveConsole.h>
#include "video_driver.h"
#include "scheduler.h"

extern void _scheduler();

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
    if(ticks == 9){
        twinkle();
        ticks = 0;
    }
    drawWord("timer");
    _scheduler();
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}
