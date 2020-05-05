#include "alarma.h"


static int p = 0;
static int PIR = 0;
static struct timespec T = {1, 0};
static struct timespec max_T =  {2147483647, 2147483647};

static int comp_codigo(int codigos[6]) {
  return (codigos[0]==codigos[3] && codigos[1]==codigos[4] && codigos[2]==codigos[5]);
}

/* Comprobaciones */
static int pulsada (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (p && timespec_less(&now, &alarma->next));
}
static int i_pulsada (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  return (p && alarma->i==0);
}
static int d_pulsada (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  return (p && alarma->i==3);
}
static int timeout_int (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (timespec_less(&alarma->next,&now) && alarma->i<2);
}
static int timeout_det (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (timespec_less(&alarma->next,&now) && alarma->i<5);
}
static int fin_int (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (timespec_less(&alarma->next,&now) && alarma->i==2);
}
static int c_acierto (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (timespec_less(&alarma->next,&now) && alarma->i==5 && comp_codigo(alarma->codigos));
}
static int c_fallo (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (timespec_less(&alarma->next,&now) && alarma->i==5 && (!comp_codigo(alarma->codigos)));
}
static int is_PIR (fsm_t* this) {
  return PIR;
}



/* Transiciones */
static void next_count(fsm_t* this) {
  p = 0;
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  timespec_add(&alarma->next,&now, &T);
  alarma->q=(alarma->q+1)%10;
  alarma->codigos[alarma->i]=alarma->q;
  return;
}
static void inicio_alarma(fsm_t* this) {
  p = 0;
  alarma_t* alarma = (alarma_t*)this;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  timespec_add(&alarma->next,&now, &T);
  alarma->i = 3;
  alarma->q=(alarma->q+1)%10;
  alarma->codigos[alarma->i]=alarma->q;
  return;
}
static void next_digit(fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  alarma->next = max_T;
  alarma->codigos[alarma->i]=alarma->q;
  alarma->i+=1;
  alarma->q = alarma->codigos[alarma->i];
  return;
}
static void acierto (fsm_t* this) {
  PIR = 0;
  alarma_t* alarma = (alarma_t*)this;
  alarma->next = max_T;
  alarma->alarma = 0;
  alarma->i=0;
  alarma->q=0;
  alarma->codigos[0]=0;
  alarma->codigos[1]=0;
  alarma->codigos[2]=0;
  alarma->codigos[3]=0;
  alarma->codigos[4]=0;
  alarma->codigos[5]=0;
}
static void fallo (fsm_t* this) {
  alarma_t* alarma = (alarma_t*)this;
  alarma->next = max_T;
  alarma->alarma = 1;
  alarma->i=3;
  alarma->q=0;
  alarma->codigos[3]=0;
  alarma->codigos[4]=0;
  alarma->codigos[5]=0;
}
static void on_alarma (fsm_t* this) {
  PIR=0;
  alarma_t* alarma = (alarma_t*)this;
  alarma->alarma = 1;
}
static void set_alarm (fsm_t* this) {
  PIR=0;
  alarma_t* alarma = (alarma_t*)this;
  alarma->next = max_T;
  alarma->codigos[alarma->i]=alarma->q;
  alarma->i+=1;
  alarma->q = alarma->codigos[alarma->i];
  return;
}

/* Estructura */
void alarma_init (alarma_t* this) {
  static fsm_trans_t tt[] = {
    { 0, i_pulsada, 1, next_count},
    { 1, pulsada, 1, next_count},
    { 1, timeout_int, 1, next_digit},
    { 1, fin_int, 2, set_alarm},

    { 2, pulsada, 2, next_count},
    { 2, timeout_det, 2, next_digit},
    { 2, c_acierto, 0, acierto},
    { 2, c_fallo, 2, fallo},
    { 2, is_PIR, 2, on_alarma},
    { -1, NULL, -1, NULL }
  };
  fsm_init ((fsm_t*) this, tt);
  this->next = max_T;
  this->q = 0;
  this->i = 0;
  this->alarma = 0;
  this->codigos[0] = 0;
  this->codigos[1] = 0;
  this->codigos[2] = 0;
  this->codigos[3] = 0;
  this->codigos[4] = 0;
  this->codigos[5] = 0;
};

alarma_t* fsm_new_alarma(void) {
  alarma_t* this = (alarma_t*) malloc (sizeof (alarma_t));
  alarma_init(this);
  return this;
}

/* Debug */
void print_alarma(alarma_t* this) {
  printf("alarma (%d) -> state: %d, q: %d, i: %d, next:%ld,%ld, codigos: %d,%d,%d\r\n", p,this->fsm.current_state , this->q,this->i,this->next.tv_sec,this->next.tv_nsec, this->codigos[0], this->codigos[1], this->codigos[2]);
}

void pulsacion_alarma_isr(void) {
  p=1;
}
void PIR_alarma_isr(void) {
  PIR = 1;
}
