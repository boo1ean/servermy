#!/usr/bin/sh
gcc -c servermy_global.c
gcc -c server.c
gcc servermy_global.o server.o -o server
rm *.o
sudo ./server
