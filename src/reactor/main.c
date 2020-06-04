#include "reactor.h"
#include "fsm.h"
#include "fsm_kbd.h"
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

void 
delay_until (struct timeval* next_activation)
{
  struct timeval now, timeout;
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  select (0, NULL, NULL, NULL, &timeout);
}

static int* sample_temp;
static int* send_temp;
static struct timeval next_sample;
static struct timeval next_send;

static int sample_activated(fsm_t* this) {
	struct timeval now;
	gettimeofday (&now, NULL);
	return timeval_less (&next_sample, &now);
}
static void take_sample (fsm_t* this) { 
	static struct timeval period = { 10, 0 };
	*sample_temp = rand() % 40; 
	timeval_add (&next_sample, &next_sample, &period);
}

static int send_activated(fsm_t* this) {
	struct timeval now;
	gettimeofday (&now, NULL);
	return timeval_less (&next_send, &now);
}
static void send_sample (fsm_t* this) { 
	static struct timeval period = { 10, 0 };
	printf ("send: %d\r\n", *send_temp); 
	timeval_add (&next_send, &next_send, &period);
}

fsm_t*
fsm_new_sample(int* temp)
{
	static struct fsm_trans_t tt[] = {
		{ 0, sample_activated, 0, take_sample },
		{ -1, NULL, -1, NULL},
	};
	sample_temp = temp;
	gettimeofday(&next_sample, NULL);
	return fsm_new(tt);
}

fsm_t*
fsm_new_send(int* temp)
{
	static struct fsm_trans_t tt[] = {
		{ 0, send_activated, 0, send_sample },
		{ -1, NULL, -1, NULL},
	};
	send_temp = temp;
	gettimeofday(&next_send, NULL);
	return fsm_new(tt);
}

static int temp;
static fsm_t* fsm_sample;
static fsm_t* fsm_send;
static fsm_t* fsm_kbd;

static
void
sample_func (struct event_handler_t* this)
{
  static const struct timeval period = { 5, 0 };
  fsm_fire (fsm_sample); 
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static
void
send_func (struct event_handler_t* this)
{
  static const struct timeval period = { 5, 0 };
  fsm_fire (fsm_send); 
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static
void
kbd_func (struct event_handler_t* this)
{
  static const struct timeval period = { 0, 200 * 1000 };
  fsm_fire (fsm_kbd); 
  timeval_add (&this->next_activation, &this->next_activation, &period);
}


int
main ()
{
  EventHandler eh_sample, eh_send, eh_kbd;
  reactor_init ();

  fsm_sample = fsm_new_sample(&temp);
  fsm_send = fsm_new_send(&temp);
  fsm_kbd = fsm_new_kbd();

  event_handler_init (&eh_sample, 2, sample_func);
  reactor_add_handler (&eh_sample);

  event_handler_init (&eh_send, 1, send_func);
  reactor_add_handler (&eh_send);

  event_handler_init (&eh_kbd, 1, kbd_func);
  reactor_add_handler (&eh_kbd);

  while (1) {
    reactor_handle_events ();
  }
}

