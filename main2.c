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

//TODO: Fix nasty linked structs create better solution

int main() {
    XEvent event;
    int fd;
    struct stat sb;
    Display *disp;
    int screen;
    vdp_ctx *vdpau_ctx;
    vdp_decoder_ctx *vdpau_dec_ctx;
    vdp_mixer_ctx *vdpau_mixer_ctx;
    char *memblk;
    VdpStatus status = VDP_STATUS_OK;
    unsigned long black,white;

    h264_frame *frames[FRAMES_IN_SAMPLE];

    disp = XOpenDisplay((char*)0);
    screen = DefaultScreen(disp);  

    black = BlackPixel(disp,screen);
    white = WhitePixel(disp,screen);

    vdpau_ctx = init_vdpau_ctx(disp,screen);
    if (!vdpau_ctx) {
        fprintf(stdout,"Failed to init vdpau context\n");
        return -1;
    }
    
    vdpau_ctx->win = XCreateSimpleWindow(disp, DefaultRootWindow(disp),
                                         0, 0, 1920, 1080, 5, black,
                                         white);
    XSetStandardProperties(disp, vdpau_ctx->win,"VDPAU Player","VDPAU",None,NULL,0,NULL);
    XSelectInput(disp,vdpau_ctx->win,ExposureMask|ButtonPressMask|KeyPressMask);
    vdpau_ctx->gc=XCreateGC(disp,vdpau_ctx->win, 0,NULL);
    XSetBackground(disp,vdpau_ctx->gc,white);
    XSetBackground(disp,vdpau_ctx->gc,black);
    XClearWindow(disp,vdpau_ctx->win);
    XMapWindow(disp, vdpau_ctx->win);
    
    fd = open("sample.dat",O_RDONLY);
    fstat(fd,&sb);
    memblk = mmap(0,sb.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memblk == MAP_FAILED) {
        fprintf(stderr,"Failed to create mmap\n");
        return -1;
    }

    // Parse width 4b, height 4b, ratio 8b, profile 4b
    vdpau_dec_ctx = init_vdpau_decoder(vdpau_ctx, &memblk);
    if (!vdpau_dec_ctx) {
        fprintf(stdout,"Failed to create decoder\n");
        return -1;
    }
    fprintf(stdout,"Created decoder for %ld x %ld : %lf\n",
            vdpau_dec_ctx->width, vdpau_dec_ctx->height,
            vdpau_dec_ctx->ratio);
   
    vdpau_mixer_ctx = init_vdpau_mixer(vdpau_dec_ctx);
    if (!vdpau_mixer_ctx) {
        fprintf(stderr,"Failed to create mixer\n");
        return -1;
    }

    status = init_vdpau_output(vdpau_dec_ctx);
    if (status != VDP_STATUS_OK) {
        fprintf(stderr,"Failed to create vdpau output\n");
        return -1;
    }
    fprintf(stdout, "Created vdpau output stuff\n");
    status = init_vdpau_surfaces(vdpau_dec_ctx);
    if (status != VDP_STATUS_OK) {
        fprintf(stderr,"Failed to initialize surfaces\n");
        return -1;
    }
    int i;
    for (i = 0; i < FRAMES_IN_SAMPLE; i++) { 
        frames[i]  = create_h264_frame(&memblk); 
        //fprintf(stdout,"Frame size: %d\n",frames[i]->datalen);
    }
     
    if (!munmap(memblk,sb.st_size)) {
        fprintf(stderr,"Failed to unmmap file\n");
    }
     
    fprintf(stdout,"Unmmap'd file\n");
    while(1){
        XNextEvent(disp, &event);
        if(event.type == KeyPress){
            XFreeGC(disp, vdpau_ctx->gc);
            XDestroyWindow(disp, vdpau_ctx->win);
            XCloseDisplay(disp);
            exit(0);

        }
    }
}
