#include "fsm.h"
#include "reactor.h"

struct luces_t;
typedef struct luces_t luces_t;
struct luces_t {
 fsm_t fsm;
 struct timespec next;
 int l;
};

void luces_init (luces_t* this);
luces_t* fsm_new_luces();

void print_luces(luces_t* this);
void pulsacion_luces_isr(void);
void PIR_luces_isr(void);
