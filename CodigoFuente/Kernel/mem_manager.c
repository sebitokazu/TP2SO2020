#include "mem_manager.h"

#include "lib.h"
#ifdef IMP_BITMAP
#define BLOCK_SIZE 1024
#define BLOCK_QTY 1024

typedef int bit;

char *memory = 0x600000;

bit bitmap[BLOCK_QTY] = {0};
int blocksUsed = 0;

void *my_malloc(unsigned int size) {
    if (size == 0)
        return NULL;

    int needed = (int)size / BLOCK_SIZE + (size % BLOCK_SIZE != 0 ? 1 : 0);
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
        bitmap[init_pos] = needed;
        blocksUsed += needed;
        for (j = 1; j < needed; j++)
            bitmap[init_pos + j] = 1;
        return (void *)(memory + (init_pos * BLOCK_SIZE));
    }
    return NULL;
}

void my_free(void *ptr) {
    if (ptr == NULL || ptr < memory)
        return;
    int init = ((char *)ptr - memory) / BLOCK_SIZE;
    if (bitmap[init] > 0) {
        int toFreeBlocks = bitmap[init];
        blocksUsed -= bitmap[init];
        for (; toFreeBlocks > 0; toFreeBlocks--)
            bitmap[init++] = 0;
    }
}

void initialize_memory() {
    int i = 0;
    char *mem = memory;
    while (i < BLOCK_SIZE * BLOCK_QTY)
        mem[i++] = 0;
}

void checkMemoryStatus() {
    drawWord("Total memory: 1048576 bytes");
    jumpLine();
    drawWord("Currently, you are using: ");
    long bytesQty = blocksUsed * 1024;
    char buffer[30];
    intToStr(bytesQty, buffer);
    drawWord(buffer);
    jumpLine();
}
#endif
#ifdef IMP_BUDDY

#define UNDEFINED 0
#define FREE 1
#define OCCUPIED 2
#define SPLIT 3

#define MEM_SIZE 1048576  //2^20 = 1M
#define MEM_SIZE_EXP 20

#define MIN_BLOCK_SIZE 4096  //2^12 = 4K
#define MIN_BLOCK_SIZE_EXP 12

#define MEM_FIRST_ADDRESS 0x600000  

void printmem2(char *s);
void test_malloc1();
void test_block_join_after_free();

typedef struct node {
    int state;
    struct node *left;
    struct node *right;
    struct node *parent;
    void *ptr;            //memory direction to be returned in malloc
    int exp;              //size = 2^exp
    int idx;              //indice donde esta guardada la estructura - mem[idx]
    int array_ptr_idx;    //indice de la lista en la que esta el nodo - block_arrays_ptrs[array_ptr_idx]
} node;

node mem[24600];                                                                                   //Initialized in main function (NOT ANY MORE). 24600 = nodes_qty * sizeof(node)
static node root = {FREE, &mem[1], &mem[2], NULL, (void *)MEM_FIRST_ADDRESS, MEM_SIZE_EXP, 0, 0}; 
void *block_arrays_ptrs[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1];                                    //Initialized in main function.
int block_arrays_free_spaces[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1] = {0};                         //free nodes in level
int block_arrays_sizes[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1] = {0};                               //number of initialized nodes (free, split, occupied) in level

/* Function to calculate x raised to the power y in O(logn)
    From: https://www.geeksforgeeks.org/write-a-c-program-to-calculate-powxn/
*/
int pow(int x, unsigned int y) {
    int temp;
    if (y == 0)
        return 1;
    temp = pow(x, y / 2);
    if (y % 2 == 0)
        return temp * temp;
    else
        return x * temp * temp;
}

unsigned long nodes_qty() {
    unsigned long suma = 0;

    for (int i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; i++)
        suma += 1 << i;  //2^i

    return suma;
}

unsigned int exp_of_block_needed(unsigned int request) {
    int i;
    for (i = 0; 1 << i < request; i++)
        ;

    return i >= MIN_BLOCK_SIZE_EXP ? i : MIN_BLOCK_SIZE_EXP;
}

unsigned int get_idx_of_next_available_node(int array_idx) {
    //Tener en cuenta que cuando se llama a esta funcion, ya se sabe que el array no esta vacio
    unsigned int ans = 0;

    while (block_arrays_ptrs[array_idx] + ans * sizeof(node) == NULL || ((node *)(block_arrays_ptrs[array_idx] + ans * sizeof(node)))->state != FREE) {
        ans++;
    }

    return ans;
}

void split_node(node *n) { 
    int i = n->idx, idx_aux = 2 * i + 1, child_arr_ptr_idx = n->array_ptr_idx + 1;

    mem[2 * i + 1] = *n->left;
    mem[2 * i + 2] = *n->right;

    n->left->state = FREE;
    n->left->left = &mem[2 * idx_aux + 1];
    n->left->right = &mem[2 * idx_aux + 2];
    n->left->parent = n;
    n->left->ptr = n->ptr; 

    n->left->exp = n->exp - 1;
    n->left->idx = idx_aux;
    n->left->array_ptr_idx = child_arr_ptr_idx;

    idx_aux = 2 * i + 2;

    n->right->state = FREE;
    n->right->left = &mem[2 * idx_aux + 1];
    n->right->right = &mem[2 * idx_aux + 2];
    n->right->parent = n;
    n->right->ptr = n->ptr + (1 << (n->exp - 1));  //left node begin address + size of (left) block

    
    n->right->exp = n->exp - 1;
    n->right->idx = idx_aux;
    n->right->array_ptr_idx = child_arr_ptr_idx;

    i = get_idx_of_next_available_node(child_arr_ptr_idx);
    void *p = block_arrays_ptrs[child_arr_ptr_idx];
    p += i * sizeof(node);
    p = (void *)n->left;

    i = get_idx_of_next_available_node(child_arr_ptr_idx);
    p = block_arrays_ptrs[child_arr_ptr_idx];
    p += i * sizeof(node);
    p = (void *)n->right;

    n->state = SPLIT;
}

//mem {0: 2^0, 1: 2^1, 2: 2^2} 2^5 -> 2^0+2^1+2^2+2^3+2^4 hasta 2^0+2^1+2^2+2^3+2^4+2^5-1

void *my_malloc(unsigned int size_request) {
    unsigned int exp_request = exp_of_block_needed(size_request);  //We need a block of size 2^exp
    unsigned int idx = MEM_SIZE_EXP - exp_request;  //Para recorrer los arreglos de bloques con tamano >= 2^exp_request

    if (exp_request > MEM_SIZE_EXP)
        return NULL;

    while (idx > 0 && block_arrays_free_spaces[idx] == 0)
        idx--;

    if (block_arrays_free_spaces[idx] == 0)
        return NULL;  //idx=0 ==> todas las listas estaban vacias

    while (idx < MEM_SIZE_EXP - exp_request && block_arrays_sizes[idx + 1] < pow(2, idx + 1)) {  //pow(2, idx) = number of nodes at idx level
        unsigned int i = get_idx_of_next_available_node(idx);

        split_node((node *)(block_arrays_ptrs[idx] + i * sizeof(node)));

        block_arrays_free_spaces[idx]--;
        idx++;
        block_arrays_free_spaces[idx] += 2;
        block_arrays_sizes[idx] += 2;
    }

    unsigned int i = get_idx_of_next_available_node(idx);
    node *n = (node *)(block_arrays_ptrs[idx] + i * sizeof(node));
    n->state = OCCUPIED;
    block_arrays_free_spaces[idx]--;

    return n->ptr;
}

void my_free(void *ptr) {
    node *n, *bro, *parent;
    int i, j, found = 0, stop = 0, k;

    for (i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1 && !found; i++) {
        if (block_arrays_sizes[i] > 0) {  //no recorrer el array innecesariamente

            for (j = 0; j < pow(2, i) && !found; j++) {
                n = (node *)(block_arrays_ptrs[i] + j * sizeof(node));

                if (n != NULL && n->ptr == ptr && n->state == OCCUPIED) {
                    found = 1;
                    n->state = FREE;
                    block_arrays_free_spaces[i]++;
                }
            }
        }
    }

    /* Checks if its brother is free and joins blocks (once joined its parent does the same). */
    if (found) {
        j--;
        i--;
        while (i > 0 && !stop) {
            k = j + (j % 2 == 0 ? 1 : -1);  //k = brother_index, j = node_idx

            bro = (node *)(block_arrays_ptrs[i] + k * sizeof(node));

            if (bro != NULL && bro->state == FREE) {
                

                n = (node *)(block_arrays_ptrs[i] + j * sizeof(node));
                // bro already assigned

                j = (j % 2 == 0 ? j / 2 : k / 2);  //j = parent_idx

                parent = (node *)(block_arrays_ptrs[i - 1] + j * sizeof(node));
                //parent = &mem[(k-aux)/2];
                parent->ptr = (k % 2 == 0 ? bro->ptr : n->ptr);  //parent->ptr = left->ptr;
                parent->state = FREE;

                n->state = UNDEFINED;
                n->ptr = 0;
                bro->state = UNDEFINED;
                bro->ptr = 0;

                //Two blocks at level i are repalced for one block at level i-1
                block_arrays_free_spaces[i] -= 2;
                block_arrays_sizes[i] -= 2;
                block_arrays_free_spaces[i - 1]++;
                block_arrays_sizes[i - 1]++;


                i--;  //So it can check if its uncle is free
                      // Remember that j = parent_idx;

            } else
                stop = 1;
        }
    }
}

unsigned int get_idx_of_first_node_in_level(unsigned int level) {
    unsigned int idx = 0;

    for (int j = 0; j < level; j++)
        idx += pow(2, j);

    return idx;
}

void checkMemoryStatus() {
    int i, j;
    long occupied = 0;
    node *n;
    for (i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1 && occupied < 1048576; i++) {
        if (block_arrays_sizes[i] > 0) {  //no recorrer el array innecesariamente

            for (j = 0; j < pow(2, i) && occupied < 1048576; j++) {
                n = (node *)(block_arrays_ptrs[i] + j * sizeof(node));

                if (n != NULL && n->state == OCCUPIED) {
                    occupied += pow(2, MEM_SIZE_EXP - i);
                }
            }
        }
    }
    drawWord("Total memory: 1048576 bytes");
    jumpLine();
    drawWord("Currently, you are using: ");
    char buffer[30];
    intToStr(occupied, buffer);
    drawWord(buffer);
    jumpLine();
}

void initialize_memory() {
    /* Set first node */
    mem[0] = root;

    /* Pointers initialization */
    for (int level = 0, j; level < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; level++) {
        j = get_idx_of_first_node_in_level(level);
        block_arrays_ptrs[level] = &mem[j];  //mem + (j * sizeof(node))
    }
    block_arrays_free_spaces[0] = 1;
    block_arrays_sizes[0] = 1;
}
#endif