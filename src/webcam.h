/*
 * FFmpeg + SDL2 excercises
 * 
 * @author ars.kov@gmail.com
 * 
 */
#ifndef MOD_WEBCAM_H
#define MOD_WEBCAM_H

#include <libavcodec/avcodec.h>

typedef void (*FrameCallback) (AVCodecContext *ctx, AVFrame *frame);

typedef struct PacketData {
    void *data;
    int error;
} PacketData;

extern int     webcam_init(const char *deviceName, FrameCallback cbParam);
extern void    webcam_destroy();
extern int     webcam_get_frame();

#endif
