#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vdpau_layer.h"
#include "h264_layer.h"

#define FRAMES_IN_SAMPLE 25

int main() {
    int fd;
    struct stat sb;
    Display *disp;
    int screen;
    vdp_ctx *vdpau_ctx;
    char *memblk;

    h264_frame *frames[FRAMES_IN_SAMPLE];

    disp = XOpenDisplay((char*)0);
    screen = DefaultScreen(disp); 

    vdpau_ctx = init_vdpau_ctx(disp,screen);
    if (!vdpau_ctx) {
        fprintf(stdout,"Failed to init vdpau context\n");
        return -1;
    }
     
    fd = open("sample.dat",O_RDONLY);
    fstat(fd,&sb);
    memblk = mmap(NULL,sb.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memblk == MAP_FAILED) {
        fprintf(stderr,"Failed to create mmap\n");
        return -1;
    }
    memblk += H264_INFO_HDR_OFFSET;
    int i;
    for (i = 0; i < FRAMES_IN_SAMPLE; i++) {
        frames[i]  = create_h264_frame(&memblk); 
        fprintf(stdout,"Frame size: %d\n",frames[i]->datalen);

    }
}
