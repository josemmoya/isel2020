#include "fsm_kbd.h"
#include "ttyraw.h"
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

static char last_key = 0;

static int chk_q (fsm_t* this) {
	return last_key == 'q';
}
static int chk_input (fsm_t* this) {
	fd_set rdset;
	struct timeval timeout = {0,0};
	FD_ZERO (&rdset);
	FD_SET (0, &rdset);
	return select (1, &rdset, NULL, NULL, &timeout) > 0;
}
static void do_exit (fsm_t* this) { 
	exit (0);
}
static void do_read (fsm_t* this) { 
	char c;
	fread (&c, 1, 1, stdin);
	printf ("key pressed: %c\r\n", c);
	if (c == 'q') exit (0);
}

fsm_t*
fsm_new_kbd()
{
	static struct fsm_trans_t tt[] = {
		{ 0, chk_q, 0, do_exit },
		{ 0, chk_input, 0, do_read },
		{ -1, NULL, -1, NULL},
	};
	static int raw;
	if (!raw) {
		tty_raw();
		raw = 1;
	}
	return fsm_new(tt);
}
