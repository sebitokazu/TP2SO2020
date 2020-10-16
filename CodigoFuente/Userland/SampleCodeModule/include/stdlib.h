#ifndef stdlib_h
#define stdlib_h

double stringToDouble(char* c);

double charToNum(char c);

int strlen(char* string);

int strcmp(char* s1, char* s2);

void reverse(char* str, int len);

int intToStr(int x, char str[]);

int itoa(int x, char str[]);

void floatToString(float n, char* res, int afterpoint);

int stringToInt(char* str);

#endif /* stdlib_h */
