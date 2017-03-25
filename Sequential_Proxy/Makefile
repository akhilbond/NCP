#Sample Makefile. You can make changes to this file according to your need
# The executable must be named proxy

CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -lpthread

OBJS = proxy.o csapp.o

all:
		gcc -pthread -o processproxy processproxy.c csapp.c
		gcc -pthread -o threadproxy threadproxy.c csapp.c

processproxy:
		gcc -pthread -o processproxy processproxy.c

threadproxy:
		gcc -pthread -o threadproxy threadproxy.c


clean:
	rm -f *~ *.o processproxy
	rm -f *~ *.o threadproxy
	rm -f *~ *.o proxy.log
