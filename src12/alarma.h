#include "fsm.h"
#include "timer.h"

struct alarma_t;
typedef struct alarma_t alarma_t;
struct alarma_t {
  fsm_t fsm;
  struct timespec next;
  int q;
  int i;
  int alarma;
  int codigos[6];
};

void alarma_init (alarma_t* this);
alarma_t* fsm_new_alarma();

void print_alarma(alarma_t* this);

/* isr functions */
void pulsacion_alarma_isr(void);
void PIR_alarma_isr(void);
