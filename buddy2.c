#include <stdio.h>
#include <math.h>

#define FREE      0 
#define OCCUPIED  1
#define SPLIT     2

#define MEM_SIZE        1048576 //2^20 = 1M
#define MEM_SIZE_EXP    20  

#define MIN_BLOCK_SIZE      4096 //2^12 = 4K
#define MIN_BLOCK_SIZE_EXP  12

#define MEM_FIRST_ADDRESS  0x600000 //esta bien la dir?

#define NODES_QTY() {                                           \
  int suma = 0;                                                 \
  for( i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; i++){  \
    suma += 1 << i;                                             \
  }                                                             \
  return suma;                                                  \
} //no funca



typedef struct node{ 
  int state;
  struct node *left; 
  struct node *right;
  struct node *parent;  //tal vez no lo usemos
  void *ptr;
  int exp; //size = 2^exp
  int idx;
}node;


static node mem[]; //Initialized in main function.
static node root = {FREE, NULL, NULL, NULL, MEM_FIRST_ADDRESS, MEM_SIZE_EXP, 0}; //O LO DECLARAMOS COMO *node
void *block_arrays_ptrs[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1]; //Initialized in main function.
int block_arrays_sizes[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1] = {0};


unsigned long nodes_qty(){
  unsigned long suma = 0;

  for(int i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; i++)
    suma += 1 << i; //2^i

  return suma;
}


unsigned int exp_of_block_needed(unsigned int request){
  unsigned int exp = MEM_SIZE_EXP; //-1 ?
  unsigned int size = MIN_BLOCK_SIZE;

  while (size < request){
    exp--;
    size *= 2;
  }

  return exp;
}


void split_node(node *n){ //REVISAR ESTA FUNCION (PUNTEROS, DIRECCIONES, ETC) !!!
  //como inicializamos los hijos???
  int i = n->idx;
  *n->left = mem[2*i + 1];
  *n->right = mem[2*i + 2];
  
  (*n->left).state = FREE;
  (*n->left).left = NULL;
  (*n->left).right = NULL;
  (*n->left).parent = n;
  (*n->left).ptr = n->ptr;  //chequear direccion de inicio
  (*n->left).exp = n->exp-1;
  (*n->left).idx = 2*i+1;
  
  (*n->right).state = FREE;
  (*n->right).left = NULL;
  (*n->right).right = NULL;
  (*n->right).parent = n;
  (*n->right).ptr = n->ptr + (1 << (n->exp-1));  //left node begin address + size of (left) block
  //chequear direccion de inicio
  (*n->right).exp = n->exp-1;
  (*n->right).idx = 2*i+2;
  
  n->state = SPLIT;
}


unsigned int get_idx_of_next_available_node(int array_idx){
  //Tener en cuenta que cuando se llama a esta funcion, ya se sabe que el array no esta vacio
  unsigned int ans = 0;

  while( block_arrays_ptrs[array_idx] + ans == NULL || ( (node *) (block_arrays_ptrs[array_idx] + ans) )->state != FREE)
    ans++;
  
  return ans;
}


//mem {0: 2^0, 1: 2^1, 2: 2^2} 2^5 -> 2^0+2^1+2^2+2^3+2^4 hasta 2^0+2^1+2^2+2^3+2^4+2^5-1

void *buddy_malloc(unsigned int size_request){
  unsigned int exp_request = exp_of_block_needed(size_request); //We need a block of size 2^exp
  //unsigned int current_exp = MEM_SIZE_EXP;
  //node *current_node = &root; //no se si efectivamente apuntan a lo mismo
  unsigned int idx = MEM_SIZE_EXP - exp_request; //Para recorrer los arreglos de bloques con tamano >= 2^exp_request

  if(exp_request > MEM_SIZE_EXP)
    return NULL;

  while(idx > 0 && block_arrays_sizes[idx] == 0)
    idx--;

  if(block_arrays_sizes[idx] == 0)
    return NULL;  //idx=0 ==> todas las listas estaban vacias
  
  while(idx < MEM_SIZE_EXP - exp_request){
    unsigned int i = get_idx_of_next_available_node(idx);
    split_node( (node*) (block_arrays_ptrs[idx] + i) );
    block_arrays_sizes[idx]--;
    idx++;
    block_arrays_sizes[idx]+=2;
  }

  unsigned int i = get_idx_of_next_available_node(idx);
  ( (node*) (block_arrays_ptrs[idx] + i) )->state = OCCUPIED;

  return ( (node*) (block_arrays_ptrs[idx] + i) )->ptr;

}


void buddy_free(void *ptr){
  node *n;
  int i,j, found = 0, stop = 0, k, aux;

  for(i=0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1 && !found; i++){
    
    if(block_arrays_sizes[i] > 0){ //no recorrer el array innecesariamente  
      
      for(j=0; j < pow(2, i) && !found; j++){
       
        n = (node *) block_arrays_ptrs[i] + j;
        if( n != NULL && n->ptr == ptr ){
          found = 1;
          n->state = FREE;
          block_arrays_sizes[i]++;
        }

      }
    }
  }

  /* Checks if its brother is free and joins blocks (once joined its parent does the same). */
  if(found){
    while(i>0 && stop){
      k = j + (j%2 == 0 ? 1 : -1);  //k = brother_index
      if( ( (node *) block_arrays_ptrs[i] + k )->state == FREE){
        block_arrays_ptrs[i] + k = NULL;
        block_arrays_ptrs[i] + j = NULL;
        block_arrays_sizes[i]-=2;
        aux = (k%2 == 0 ? 1 : 2); //parent_index = (k-aux)/2
        block_arrays_ptrs[i-1] + (k-aux)/2 = mem[(k-aux)/2]; 
        block_arrays_sizes[i-1]++;
      }
      else
        stop = 1;
    }
  }

}


unsigned int get_idx_of_first_node_in_level(unsigned int level){
  unsigned int idx=0;
  
  for(int j=0; j<level; j++)
    idx += pow(2, j);

  return idx;
}

int main(){
  //o inicializar root aca?

  /* Memory for nodes initialization */
  long size = sizeof(node) * nodes_qty();
  node aux[size];
  *mem = *aux;

  /* Set first node */
  mem[0] = root;

  /* Pointers initialization */
  for(int level=0, j; level < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; level++){
    j = get_idx_of_first_node_in_level(level);
    block_arrays_ptrs[level] = mem + (j * sizeof(node));
  }
  block_arrays_sizes[0] = 1;

  return 0;
}