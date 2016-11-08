CC=gcc
CFLAGS=-lvdpau -lX11
all: test

test: main.c
	$(CC) -g -o vdpau main.c  $(CFLAGS)
