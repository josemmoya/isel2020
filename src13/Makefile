CC = gcc
CFLAGS = -Wall -g -O2 $(DEBUG) -Iinclude -Iinclude/freertos -Ifreertos/portable/GCC/Posix
LDLIBS = -lpthread -lrt

# debug options
#DEBUG='-DDEBUG(x)=x' -include stdio.h

all: main

main:	main.o fsm.o tasks.o luces.o alarma.o timer.o croutine.o queue.o list.o port.o heap_3.o

clean:
	$(RM) *.o *~ main

vpath %.c freertos
vpath %.c freertos/portable/GCC/Posix
vpath %.c freertos/portable/MemMang
