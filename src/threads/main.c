#include "task.h"
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

static int temp;
static pthread_mutex_t m_temp;

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
	pthread_mutex_lock (&m_temp);
	*sample_temp = rand() % 40; 
	pthread_mutex_unlock (&m_temp);
	timeval_add (&next_sample, &next_sample, &period);
}

static int send_activated(fsm_t* this) {
	struct timeval now;
	gettimeofday (&now, NULL);
	return timeval_less (&next_send, &now);
}
static void send_sample (fsm_t* this) { 
	static struct timeval period = { 10, 0 };
	pthread_mutex_lock (&m_temp);
	printf ("send: %d\r\n", *send_temp); 
	pthread_mutex_unlock (&m_temp);
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

static
void *
sample_func (void* arg)
{
  fsm_t* fsm_sample = fsm_new_sample(&temp);
  struct timeval *period = task_get_period (pthread_self());
  struct timeval next;

  gettimeofday (&next, NULL);
  while (1) {
    delay_until (&next);
    timeval_add (&next, &next, period);

    printf ("sample\r\n");
    fsm_fire (fsm_sample);
  }
}


static
void *
send_func (void* arg)
{
  fsm_t* fsm_send = fsm_new_send(&temp);
  struct timeval *period = task_get_period (pthread_self());
  struct timeval next;

  gettimeofday (&next, NULL);
  while (1) {
    delay_until (&next);
    timeval_add (&next, &next, period);

    printf ("send\r\n");
    fsm_fire (fsm_send);
  }
}



void
kbd_loop (void)
{
  static int cnt;
  fsm_t* fsm_kbd = fsm_new_kbd();
  struct timeval period = { 1, 0 * 1000 };
  struct timeval next;

  gettimeofday (&next, NULL);
  while (1) {
    delay_until (&next);
    timeval_add (&next, &next, &period);

    cnt = (cnt + 1) % 25;
    if (!cnt) printf ("kbd * 25\r\n");
    fsm_fire (fsm_kbd);
  }
}


int
main ()
{
  pthread_t tid_sample, tid_send, tid_kbd;
  mutex_init (&m_temp, 2);
  tid_sample = task_new ("sample", sample_func, 5000, 5000, 2, 1024);
  tid_send = task_new ("send", send_func, 5000, 5000, 1, 1024);
  kbd_loop();
  return 0;
}

