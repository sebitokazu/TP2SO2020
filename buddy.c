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
}node;


static node mem[]; //Initialized in main function.
static node root = {FREE, NULL, NULL, NULL, MEM_FIRST_ADDRESS, MEM_SIZE_EXP}; //O LO DECLARAMOS COMO *node

unsigned long nodes_qty(){
  long suma = 0;

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

unsigned int look_for_space(){
  int i = 0;
  
  //ESTA BIEN mem+i != NULL?
  while(mem+i != NULL && i < nodes_qty()) //es necesaria la segunda validacion?
    i++;

  return i;
}

void split_node(node *n){ //REVISAR ESTA FUNCION (PUNTEROS, DIRECCIONES, ETC) !!!
  //como inicializamos los hijos???
  unsigned int i = look_for_space();
  *n->left = mem[i];  //esta bien como asignamos la dir?
  i = look_for_space();
  *n->right = mem[i]; //esta bien como asignamos la dir?
  
  (*n->left).state = FREE;
  (*n->left).left = NULL;
  (*n->left).right = NULL;
  (*n->left).parent = n;
  (*n->left).ptr = n->ptr;  //chequear direccion de inicio
  (*n->left).exp = n->exp-1;
  
  (*n->right).state = FREE;
  (*n->right).left = NULL;
  (*n->right).right = NULL;
  (*n->right).parent = n;
  (*n->right).ptr = n->ptr + (1 << (n->exp-1));  //left node begin address + size of (left) block
  //chequear direccion de inicio
  (*n->right).exp = n->exp-1;
  
  n->state = SPLIT;
}

void *buddy_malloc_recursive(node *n, unsigned int exp_request){
  //n->exp < exp_request cannot happen.

  if(n->exp == exp_request){
    if(n->state == FREE)
      return n->ptr;
    else
      return NULL;
  }

  //n->exp > exp_request
  switch(n->state){
    case FREE:
      split_node(n);
      break;

    case OCCUPIED:
      return NULL;

    case SPLIT: /* Revisar, no siempre hay que ir para la izquierda, tal vez tu hermano ya esta splitted. */
      void *aux = buddy_malloc_recursive(n->left, exp_request);
      if(aux != NULL)
        return aux;
      return buddy_malloc_recursive(n->right, exp_request); 
      
    default: //impossible
      return NULL;
  }
  
  return NULL;  //impossible
}

void *buddy_malloc(unsigned int size_request){
  unsigned int exp_request = exp_of_block_needed(size_request); //We need a block of size 2^exp
  //unsigned int current_exp = MEM_SIZE_EXP;
  //node *current_node = &root; //no se si efectivamente apuntan a lo mismo
  
  if(exp_request > MEM_SIZE_EXP)
    return NULL;

  return buddy_malloc_recursive(&root, exp_request);
}

int main(){
  //o inicializar root aca?

  /* Memory for nodes initialization */
  long size = sizeof(node) * nodes_qty();
  node aux[size];
  *mem = *aux;

  return 0;
}