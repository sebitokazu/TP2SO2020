/*
Adaptacion de la implementacion obtenida en:
https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/
*/

#include "philo.h"

int state[MAX_PHILOS];	//malloc
int phil[N] = { 0, 1, 2, 3, 4 };	//CREO QUE NO ES NECESARIO

int philos_number = N;
int mutex; //not used
char *S[MAX_PHILOS];	//malloc

void sleep(int n) {
    long i;
	n *= 100000000;
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

		sleep(2); 

		// sem_post(S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(S[phnum]); 
		if(phnum == 0)
			printf("siiiiiii");

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

	if(phnum == 0)
		printf("PAN");
	// if unable to eat wait to be signalled 
	sem_wait(S[phnum]);
	if(phnum == 0)
		printf("QUESO");
 

	sleep(1); 
} 

// put down chopsticks 
void put_fork(int phnum) 
{ 

	sem_wait("mutex"); 

	// state that thinking 
	state[phnum] = THINKING; 

	// enter();
	// printf("Mine: "); printfd(phnum); enter();
	// printf("Left: "); printfd(LEFT); enter();
	// printf("Right: "); printfd(RIGHT); enter();
	// sleep(2);

	test(LEFT); 
	test(RIGHT); 

	sem_post("mutex"); 
} 

void* philospher(/*void* num*/ int phnum) 
{ 
	printf("  PHILO: ");printfd(phnum);enter();
	while (1) { 
		// int* i = num; 

		sleep(1); 

		printf("			Philo"); printfd(phnum); printf(" TRIES"); enter();
		//take_fork(*i); 
		take_fork(phnum);
		printf("			Philo"); printfd(phnum); printf(" GETS"); enter();

		
		print_table();
		//enter(); ps(); enter(); enter();

		sleep(0); 

		//put_fork(*i);
		printf("			Philo"); printfd(phnum); printf(" PUTS"); enter();
		put_fork(phnum);
		printf("			Philo"); printfd(phnum); printf(" LEAVES"); enter();


		print_table();
		//enter(); ps(); enter(); enter();

	} 
} 


void new_philo(int i){
	char aux[3];

	/* create semaphore */
	state[i] = THINKING;	//set philo's state
	itoa(i, aux);
	S[i][0] = 's';
	S[i][1] = aux[0];
	S[i][2] = aux[1];
	S[i][3] = aux[2];
	//podria hacer name+1 = aux

	if( sem_open(S[i], 0) == -1)
		printf("ERROR WHILE OPENING SEM");

	/* create philosopher processes */ 
	///pthread_create(&thread_id[i], NULL, philospher, &phil[i]);
	// name[0] = 'p';
	// itoa(i, aux);
	// name[1] = aux[0];
	// name[2] = ' ';
	// name[3] = '&';
	
	char *name[] = {"p &"};
	itoa(i, aux);
	printfd(i);
	printf(aux);
	printf(" ");
	char *par[] = {aux};
	char *argv[] = {"p &", par};
	exec(&philospher, i, argv);		//REVISAR SEGUNDO PARAM
	//ps();

}


void philo() { //void philo()
	int i;
	char c;
	///pthread_t thread_id[N]; 

	// initialize the semaphores 
	mutex = sem_open("mutex", 1);
	
	if( mutex == -1)
		printf("ERROR WHILE OPENING SEM");

	 
	for (i = 0; i < N; i++){
		new_philo(i);
	}

	/* REVISAR */
	while ((c = getChar()) != 'q'){
		
		if(c == 'a' && philos_number < MAX_PHILOS){
			sem_wait(mutex);
			new_philo(philos_number);
			philos_number++;
			sem_post(mutex);
		}

		if(c == 'r' && philos_number > 0){
			sem_wait(mutex);
			philos_number--;
			sem_close(S[philos_number]);	//matar proceso
			sem_post(mutex);
		}

		if(c == 't')
			print_table();
		
	}

	// for (i = 0; i < N; i++) 

	// pthread_join(thread_id[i], NULL);

	yield();	// agregar antes matar philos y cerrar semaforos
	// SINO CAMBIAR WHILE(GETCHAR) POR WHILE(1)
	
	// ADEMAS DEBERIAMOS VER QUE LA SHELL Y PHILO (este while) 
	//	NO LE ROBE TIEMPO A LOS PROCESOS P (filosofos)
	
} 

void print_table(){
	for(int i=0; i < philos_number; i++){
		if( state[i] == EATING )
			printf(" E ");
		else
			printf(" * ");
	}
	enter();
}