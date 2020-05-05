#include "luces.h"

static int p = 0;
static int PIR = 0;
static struct timespec T = {60, 0};

/* Comprobaciones */
static int no_pulse (fsm_t* this) {
  return p==0 && PIR==0;
}
static int pulse (fsm_t* this) {
  return p || PIR;
}
static int no_pulse_no_t (fsm_t* this) {
  luces_t* luz = (luces_t*) this;
  return (p==0 && PIR==0 && timespec_less(&now,&luz->next));
}
static int no_pulse_t (fsm_t* this) {
  luces_t* luz = (luces_t*) this;
  return (p==0 && PIR==0 && timespec_less(&luz->next,&now));
}

/* Transiciones */
static void apagada (fsm_t* this) {

  luces_t* luz = (luces_t*) this;
  luz->l = 0;
}
static void encender (fsm_t* this) {
  p = 0;
  PIR = 0;
  luces_t* luz = (luces_t*) this;
  luz->l = 1;
  timespec_add(&luz->next,&now, &T);

}
static void encendida (fsm_t* this) {
  luces_t* luz = (luces_t*) this;
  luz->l = 1;
}

/* Estructura */
void luces_init (luces_t* this) {
  static fsm_trans_t tt[] = {
    { 0, no_pulse, 0, apagada},
    { 0, pulse, 1, encender},
    { 1, pulse, 1, encender},
    { 1, no_pulse_no_t, 1, encendida},
    { 1, no_pulse_t, 0, apagada},
    { -1, NULL, -1, NULL }
  };
  fsm_init ((fsm_t*) this, tt);

  this->l = 0;
}

luces_t* fsm_new_luces (void)
{
  luces_t* this = (luces_t*) malloc (sizeof (luces_t));
  luces_init (this);
  return this;
}

/* Debug */
void print_luces(luces_t* this) {
  printf("luces (%d)-> state: %d, l: %d, next:%ld,%ld\r\n", p,this->fsm.current_state , this->l,this->next.tv_sec,this->next.tv_nsec);
}

/* isr functions */
void pulsacion_luces_isr(void) {
  p=1;
}
void PIR_luces_isr(void) {
  PIR=1;
}
