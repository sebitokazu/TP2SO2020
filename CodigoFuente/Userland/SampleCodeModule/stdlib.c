#include <stdlib.h>

static double pot(int num, int pot);

int strlen(char* string) {
    int i;
    for (i = 0; string[i] != 0; i++)
        ;
    return i;
}

int strcmp(char* s1, char* s2) {
    int match = 1;
    int i;
    for (i = 0; s1[i] != 0 && s2[i] != 0 && match; i++) {
        if (s1[i] != s2[i])
            match = 0;
    }
    if (s1[i] != 0 || s2[i] != 0)
        match = 0;
    return match;
}

/*Receives a pointer to pointer where num starts and returns it as a double, having increased the pointer till it ends*/
double stringToDouble(char* c) {
    if (c[0] == '-') return (stringToDouble(c + 1)) * -1;  // check if it is neg
    /*Only positive numbers get to here*/
    double tempDouble = 0;
    char tempChar = c[0];
    int i = 1;
    while (tempChar != '.' && tempChar != ' ' && tempChar != 0) {
        tempDouble = (double)(tempDouble * 10 + charToNum(tempChar));
        tempChar = c[i++];
    }
    if (tempChar != '.') return tempDouble;
    double tempAux;
    tempChar = c[i++];
    for (int d = 1; tempChar != ' ' && tempChar != 0; d++) {
        tempAux = (double)charToNum(tempChar);
        int t = d;
        while (t--)
            tempAux /= 10;
        tempDouble += tempAux;
        tempChar = c[i++];
    }
    return tempDouble;
}

int stringToInt(char* str) {
    // Initialize result
    int res = 0;

    // Initialize sign as positive
    int sign = 1;

    // Initialize index of first digit
    int i = 0;

    // If number is negative,
    // then update sign
    if (str[0] == '-') {
        sign = -1;

        // Also update index of first digit
        i++;
    }

    // Iterate through all digits
    // and update the result
    for (; str[i] != '\0'; i++)
        res = (res * 10 + (str[i] - '0'));

    // Return result with sign
    return sign * res;
}

double charToNum(char c) {
    return (int)c - 48;
}

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len) {
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
//Generates a string of an integer
/*
    From: https://www.geeksforgeeks.org/implement-itoa/
*/
int itoa(int x, char str[]) {
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (x == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return 1;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (x < 0) {
        isNegative = 1;
        x = -x;
    }

    // Process individual digits
    while (x != 0) {
        int rem = x % 10;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        x = x / 10;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';  // Append string terminator

    // Reverse the string
    reverse(str, i);

    return i;
}
//Generates a string of an integer
int intToStr(int x, char str[]) {
    int i = 0;
    //Check if its negative
    int neg = 0;
    if (x < 0) {
        i++;
        neg = 1;
        x *= -1;
    }
    if (x != 0) {
        while (x) {
            str[i++] = (x % 10) + '0';
            x = x / 10;
        }
        if (neg)
            str[i++] = '-';
        reverse(str, i);
        str[i] = '\0';
    } else {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    return i;
}

//https://www.geeksforgeeks.org/convert-floating-point-number-string/
void floatToString(float n, char* res, int afterpoint) {
    int neg = 0;

    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;
    if (fpart < 0) fpart *= -1;

    //Check if its negative
    if (n < 0) {
        ipart *= -1;
        neg = 1;
        *res = '-';
    }

    // convert integer part to string
    int i = intToStr(ipart, neg ? res + 1 : res);
    if (neg) i++;
    if (fpart > 0.01) {
        if (afterpoint != 0) {
            res[i++] = '.';  // add dot
            int k = 0;
            while (fpart < 0.1) {
                res[i++] = '0';
                fpart *= 10;
                k++;
            }
            fpart = fpart * pot(10, afterpoint - k);
            intToStr((int)fpart, res + i);
        }
    }
}

static double pot(int num, int pot) {
    double toRet = 1;
    while (pot--)
        toRet *= num;
    return toRet;
}
