#include <stdlib.h>

static double pot(int num, int pot);


int strlen(char *string){
    int i;
    for(i=0; string[i] != 0; i++);
    return i;
}

int strcmp(char* s1, char* s2){
    int match=1;
    int i;
    for(i=0; s1[i] != 0 && s2[i] != 0 && match; i++){
        if(s1[i] != s2[i])
            match=0;
    }
    if(s1[i] != 0 || s2[i] != 0 )
        match=0;
    return match;
}


/*Receives a pointer to pointer where num starts and returns it as a double, having increased the pointer till it ends*/
double stringToDouble(char *c){
    if(c[0] == '-') return (stringToDouble(c+1)) * -1; // check if it is neg
    /*Only positive numbers get to here*/
    double tempDouble = 0;
    char tempChar=c[0];
    int i=1;
    while(tempChar != '.' && tempChar != ' ' && tempChar != 0){
        tempDouble = (double) (tempDouble*10 + charToNum(tempChar));
        tempChar = c[i++];
    }
    if(tempChar != '.') return tempDouble;
    double tempAux;
    tempChar = c[i++];
    for(int d=1; tempChar != ' ' && tempChar != 0; d++){
        tempAux = (double) charToNum(tempChar);
        int t=d;
        while(t--)
            tempAux /= 10;
        tempDouble += tempAux;
        tempChar = c[i++];
    }
    return tempDouble;
}

double charToNum(char c){
    return (int)c-48;
}


// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
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
int intToStr(int x, char str[])
{
    int i = 0;
    if(x != 0){
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    reverse(str, i);
    str[i] = '\0';
    }
    else{
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    return i;
}

//https://www.geeksforgeeks.org/convert-floating-point-number-string/
void floatToString(float n, char* res, int afterpoint)
{
    int neg = 0;
    
    // Extract integer part
    int ipart = (int)n;
    
    // Extract floating part
    float fpart = n - (float)ipart;
    if(fpart < 0) fpart *= -1;
    
    //Check if its negative
    if(n<0){
        ipart *= -1;
        neg = 1;
        *res = '-';
    }
    
    // convert integer part to string
    int i = intToStr(ipart, neg ? res+1 : res);
    if(neg) i++;
    if(fpart > 0.01){
        if(afterpoint != 0) {
            res[i++] = '.'; // add dot
            int k=0;
            while(fpart<0.1){
                res[i++]='0';
                fpart*=10;
                k++;
            }
            fpart = fpart * pot(10, afterpoint-k);
            intToStr((int)fpart, res + i );
        }
        
    }
}



static double pot(int num, int pot){
    double toRet = 1;
    while(pot--)
        toRet *= num;
    return toRet;
}




