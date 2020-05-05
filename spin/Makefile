all: pan
	./pan -a -f -N pulsando
	./pan -a -f -N no_pulsar
	./pan -a -f -N tiempo_encendida
	./pan -a -f -N introducir_codigo
	./pan -a -f -N infrarrojo
	./pan -a -f -N idle_tran
	./pan -a -f -N introducir_tran
	./pan -a -f -N activada_tran

pan: pan.c
	gcc -o pan pan.c

pan.c: model.pml
	spin -a model.pml
