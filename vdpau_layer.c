#include "vdpau_layer.h"

vdp_ctx *init_vdpau_ctx(Display *display, int screen) {
    VdpStatus status = VDP_STATUS_OK;

    if (!display) {
        fprintf(stderr,"Display cannot be null\n");
        return NULL;
    }
    vdp_ctx *ctx = malloc(sizeof(vdp_ctx));
    if (!ctx) return NULL;

    ctx->display = display;
    ctx->screen = screen;

    fprintf(stdout,"Initializing vdpau context\n");
    status = vdp_device_create_x11(ctx->display, ctx->screen, &ctx->vdp_device, 
                                   &ctx->vdp_get_proc_address);
    fprintf(stdout,"Created x11 device\n");

    ctx->table = init_vdpau_functions(ctx);

    return ctx;
}

vdp_functable *init_vdpau_functions(vdp_ctx *ctx) {
    fprintf(stdout,"Initializing vpdau functions\n");
    VdpStatus status;
    vdp_functable *table = malloc(sizeof(vdp_functable));
    if (!table) return NULL;

    void *iter;
    int i;
    int size = sizeof(vdp_functable) / sizeof(void*);
   
    for (i = 0, iter = table; i < size; i++, iter+=(sizeof(void*))) {
        status = ctx->vdp_get_proc_address(ctx->vdp_device, refs[i], iter);
        if (status != VDP_STATUS_OK) printf("Failed\n");
    }
    unsigned int version;
    const char *info;
    table->vdp_get_information_string(&info); 
    table->vdp_get_api_version(&version);
    printf("%s %d\n",info,version);
    return table;
}
