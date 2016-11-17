CC=gcc
CFLAGS=-lvdpau -lX11
all: test

test: main.c vdpau_layer.c
	$(CC) -g -o vdpau main.c vdpau_layer.c $(CFLAGS)
