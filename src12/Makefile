CFLAGS= -Wall -Werror -g -O -ansi 

main: main.o fsm.o luces.o timer.o alarma.o
	gcc -o main $(CFLAGS)  main.o fsm.o luces.o timer.o alarma.o

%.o : %.c
	gcc -c $<

clean:
	rm -f main *.o
