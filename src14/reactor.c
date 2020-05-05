#include "reactor.h"

/* res = a - b */
void timespec_sub (struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_nsec = a->tv_nsec - b->tv_nsec;
	if (res->tv_nsec < 0) {
		--res->tv_sec;
		res->tv_nsec += 1000000000;
	}
}

/* res = a + b */
void timespec_add (struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec + b->tv_sec;
	res->tv_nsec = a->tv_nsec + b->tv_nsec;
	if (res->tv_nsec > 1000000000) {
		++res->tv_sec;
		res->tv_nsec -= 1000000000;
	}
}

/* a < b */
int timespec_less (struct timespec *a, struct timespec *b)
{
  return (a->tv_sec < b->tv_sec) ||
    ((a->tv_sec == b->tv_sec) && (a->tv_nsec < b->tv_nsec));
}

/* wait until next_activation (absolute time) */
void delay_until (struct timespec* next_activation)
{
	struct timespec resto, now;
	clock_gettime(CLOCK_REALTIME, &now);
	timespec_sub(&resto, next_activation, &now);
	while (nanosleep(&resto,&resto)>0);
}

void printspec (struct timespec a, char* nombre)
{
  printf("%s:%ld,%ld \n",nombre, a.tv_sec,a.tv_nsec);
}

typedef struct reactor_t {
  EventHandler* ehs[10];
  int n_ehs;
} Reactor;

static Reactor r;

void event_handler_init (EventHandler* eh, int prio, eh_func_t run)
{
  eh->prio = prio;
  clock_gettime(CLOCK_REALTIME, &eh->next_activation);
  eh->run = run;
}

void event_handler_run (EventHandler* eh)
{
  eh->run (eh);
}

void reactor_init (void)
{
  r.n_ehs = 0;
}

int compare_prio (const void* a, const void* b)
{
  EventHandler* eh1 = *(EventHandler**) a;
  EventHandler* eh2 = *(EventHandler**) b;
  if (eh1->prio > eh2->prio)
    return -1;
  if (eh1->prio < eh2->prio)
    return 1;
  return 0;
}

void reactor_add_handler (EventHandler* eh)
{
  r.ehs[r.n_ehs++] = eh;
  qsort (r.ehs, r.n_ehs, sizeof (EventHandler*), compare_prio);
}

static struct timespec* reactor_next_timeout (void)
{
  static struct timespec next;
  struct timespec now;
  int i;

  if (! r.n_ehs) return NULL;

  clock_gettime(CLOCK_REALTIME, &now);
  next.tv_sec = now.tv_sec + 24*60*60;
  next.tv_nsec = now.tv_nsec;

  for (i = 0; i < r.n_ehs; ++i) {
    EventHandler* eh = r.ehs[i];
    if (timespec_less (&eh->next_activation, &next)) {
      next.tv_sec = eh->next_activation.tv_sec;
      next.tv_nsec = eh->next_activation.tv_nsec;
    }
  }
  if (timespec_less (&next, &now)) {
    next.tv_sec = now.tv_sec;
    next.tv_nsec = now.tv_nsec;
  }
  return &next;
}

void reactor_handle_events (void)
{
  int i;
  struct timespec now;

  delay_until(reactor_next_timeout());

  clock_gettime(CLOCK_REALTIME, &now);
  for (i = 0; i < r.n_ehs; ++i) {
    EventHandler* eh = r.ehs[i];
    if (timespec_less (&eh->next_activation, &now)) {
      event_handler_run (eh);
    }
  }
}
