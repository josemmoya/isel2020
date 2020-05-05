#ifndef TIMER_H
#define TIMER_H
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
//FSM Time Management functions
void timespec_sub (struct timespec *res, struct timespec *a, struct timespec *b);
void timespec_add (struct timespec *res, struct timespec *a, struct timespec *b);
int timespec_less (struct timespec *a, struct timespec *b);
void delay_until (struct timespec* next_activation);


/* Libreria */
struct timespec now;
