#!/usr/bin/sh
gcc -c servermy_global.c
gcc -c client.c
gcc servermy_global.o client.o -o client
rm *.o
sudo ./client
