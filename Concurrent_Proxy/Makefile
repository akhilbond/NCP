#Sample Makefile. You can make changes to this file according to your need
# The executable must be named proxy

CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -lpthread

OBJS = proxy.o csapp.o

all:
		gcc -pthread -o proxy proxy.c csapp.c

proxy:
		gcc -pthread -o proxy proxy.c

clean:
	rm -f *~ *.o proxy
	rm -f *~ *.o proxy.log
