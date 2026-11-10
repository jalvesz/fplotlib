#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "window.h"

struct fp_window {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width, height;
};

static void report_error(void)
{
    fprintf(stderr, "fplotlib SDL3: %s\n", SDL_GetError());
}

void fp_window_destroy(fp_window *w)
{
    if (!w) return;
    SDL_DestroyTexture(w->texture);
    SDL_DestroyRenderer(w->renderer);
    SDL_DestroyWindow(w->window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    free(w);
}

static int redraw(fp_window *w)
{
    int width, height;
    SDL_FRect dst;
    float scale;

    if (!SDL_GetRenderOutputSize(w->renderer, &width, &height)) return 1;
    if (width <= 0 || height <= 0) return 0;
    scale = SDL_min((float)width / w->width, (float)height / w->height);
    dst.w = w->width * scale;
    dst.h = w->height * scale;
    dst.x = (width - dst.w) / 2;
    dst.y = (height - dst.h) / 2;
    return !SDL_SetRenderDrawColor(w->renderer, 32, 32, 32, 255) ||
           !SDL_RenderClear(w->renderer) ||
           !SDL_RenderTexture(w->renderer, w->texture, NULL, &dst) ||
           !SDL_RenderPresent(w->renderer);
}

int fp_window_present(fp_window *w, const unsigned char *pixels, int width, int height)
{
    SDL_Texture *texture;

    if (width != w->width || height != w->height || !w->texture) {
        texture = SDL_CreateTexture(w->renderer, SDL_PIXELFORMAT_RGB24,
                                    SDL_TEXTUREACCESS_STATIC, width, height);
        if (!texture) goto error;
        SDL_DestroyTexture(w->texture);
        w->texture = texture;
        w->width = width;
        w->height = height;
    }
    if (!SDL_UpdateTexture(w->texture, NULL, pixels, width * 3) || redraw(w)) goto error;
    return 0;
error:
    report_error();
    return 1;
}

fp_window *fp_window_create(const char *title, const unsigned char *pixels, int width, int height)
{
    fp_window *w;

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        report_error();
        return NULL;
    }
    w = calloc(1, sizeof(*w));
    if (!w) {
        fprintf(stderr, "fplotlib SDL3: cannot allocate window\n");
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return NULL;
    }
    w->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!w->window) goto error;
    w->renderer = SDL_CreateRenderer(w->window, "software");
    if (!w->renderer) goto error;
    if (fp_window_present(w, pixels, width, height)) {
        fp_window_destroy(w);
        return NULL;
    }
    return w;
error:
    report_error();
    fp_window_destroy(w);
    return NULL;
}

int fp_window_poll(fp_window *w)
{
    SDL_Event event;
    bool dirty = false;
    SDL_WindowID id = SDL_GetWindowID(w->window);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == id) ||
            (event.type == SDL_EVENT_KEY_DOWN && event.key.windowID == id && event.key.key == SDLK_ESCAPE))
            return 0;
        if ((event.type == SDL_EVENT_WINDOW_EXPOSED ||
             event.type == SDL_EVENT_WINDOW_RESIZED ||
             event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) && event.window.windowID == id)
            dirty = true;
    }
    if (dirty && redraw(w)) {
        report_error();
        return -1;
    }
    return 1;
}

void fp_window_pause(void)
{
    SDL_Delay(16);
}
