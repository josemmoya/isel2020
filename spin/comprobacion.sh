#!bin/bash

#
#	Script que muestra como se cumplen todas las especificaciones
#	del ejercicio de las carreteras propuesto en clase de ISEL
#	2020 (cuando se cumplan)
#
#	author: Miguel Taibo
#	date 27/02/2020
#
# usage:
#   bash comprobacion.sh

spin -a model.pml
gcc -w -o pan pan.c

echo
echo

timeout 10s ./pan -a -f -N pulsando
timeout 10s ./pan -a -f -N no_pulsar
timeout 10s ./pan -a -f -N tiempo_encendida
timeout 10s ./pan -a -f -N introducir_codigo
timeout 10s ./pan -a -f -N infrarrojo
timeout 10s ./pan -a -f -N idle_tran
timeout 10s ./pan -a -f -N introducir_tran
timeout 10s ./pan -a -f -N activada_tran
