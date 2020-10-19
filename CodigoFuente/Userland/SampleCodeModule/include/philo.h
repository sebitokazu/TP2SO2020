#ifndef PHILO_H
#define PHILO_H

#include "semaphore_lib.h" 
#include "windowManager.h"
#include "stdio.h"

#define N 5 
#define MAX_PHILOS 30
#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 
#define LEFT (phnum + philos_number - 1) % philos_number 
#define RIGHT (phnum + 1) % philos_number

void philo();

#endif