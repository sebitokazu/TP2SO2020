
#ifndef __IDT_LOADER_H_
#define __IDT_LOADER_H_
#include <stdint.h>

void load_idt();
void setup_IDT_entry(int index, uint64_t offset);

#endif
