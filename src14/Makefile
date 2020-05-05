CFLAGS= -Wall -Werror -g -O -ansi

all: main

main: main.o fsm.o luces.o alarma.o reactor.o
	gcc -o main $(CFLAGS) main.o reactor.o fsm.o luces.o  alarma.o

%.o : %.c
	gcc -c $<

clean:
	rm -f main *.o
