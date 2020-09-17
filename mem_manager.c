#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define BLOCK_QTY 20
#define INITIALBLOCK '*'

typedef char bit;

void* testMalloc(size_t size);
void testFree(void* ptr);

char memory[BLOCK_SIZE * BLOCK_QTY] = {0};
bit bitmap[BLOCK_QTY] = {0};

void* my_malloc(size_t size) {
    if (size == 0)
        return NULL;

    int needed = (int)size / BLOCK_SIZE + (size % BLOCK_SIZE != 0 ? 1 : 0);  //habria que redondear para arriba!
    int counter = 0;
    int i, j;
    for (i = 0; i < BLOCK_QTY && counter < needed; i++) {
        if (bitmap[i] == 0)
            counter++;
        else
            counter = 0;
    }

    if (counter == needed) {
        int init_pos = i - needed;  //index of first block
        bitmap[init_pos] = INITIALBLOCK;
        for (j = 1; j < needed; j++)
            bitmap[init_pos + j] = 1;
        return (void*)(memory + (init_pos * BLOCK_SIZE));  //MAL, tendriamos que devolver needed cantidad de punteros (bloques)
    }
    return NULL;
}

void my_free(void* ptr) {
    if (ptr == NULL)
        return;
    int init = ((char*)ptr - memory) / BLOCK_SIZE;
    if (bitmap[init] == INITIALBLOCK) {
        do {
            bitmap[init++] = 0;
        } while (bitmap[init] != 0 && bitmap[init] != INITIALBLOCK);
    }
}

void initialize(void* base) {
    int i = 0;
    char* mem = (char*)base;
    while (i < BLOCK_SIZE * BLOCK_QTY)
        mem[i++] = 0;
}

void printBitmap() {
    int i;
    for (i = 0; i < BLOCK_QTY; i++) {
        if (bitmap[i] == 0)
            putchar('o');
        else
            putchar('x');
    }
    putchar('\n');
}

int main(void) {
    char* base = memory;
    initialize(memory);
    printf("Dir. Inicial de la mem:%p\n", base);
    char* arr1 = (char*)testMalloc(BLOCK_SIZE);
    strcpy(arr1, "Hello World!\n");
    char* arr2 = (char*)testMalloc(1024);
    char* arr3 = (char*)testMalloc(BLOCK_SIZE * 10);
    printf("%s", arr1);
    strcpy(arr2, "Concatenando strings\n");
    strcat(arr2, arr1);
    testFree(arr1);
    printf("%s", arr2);
    int* arr = (int*)testMalloc(sizeof(int) * 5);
    int i;
    for (i = 0; i < 5; i++) {
        arr[i] = i;
        printf("%d\n", arr[i]);
    }
    testFree(arr);
    char* arr4 = (char*)testMalloc(512);
    char* arr5 = (char*)testMalloc(BLOCK_SIZE * 10);
    strcpy(arr4, "Hello World!\n");
    printf("%s", arr4);
    testFree(arr4);
    testFree(arr2);
    testFree(arr3);
    arr5 = (char*)testMalloc(BLOCK_SIZE * 10);
    testFree(arr5);

    return 0;
}

void* testMalloc(size_t size) {
    char* arr1 = (char*)my_malloc(size);
    if (arr1 == NULL)
        printf("No se encontro slot de memoria\n");
    else
        printf("Dir. mem:%p\n", arr1);
    printBitmap();
    return arr1;
}

void testFree(void* ptr) {
    my_free(ptr);
    printBitmap();
}
