/*
 * FFmpeg + SDL2 excercises
 * 
 * @author ars.kov@gmail.com
 * 
 */
#include <stdio.h>
#include <SDL2/SDL.h>
#include "webcam.h"

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

static SDL_Window *pWindow = NULL;
static SDL_Renderer *pRenderer = NULL;
static SDL_Texture *pTexture = NULL;
static SDL_Event event;

static void frameCallback(AVCodecContext *pCodecContext, AVFrame *pFrame)
{
    // printf("got frame: %li\n", pFrame->best_effort_timestamp);
    SDL_UpdateTexture(pTexture, NULL, pFrame->data[0], pFrame->linesize[0]);
    SDL_RenderClear(pRenderer);
    SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);
    SDL_RenderPresent(pRenderer);
}

int main(int argc, char *argv[])
{

    if (webcam_init("/dev/video0", &frameCallback) < 0)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Webcam initialization error\n");
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "SDL initialization error: %s\n",
            SDL_GetError());
        goto end;
    }

    pWindow = SDL_CreateWindow(
        "WebCam Tutorial",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (pWindow == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Window could not be created! SDL_Error: %s\n",
             SDL_GetError());
        goto end;
    }

    pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
    if (pRenderer == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Renderer could not be created! SDL_Error: %s\n",
             SDL_GetError());
        goto end;
    }

    pTexture = SDL_CreateTexture(
        pRenderer,
        SDL_PIXELFORMAT_YUY2,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);
    
    while (1)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
            break;
        webcam_get_frame();
    }

    end:
    webcam_destroy();
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}