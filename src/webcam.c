/*
 * FFmpeg + SDL2 excercises
 * 
 * @author ars.kov@gmail.com
 * 
 */

#include "webcam.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>

static AVInputFormat *pInputFmt = NULL;
static AVDictionary *pOptions = NULL;
static AVFormatContext *pFormatCtx = NULL;
static AVDeviceInfoList **ppDeviceInfoList = NULL;
static AVFrame *pRawFrame = NULL;
static AVPacket rawPacket;
static int videoStream = -1;
static AVCodecParameters *pCodecParams1 = NULL;
static AVCodec *pCodec = NULL;
static AVCodecContext *pCodecCtx = NULL;
static FrameCallback cb;

int webcam_init(const char *deviceName, FrameCallback cbParam)
{
    cb = cbParam; 

    av_register_all();
    avdevice_register_all();
    pInputFmt = av_find_input_format("video4linux2");
    if (pInputFmt == NULL) {
        av_log(0, AV_LOG_ERROR, "Cannot find input format\n");
        return -1;
    }

    pFormatCtx = avformat_alloc_context();
    if (pFormatCtx == NULL)
    {
        av_log(0, AV_LOG_ERROR, "Cannot initialize context\n");
        return -1;
    }

    av_dict_set(&pOptions, "framerate", "30", 0);
    av_dict_set(&pOptions, "input_format", "yuyv422", 0);
    av_dict_set(&pOptions, "video_size", "640x480", 0);

    if (avformat_open_input(&pFormatCtx, deviceName, pInputFmt, &pOptions) < 0) 
    {
        av_log(0, AV_LOG_ERROR, "Cannot open input\n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {   
        av_log(0, AV_LOG_ERROR, "Cannot find stream info\n");
        return -1;
    }
    av_dump_format(pFormatCtx, 0, deviceName, 0);
        
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1)
    {
        av_log(0, AV_LOG_ERROR, "Didn't find a video stream\n");
        return -1;
    }
    
    pCodecParams1 = pFormatCtx->streams[videoStream]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParams1->codec_id);
    if (pCodec == NULL) {
        av_log(0, AV_LOG_ERROR, "Unsupported codec\n");
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL)
    {
        av_log(0, AV_LOG_ERROR, "Cannot allocate codec context\n");
        return -1;
    }

    if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) < 0)
    {
        av_log(0, AV_LOG_ERROR, "Couldn't create codec context from parameters\n");
        return -1;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        av_log(0, AV_LOG_ERROR, "Could not open codec\n");
        return -1;
    }
    pRawFrame = av_frame_alloc();
}

void webcam_destroy()
{
    av_packet_unref(&rawPacket);
    av_frame_unref(pRawFrame);
    avformat_close_input(&pFormatCtx);
    //avformat_free_context(pFormatCtx);
    avcodec_free_context(&pCodecCtx);
}

int webcam_get_frame()
{
    // This is what bothered me when I studied the API.
    // The function read_frame actually reads a packet not a frame :)
    // And when you whant to decode, you firstly send_packet and then receive_frame.
    int res = av_read_frame(pFormatCtx, &rawPacket);
    if (res < 0)
    {
        return -1;
    }

    if (rawPacket.stream_index == videoStream)
    {
        int frameFinished = 0;
        res = avcodec_send_packet(pCodecCtx, &rawPacket);
        // TODO : check error
        send_again:
        res = avcodec_receive_frame(pCodecCtx, pRawFrame);
        if (res == AVERROR(EAGAIN))
        {
            goto send_again;
        }
        else if (res < 0)
        {
            return -1;
        }

        if (cb)
        {
            cb(pCodecCtx, pRawFrame);
        }
    }
    av_packet_unref(&rawPacket);
    av_frame_unref(pRawFrame);
    return 0;
}

/*
static void frameCallback(AVFrame *frame)
{
    printf("Got packet in callback: %li\n", frame->best_effort_timestamp);
}

int main(int argc, char *argv[])
{
    if (webcam_init("/dev/video0", &frameCallback) < 0)
    {
        fprintf(stderr, "Something went wrong during init\n");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        int ret = webcam_get_frame();
        if (ret < 0)
        {
            fprintf(stderr, "Something went wrong during decode\n");
            exit(EXIT_FAILURE);
        }
    }
    webcam_destroy();
}
*/