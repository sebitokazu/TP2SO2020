#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);
int intToStr(int x, char str[]);
char *strcpy(char *destination, const char *source);
int my_strlen(const char *s);
int strcmp(const char *s1, const char *s2);

char *cpuVendor(char *result);

#endif