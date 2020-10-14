#include <stdio.h>
#include <math.h>

#define FREE      0 
#define OCCUPIED  1
#define SPLIT     2

#define MEM_SIZE        1048576 //2^20 = 1M
#define MEM_SIZE_EXP    20  

#define MIN_BLOCK_SIZE      4096 //2^12 = 4K
#define MIN_BLOCK_SIZE_EXP  12

#define MEM_FIRST_ADDRESS  0x600000 //EN NINGUN MOMENTO NOS ASEGURAMOS DE QUE LA MEM EN ESTAS POSICIONES NO ESTE OCUPADA

/*
#define NODES_QTY() {                                           \
  int suma = 0;                                                 \
  for( i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; i++){  \
    suma += 1 << i;                                             \
  }                                                             \
  return suma;                                                  \
} //no funca
*/


typedef struct node{ 
  int state;
  struct node *left; 
  struct node *right;
  struct node *parent;  //tal vez no lo usemos
  void *ptr;            //memory direction to be returned in malloc
  int exp;              //size = 2^exp
  int idx;              //indice donde esta guardada la estructura - mem[idx]
  int array_ptr_idx;     //indice de la lista en la que esta el nodo - block_arrays_ptrs[array_ptr_idx]
}node;


node mem[24600]; //Initialized in main function (NOT ANY MORE). 24600 = nodes_qty * sizeof(node) 
static node root = {FREE, mem+1, mem+2, NULL, (void *)MEM_FIRST_ADDRESS, MEM_SIZE_EXP, 0, 0}; //O LO DECLARAMOS COMO *node
void *block_arrays_ptrs[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1]; //Initialized in main function.
int block_arrays_free_spaces[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1] = {0};  //free nodes in level
int block_arrays_sizes[MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1] = {0};  //number of initialized nodes (free, split, occupied) in level



unsigned long nodes_qty(){
  unsigned long suma = 0;

  for(int i = 0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; i++)
    suma += 1 << i; //2^i

  return suma;
}


unsigned int exp_of_block_needed(unsigned int request){
  unsigned int exp = MIN_BLOCK_SIZE_EXP;
  unsigned int size = MIN_BLOCK_SIZE;

  while (size < request){
    exp++;
    size *= 2;
  }

  return exp;
}


unsigned int exp_of_block_needed2(unsigned int request) {
    int i;
    for (i = 0; 1 << i < request; i++)
        ;

    return i >= MIN_BLOCK_SIZE_EXP ? i : MIN_BLOCK_SIZE_EXP;
}


unsigned int get_idx_of_next_available_node(int array_idx){
  //Tener en cuenta que cuando se llama a esta funcion, ya se sabe que el array no esta vacio
  unsigned int ans = 0;

  while( block_arrays_ptrs[array_idx] + ans*sizeof(node) == NULL || ( (node *) (block_arrays_ptrs[array_idx] + ans*sizeof(node)) )->state != FREE){
    //if(block_arrays_ptrs[array_idx] + ans*sizeof(node) != NULL)
      //printf("NOT FREE %d - %d\n", ans, ( (node *) (block_arrays_ptrs[array_idx] + ans*sizeof(node)) )->state) ;
    ans++;
}
  //printf("ANS %d\n", ans);

  return ans;
}


void split_node(node *n){ //REVISAR ESTA FUNCION (PUNTEROS, DIRECCIONES, ETC) !!!
  //como inicializamos los hijos???
  int i = n->idx, idx_aux = 2*i+1, child_arr_ptr_idx = n->array_ptr_idx + 1;
            printf("oki %d %d\n", n->state, i);

  mem[2*i + 1] = *n->left;
  mem[2*i + 2] = *n->right;
  
  n->left->state = FREE;
  n->left->left = mem + (2*idx_aux+1);
  n->left->right = mem + (2*idx_aux+2);
  n->left->parent = n;
  n->left->ptr = n->ptr;  //chequear direccion de inicio

  //printf("ptr left %ld\n", (long) n->left->ptr);

  n->left->exp = n->exp-1;
  n->left->idx = idx_aux;
  n->left->array_ptr_idx = child_arr_ptr_idx;
  
  idx_aux = 2*i+2;


  n->right->state = FREE;
  n->right->left = mem + (2*idx_aux+1);
  n->right->right = mem + (2*idx_aux+2);
  n->right->parent = n;
  n->right->ptr = n->ptr + (1 << (n->exp-1));  //left node begin address + size of (left) block
  
  //printf("ptr right %ld\n", (long) n->right->ptr);

  //chequear direccion de inicio
  n->right->exp = n->exp-1;
  n->right->idx = idx_aux;
  n->right->array_ptr_idx = child_arr_ptr_idx;


  i = get_idx_of_next_available_node(child_arr_ptr_idx);
  //block_arrays_ptrs[child_arr_ptr_idx] + i = (void*) n->left;
  void *p = block_arrays_ptrs[child_arr_ptr_idx];
  p += i*sizeof(node);
  p = (void*)n->left;
  
  i = get_idx_of_next_available_node(child_arr_ptr_idx);
  //block_arrays_ptrs[child_arr_ptr_idx] + i = (void*) n->right;
  p = block_arrays_ptrs[child_arr_ptr_idx];
  p += i*sizeof(node);
  p = (void*)n->right;

  n->state = SPLIT;

}





//mem {0: 2^0, 1: 2^1, 2: 2^2} 2^5 -> 2^0+2^1+2^2+2^3+2^4 hasta 2^0+2^1+2^2+2^3+2^4+2^5-1

void *buddy_malloc(unsigned int size_request){
  unsigned int exp_request = exp_of_block_needed(size_request); //We need a block of size 2^exp
  //unsigned int current_exp = MEM_SIZE_EXP;
  //node *current_node = &root; //no se si efectivamente apuntan a lo mismo
  unsigned int idx = MEM_SIZE_EXP - exp_request; //Para recorrer los arreglos de bloques con tamano >= 2^exp_request

  if(exp_request > MEM_SIZE_EXP)
    return NULL;

  while(idx > 0 && block_arrays_free_spaces[idx] == 0)
    idx--;

  printf("IDX --> %d\n", idx);

  if(block_arrays_free_spaces[idx] == 0)
    return NULL;  //idx=0 ==> todas las listas estaban vacias
  
  while(idx < MEM_SIZE_EXP - exp_request && block_arrays_sizes[idx+1] < pow(2, idx)){   //pow(2, idx) = number of nodes at idx level
    unsigned int i = get_idx_of_next_available_node(idx);
            printf("idx %d - offset %d\n", idx, i);

    split_node( (node*) (block_arrays_ptrs[idx] + i*sizeof(node)) );

    block_arrays_free_spaces[idx]--;
    idx++;
    block_arrays_free_spaces[idx]+=2;
    block_arrays_sizes[idx]+=2;

  }

  printf("IDX1 --> %d\n", idx);


  unsigned int i = get_idx_of_next_available_node(idx);
              printf("idx %d - offset %d\n", idx, i);
  ( (node*) (block_arrays_ptrs[idx] + i*sizeof(node)) )->state = OCCUPIED;
  block_arrays_free_spaces[idx]--;


  return ( (node*) (block_arrays_ptrs[idx] + i*sizeof(node)) )->ptr;

}


void buddy_free(void *ptr){
  node *n;
  int i,j, found = 0, stop = 0, k, aux;

  for(i=0; i < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1 /*&& !found*/; i++){
    printf("i = %d - size = %d\n", i, block_arrays_free_spaces[i]);
    if(block_arrays_sizes[i] > 0){ //no recorrer el array innecesariamente  
      
      for(j=0; j < pow(2, i) /*&& !found*/; j++){
       
        n = (node *) block_arrays_ptrs[i] + j*sizeof(node);
        if( n != NULL && n->ptr == ptr && n->state == OCCUPIED ){
          printf("%d found!!!!!!!!\n", i);
          found = 1;
          n->state = FREE;
          block_arrays_free_spaces[i]++;
        }

      }
    }
  }

  /* Checks if its brother is free and joins blocks (once joined its parent does the same). */
  if(found){
    while(i>0 && stop){
      k = j + (j%2 == 0 ? 1 : -1);  //k = brother_index
      if( ( (node *) block_arrays_ptrs[i] + k*sizeof(node) )->state == FREE){
        // block_arrays_ptrs[i] + k*sizeof(node) = NULL;
        // block_arrays_ptrs[i] + j*sizeof(node) = NULL;
        // block_arrays_free_spaces[i]-=2;
        // block_arrays_sizes[i]-=2;
        // aux = (k%2 == 0 ? 1 : 2); //parent_index = (k-aux)/2
        // block_arrays_ptrs[i-1] + (k-aux)/2*sizeof(node) = mem[(k-aux)/2]; 
        // block_arrays_free_spaces[i-1]++;
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
          printf("%ld %ld %ld\n", size, sizeof(node), nodes_qty());
  //*mem = *aux;

  /* Set first node */
  mem[0] = root;

  /* Pointers initialization */
  for(int level=0, j; level < MEM_SIZE_EXP - MIN_BLOCK_SIZE_EXP + 1; level++){
    j = get_idx_of_first_node_in_level(level);
          printf("Level %d - idx %d\n", level, j);
    block_arrays_ptrs[level] = &mem[j]; //mem + (j * sizeof(node))
  }
  block_arrays_free_spaces[0] = 1;
  block_arrays_sizes[0] = 0;

  /* TESTS */
  for(int size = MEM_SIZE+1000; size >= 512; size/=2){
    //printf("%d (%d) (%d)\n", size, exp_of_block_needed(size), exp_of_block_needed2(size));
  }
  
  void *a = buddy_malloc(4096);
  printf("----------\n");
  void *b = buddy_malloc(4096);
  printf("----------\n");
  void *c = buddy_malloc(4096);
  printf("----------\n");
  void *d = buddy_malloc(4096);

  printf("%ld - %ld - %ld - %ld\n", (long)a, (long)b, (long)c, (long)d);

  buddy_free(a);
  printf("----------\n");
  a = buddy_malloc(4096);
  printf("----------\n");
  void *e = buddy_malloc(4096);

  printf("%ld - %ld\n", (long)a, (long)e);

  return 0;
}