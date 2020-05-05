#include <stdio.h>
#include <termios.h>
#include <sys/select.h>

#include "reactor.h"
#include "luces.h"
#include "alarma.h"

luces_t* lampara = NULL;
alarma_t* alarma = NULL;

/* RAW MODE */
static struct termios orig_termios;
void disable_raw_mode()
{
  if (tcsetattr(0, TCSAFLUSH, &orig_termios) == -1)
    perror ("tcsetattr");
}
void enable_raw_mode()
{
  if (tcgetattr(0, &orig_termios) == -1) perror("tcgetattr");
  atexit(disable_raw_mode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(0, TCSAFLUSH, &raw) == -1) perror("tcsetattr");
}

/* Funciones auxiliares de task_keyboard */
int key_pressed (void)
{
  struct timeval timeout = { 0, 0 };
  fd_set rd_fdset;
  FD_ZERO(&rd_fdset);
  FD_SET(0, &rd_fdset);
  return select(1, &rd_fdset, NULL, NULL, &timeout) > 0;
}

void key_process (int ch)
{
  switch (ch) {
    case 'p':
      pulsacion_alarma_isr();
      pulsacion_luces_isr();
      break;
    case 'i':
      PIR_alarma_isr();
      PIR_luces_isr();
      break;
  	case 'q':

      printf ("\e0\e[?25h"); //printf "\e0\e[?25h"
      printf("\e[%d;%df%s",15,10,"\r\n");
      /* TOMA de datos */
      //printf("tiempo de ejecucion maximo: %ld.%.9ld (s)\r\n",max.tv_sec,max.tv_nsec);
  		exit(0); break;
  }
}

/* Funciones auxiliares de task_screen */
void setup_screen(void)
{
	printf ("\e7\e[?25l");
	printf("\e[2J\e[%d;0f",5);
  printf("\e[%d;%df%s",5,5,"Presiona p como pulsador");
  printf("\e[%d;%df%s",6,5,"Presiona i como sensor PIR");
  printf("\e[%d;%df%s",7,5,"Presiona q para salir");
  printf("\e[%d;%df%s",10,10,"LUZ");
  printf("\e[%d;%df%s",12,11,"_");
  printf("\e[%d;%df%s",10,20,"CODIGO");
  printf("\e[%d;%df%s",12,21,"_ _ _\n");
  printf("\e[%d;%df%s",10,30,"DISPLAY");
  printf("\e[%d;%df%s",12,31,"_ _ _\n");
  printf("\e[%d;%df%s",10,40,"ALARMA");
  printf("\e[%d;%df%s",12,42,"_\n");
}

void refresh_screen(void)
{
  if (lampara->l)
    printf("\e[%d;%df%s",12,11,"X");
  else
    printf("\e[%d;%df%s",12,11,"_");
  if (alarma->alarma)
    printf("\e[%d;%df%s",12,42,"X");
  else
    printf("\e[%d;%df%s",12,42,"_");
  printf("\e[%d;%df%s",11,19,"                 ");
  char code[7];
  if (alarma->i<3) {
    printf("\e[%d;%df%s",11,21+2*alarma->i,"o");
    sprintf(code, "%d %d %d\n", alarma->codigos[0], alarma->codigos[1], alarma->codigos[2]);
    printf("\e[%d;%df%s",12,21,code);
  } else {
    printf("\e[%d;%df%s",11,25+2*alarma->i,"o");
    sprintf(code, "%d %d %d\n", alarma->codigos[3], alarma->codigos[4], alarma->codigos[5]);
    printf("\e[%d;%df%s",12,31,code);
  }

}

static void alarma_func (struct event_handler_t* this)
{
  static const struct timespec period = {0, 010000000};
  fsm_fire((fsm_t*) alarma);
  timespec_add (&this->next_activation, &this->next_activation, &period);
}
static void luces_func (struct event_handler_t* this)
{
  static const struct timespec period = {0, 040000000};
  fsm_fire((fsm_t*) lampara);
  timespec_add (&this->next_activation, &this->next_activation, &period);
}
static void screen_func (struct event_handler_t* this)
{
  static const struct timespec period = {0, 040000000};
  refresh_screen();
  timespec_add (&this->next_activation, &this->next_activation, &period);
}
static void keyboard_func (struct event_handler_t* this)
{
  static const struct timespec period = {0, 010000000};
  if (key_pressed()) {
    key_process(getchar());
  }
  timespec_add (&this->next_activation, &this->next_activation, &period);
}

int main () {

  enable_raw_mode();
  setup_screen();

  lampara = fsm_new_luces();
  alarma = fsm_new_alarma();

  EventHandler eh_alarma;
  EventHandler eh_luces;
  EventHandler eh_screen;
  EventHandler eh_keyboard;

  reactor_init ();

  event_handler_init (&eh_alarma, 4, alarma_func);
  event_handler_init (&eh_luces, 1, luces_func);
  event_handler_init (&eh_screen, 2, screen_func);
  event_handler_init (&eh_keyboard, 3, keyboard_func);

  reactor_add_handler (&eh_alarma);
  reactor_add_handler (&eh_luces);
  reactor_add_handler (&eh_screen);
  reactor_add_handler (&eh_keyboard);

  while (1) {
    reactor_handle_events ();
  }
}
