#ifndef PHILO_H
#define PHILO_H

#include "semaphore_lib.h" 
#include "stdlib.h"

#define N 5 
#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 
#define LEFT (phnum + 4) % N 
#define RIGHT (phnum + 1) % N

void philo();

#endif