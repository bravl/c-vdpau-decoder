CC=gcc
CFLAGS=-lvdpau -lX11
all: test

test: main.c vdpau_layer.c h264_layer.c
	$(CC) -g -o vdpau main2.c vdpau_layer.c h264_layer.c $(CFLAGS)
