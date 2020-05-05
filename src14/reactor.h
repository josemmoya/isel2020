#ifndef REACTOR_H
#define REACTOR_H

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void timespec_sub (struct timespec *res, struct timespec *a, struct timespec *b);
void timespec_add (struct timespec *res, struct timespec *a, struct timespec *b);
int timespec_less (struct timespec *a, struct timespec *b);


struct event_handler_t;
typedef void (*eh_func_t) (struct event_handler_t*);

struct event_handler_t {
  int prio;
  struct timespec next_activation;
  eh_func_t run;
};
typedef struct event_handler_t EventHandler;

void event_handler_init (EventHandler* eh, int prio, eh_func_t run);
void event_handler_run (EventHandler* eh);

void reactor_init (void);
void reactor_add_handler (EventHandler* eh);
void reactor_handle_events (void);

#endif
