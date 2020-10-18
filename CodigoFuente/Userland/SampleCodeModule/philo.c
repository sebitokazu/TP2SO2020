/*
Adaptacion de la implementacion obtenida en:
https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/
*/

#include "philo.h"

int state[MAX_PHILOS];	//malloc
int phil[N] = { 0, 1, 2, 3, 4 };	//CREO QUE NO ES NECESARIO

int philos_number = N;
int mutex; //not used
char S[MAX_PHILOS][5];	//malloc

void sleep(int n) {
    long i;
	n *= 1000000;
    for (i = 0; i < n; i++)
        ;
}

void test(int phnum) 
{ 
	if (state[phnum] == HUNGRY 
		&& state[LEFT] != EATING 
		&& state[RIGHT] != EATING) { 
		// state that eating 
		state[phnum] = EATING;
		print_table(); 

		sleep(2); 

		// sem_post(S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(S[phnum]); 
	} 
} 

// take up chopsticks 
void take_fork(int phnum) 
{ 

	sem_wait("mutex"); 

	// state that hungry 
	state[phnum] = HUNGRY; 

	// eat if neighbours are not eating 
	test(phnum); 

	sem_post("mutex"); 

	// if unable to eat wait to be signalled 
	sem_wait(S[phnum]); 

	sleep(1); 
} 

// put down chopsticks 
void put_fork(int phnum) 
{ 

	sem_wait("mutex"); 

	// state that thinking 
	state[phnum] = THINKING; 
	print_table();

	test(LEFT); 
	test(RIGHT); 

	sem_post("mutex"); 
} 

void* philospher(void* num) 
{ 

	while (1) { 

		int* i = num; 

		sleep(1); 

		take_fork(*i); 

		sleep(0); 

		put_fork(*i);

	} 
} 

void philo() { //void philo()

	int i;
	char c;
	char aux[3];
	char name[5];
	///pthread_t thread_id[N]; 

	// initialize the semaphores 
	mutex = sem_open("mutex", 1); 

	for (i = 0; i < N; i++){
		itoa(i, aux);
		S[i][0] = 's';
		S[i][1] = aux[0];
		S[i][2] = aux[1];
		S[i][3] = aux[2];
		//podria hacer name+1 = aux

		if( sem_open(S[i], 0) == -1)
			printf("ERROR WHILE OPENING SEM");
	} 

	for (i = 0; i < N; i++) { 

		// create philosopher processes 
		///pthread_create(&thread_id[i], NULL, philospher, &phil[i]);
		name[0] = 'p';
		itoa(i, aux);
		name[1] = aux[0];
		name[2] = ' ';
		name[3] = '&';
		char *argv[2] = {name, "i"};
		exec(&philospher, 2, argv);

	} 

	/* REVISAR */
	while ((c = getChar()) != 's'){
		
		if(c == 'a' && philos_number < MAX_PHILOS){
			sem_wait(mutex);
			//copiar el codigo de los dos for anteriores
			sem_post(mutex);
			philos_number++;
		}

		if(c == 'r' && philos_number > 0){
			philos_number--;
			sem_wait(mutex);
			//cerrar el semaforo
			sem_post(mutex);
		}
		
	}

	// //for (i = 0; i < N; i++) 

	// 	///pthread_join(thread_id[i], NULL); 
	
} 

void print_table(){
	for(int i=0; i < philos_number; i++){
		if( S[i] == EATING )
			printf(" E ");
		else
			printf(" * ");
	}
	enter();
}