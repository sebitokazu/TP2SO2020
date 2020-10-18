/*
Adaptacion de la implementacion obtenida en:
https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/
*/

#include "philo.h"

int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };	//CREO QUE NO ES NECESARIO

int mutex; 
int S[N];	//malloc

void test(int phnum) 
{ 
	if (state[phnum] == HUNGRY 
		&& state[LEFT] != EATING 
		&& state[RIGHT] != EATING) { 
		// state that eating 
		state[phnum] = EATING; 

		//sleep(2); 

		// sem_post(&S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(&S[phnum]); 
	} 
} 

// take up chopsticks 
void take_fork(int phnum) 
{ 

	sem_wait(&mutex); 

	// state that hungry 
	state[phnum] = HUNGRY; 

	// eat if neighbours are not eating 
	test(phnum); 

	sem_post(&mutex); 

	// if unable to eat wait to be signalled 
	sem_wait(&S[phnum]); 

	//sleep(1); 
} 

// put down chopsticks 
void put_fork(int phnum) 
{ 

	sem_wait(&mutex); 

	// state that thinking 
	state[phnum] = THINKING; 

	test(LEFT); 
	test(RIGHT); 

	sem_post(&mutex); 
} 

void* philospher(void* num) 
{ 

	while (1) { 

		int* i = num; 

		//sleep(1); 

		take_fork(*i); 

		//sleep(0); 

		put_fork(*i); 
	} 
} 

void philo() { //void philo()

	int i;
	char aux[3];
	char name[5];
	///pthread_t thread_id[N]; 

	// initialize the semaphores 
	mutex = sem_open("mutex", 1); 

	for (i = 0; i < N; i++){
		intToStr(i, aux);
		name[0] = "s";
		name[1] = aux[0];
		name[2] = aux[1];
		name[3] = aux[2];
		//podria hacer name+1 = aux

		S[i] = sem_open(name, 0);
	} 

	for (i = 0; i < N; i++) { 

		// create philosopher processes 
		///pthread_create(&thread_id[i], NULL, philospher, &phil[i]);
		name[0] = "p";
		char *argv[2] = {name, "i"};
		exec(&philospher, 2, argv);

	} 

	for (i = 0; i < N; i++) 

		///pthread_join(thread_id[i], NULL); 
	
	return;
} 
