#include "fsm.h"
#include "fsm_kbd.h"
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

int
timeval_less (const struct timeval* a, const struct timeval* b)
{
  return (a->tv_sec == b->tv_sec)? (a->tv_usec < b->tv_usec) :
    (a->tv_sec < b->tv_sec);
}

void
timeval_add (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec + b->tv_sec;
  res->tv_usec = a->tv_usec + b->tv_usec;
  if (res->tv_usec >= 1000000) {
    res->tv_sec += res->tv_usec / 1000000;
    res->tv_usec = res->tv_usec % 1000000;
  }
}

void
timeval_sub (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    res->tv_sec --;
    res->tv_usec += 1000000;
  }
}

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

int main () 
{
	struct timeval period = { 0, 200 * 1000 };
	struct timeval next;
	int temp;
	fsm_t* fsm_sample = fsm_new_sample(&temp);
	fsm_t* fsm_send = fsm_new_send(&temp);
	fsm_t* fsm_kbd = fsm_new_kbd();

	gettimeofday(&next, NULL);
	while (1) {
		fsm_fire (fsm_sample);
		fsm_fire (fsm_send);
		fsm_fire (fsm_kbd);
		timeval_add (&next, &next, &period);
		delay_until (&next);
	}
}
