#include <stdint.h>

void *memset(void *destination, int32_t c, uint64_t length) {
    uint8_t chr = (uint8_t)c;
    char *dst = (char *)destination;

    while (length--)
        dst[length] = chr;

    return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
    /*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
    uint64_t i;

    if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
        (uint64_t)source % sizeof(uint32_t) == 0 &&
        length % sizeof(uint32_t) == 0) {
        uint32_t *d = (uint32_t *)destination;
        const uint32_t *s = (const uint32_t *)source;

        for (i = 0; i < length / sizeof(uint32_t); i++)
            d[i] = s[i];
    } else {
        uint8_t *d = (uint8_t *)destination;
        const uint8_t *s = (const uint8_t *)source;

        for (i = 0; i < length; i++)
            d[i] = s[i];
    }

    return destination;
}
void reverse(char *str, int len) {
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[]) {
    int i = 0;
    if (x != 0) {
        while (x) {
            str[i++] = (x % 10) + '0';
            x = x / 10;
        }
        reverse(str, i);
        str[i] = '\0';
    } else {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    return i;
}

// Function to implement strcpy() function
char *strcpy(char *destination, const char *source) {
    // return if no memory is allocated to the destination
    if (destination == 0)
        return (char *)0;

    // take a pointer pointing to the beginning of destination string
    char *ptr = destination;

    // copy the C-string pointed by source into the array
    // pointed by destination
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }

    // include the terminating null character
    *destination = '\0';

    // destination is returned by standard strcpy()
    return ptr;
}

int my_strlen(const char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++)
        ;
    return i;
}

int strcmp(const char *X, const char *Y) {
    while (*X) {
        // if characters differ or end of second string is reached
        if (*X != *Y)
            break;

        // move to next pair of characters
        X++;
        Y++;
    }

    // return the ASCII difference after converting char* to unsigned char*
    return *(const unsigned char *)X - *(const unsigned char *)Y;
}