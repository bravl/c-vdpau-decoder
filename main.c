#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
//#include <vdpau/vdpau.h>
#include <vdpau/vdpau_x11.h>

static Display *x11_display;
static int x11_screen;

// VDPAU variables
static VdpDevice vdp_device;

// VDPAU functions (That will be dynamically linked)

VdpDeviceDestroy                                *vdp_device_destroy;
VdpGetProcAddress                               *vdp_get_proc_address;
VdpGetErrorString                               *vdp_get_error_string;

VdpVideoSurfaceCreate                           *vdp_video_surface_create;
VdpVideoSurfaceDestroy                          *vdp_video_surface_destroy;
VdpVideoSurfaceQueryCapabilities                *vdp_video_surface_query_capabilities;
VdpVideoSurfaceQueryGetPutBitsYCbCrCapabilities *vdp_video_surface_query_ycbcr_capabilities;
VdpVideoSurfaceGetParameters                    *vdp_video_surface_get_parameters;
VdpVideoSurfaceGetBitsYCbCr                     *vdp_video_surface_get_bits_ycbcr;
VdpVideoSurfacePutBitsYCbCr                     *vdp_video_surface_put_bits_ycbcr;

VdpDecoderCreate                                *vdp_decoder_create;
VdpDecoderDestroy                               *vdp_decoder_destroy;
VdpDecoderRender                                *vdp_decoder_render;
VdpDecoderQueryCapabilities                     *vdp_decoder_query_capabilities;
VdpDecoderGetParameters                         *vdp_decoder_get_parameters;

VdpVideoMixerCreate                             *vdp_video_mixer_create;
VdpVideoMixerDestroy                            *vdp_video_mixer_destroy;
VdpVideoMixerRender                             *vdp_video_mixer_render;
VdpVideoMixerSetFeatureEnables                  *vdp_video_mixer_set_feature_enables;
VdpVideoMixerSetAttributeValues                 *vdp_video_mixer_set_attribute_values;

VdpOutputSurfaceCreate                          *vdp_output_surface_create;
VdpOutputSurfaceDestroy                         *vdp_output_surface_destroy;
VdpOutputSurfaceQueryCapabilities               *vdp_output_surface_query_capabilities;
VdpOutputSurfaceGetBitsNative                   *vdp_output_surface_get_bits_native;

VdpPresentationQueueTargetCreateX11             *vdp_presentation_queue_target_create_x11;
VdpPresentationQueueTargetDestroy               *vdp_presentation_queue_target_destroy;

VdpPresentationQueueCreate                      *vdp_presentation_queue_create;
VdpPresentationQueueDestroy                     *vdp_presentation_queue_destroy;
VdpPresentationQueueDisplay                     *vdp_presentation_queue_display;
VdpPresentationQueueBlockUntilSurfaceIdle       *vdp_presentation_queue_block_until_surface_idle;
VdpPresentationQueueSetBackgroundColor          *vdp_presentation_queue_set_background_color;
VdpPresentationQueueQuerySurfaceStatus          *vdp_presentation_queue_query_surface_status;

VdpGetInformationString                         *vdp_get_information_string;

int init_x11() {
    x11_display = XOpenDisplay((char*)0); //open display 0
    if (!x11_display) {
        fprintf(stderr, "Failed to open XDisplay\n");
        return -1;
    }
    x11_screen = DefaultScreen(x11_display);
    printf("X11 Display created\n");
    return 0;
}

VdpStatus init_vdpau() {
    VdpStatus retval = VDP_STATUS_OK;

    struct VdpFunction {
        const int id;
        void *pointer;
    };

    struct VdpFunction vdp_function[] = {
        {VDP_FUNC_ID_DEVICE_DESTROY, &vdp_device_destroy},
        {VDP_FUNC_ID_VIDEO_SURFACE_CREATE,
            &vdp_video_surface_create},
        {VDP_FUNC_ID_VIDEO_SURFACE_DESTROY,
            &vdp_video_surface_destroy},
        {VDP_FUNC_ID_VIDEO_SURFACE_QUERY_CAPABILITIES,
            &vdp_video_surface_query_capabilities},
        {VDP_FUNC_ID_VIDEO_SURFACE_QUERY_GET_PUT_BITS_Y_CB_CR_CAPABILITIES,
            &vdp_video_surface_query_ycbcr_capabilities},
        {VDP_FUNC_ID_VIDEO_SURFACE_GET_BITS_Y_CB_CR,
            &vdp_video_surface_get_bits_ycbcr},
        {VDP_FUNC_ID_VIDEO_SURFACE_PUT_BITS_Y_CB_CR,
            &vdp_video_surface_put_bits_ycbcr},
        {VDP_FUNC_ID_VIDEO_SURFACE_GET_PARAMETERS,
            &vdp_video_surface_get_parameters},
        {VDP_FUNC_ID_DECODER_CREATE, &vdp_decoder_create},
        {VDP_FUNC_ID_DECODER_RENDER, &vdp_decoder_render},
        {VDP_FUNC_ID_DECODER_DESTROY, &vdp_decoder_destroy},
        {VDP_FUNC_ID_DECODER_QUERY_CAPABILITIES,
            &vdp_decoder_query_capabilities},
        {VDP_FUNC_ID_DECODER_GET_PARAMETERS,
            &vdp_decoder_get_parameters},
        {VDP_FUNC_ID_VIDEO_MIXER_CREATE, &vdp_video_mixer_create},
        {VDP_FUNC_ID_VIDEO_MIXER_DESTROY, &vdp_video_mixer_destroy},
        {VDP_FUNC_ID_VIDEO_MIXER_RENDER, &vdp_video_mixer_render},
        {VDP_FUNC_ID_VIDEO_MIXER_SET_FEATURE_ENABLES,
            &vdp_video_mixer_set_feature_enables},
        {VDP_FUNC_ID_VIDEO_MIXER_SET_ATTRIBUTE_VALUES,
            &vdp_video_mixer_set_attribute_values},
        {VDP_FUNC_ID_OUTPUT_SURFACE_CREATE, &vdp_output_surface_create},
        {VDP_FUNC_ID_OUTPUT_SURFACE_DESTROY, &vdp_output_surface_destroy},
        {VDP_FUNC_ID_OUTPUT_SURFACE_QUERY_CAPABILITIES,
            &vdp_output_surface_query_capabilities},
        {VDP_FUNC_ID_OUTPUT_SURFACE_GET_BITS_NATIVE,
            &vdp_output_surface_get_bits_native},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_CREATE_X11,
            &vdp_presentation_queue_target_create_x11},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_DESTROY,
            &vdp_presentation_queue_target_destroy},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_CREATE,
            &vdp_presentation_queue_create},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_DESTROY,
            &vdp_presentation_queue_destroy},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_DISPLAY,
            &vdp_presentation_queue_display},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_BLOCK_UNTIL_SURFACE_IDLE,
            &vdp_presentation_queue_block_until_surface_idle},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_SET_BACKGROUND_COLOR,
            &vdp_presentation_queue_set_background_color},
        {VDP_FUNC_ID_PRESENTATION_QUEUE_QUERY_SURFACE_STATUS,
            &vdp_presentation_queue_query_surface_status},
        {VDP_FUNC_ID_GET_INFORMATION_STRING, &vdp_get_information_string},
        {0,NULL}
    };

    retval = vdp_device_create_x11(x11_display, x11_screen, &vdp_device,&vdp_get_proc_address);
    if (retval != VDP_STATUS_OK) {
        fprintf(stderr, "Failed to create X11 device");
        return -1;
    }

    fprintf(stdout,"X11 device created\n");
    const struct VdpFunction *dsc;
    for (dsc = vdp_function; dsc->pointer; dsc++) {
        retval = vdp_get_proc_address(vdp_device, dsc->id,dsc->pointer);
        if (retval != VDP_STATUS_OK) {
            fprintf(stderr,"Failed to link function id: %d\n",dsc->id);
            return retval;
        } 
    }
    fprintf(stdout,"Linked all functions\n");

    return retval;
}

int init_decoder() {

}

int main() {
    int retval;
    const char *info;
    VdpStatus vdpret = VDP_STATUS_OK;

    retval = init_x11();
    if (retval) {
        fprintf(stderr,"Failed to init X11");
        return -1;
    }
    vdpret = init_vdpau();
    if (vdpret != VDP_STATUS_OK) {
        fprintf(stderr,"Failed to init VDPAU");
        return -1;
    }
    vdpret = vdp_get_information_string(&info);
    printf("%s\n",info);
}
