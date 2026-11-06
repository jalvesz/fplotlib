#include <SDL3/SDL.h>
#include <stdio.h>

static float clamp(float x, float lo, float hi)
{
    return x < lo ? lo : (x > hi ? hi : x);
}

int fplotlib_sdl3_view(const unsigned char *pixels, int width, int height, int smoke)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event event;
    SDL_FRect src, dst;
    float zoom = 1.0f, cx = width / 2.0f, cy = height / 2.0f;
    int out_w, out_h, status = 1;
    bool running = true, video_initialized = false;

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) goto error;
    video_initialized = true;
    if (!SDL_CreateWindowAndRenderer("fplotlib SDL3 prototype", width, height,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer)) goto error;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture || !SDL_UpdateTexture(texture, NULL, pixels, width * 3)) goto error;

    while (running) {
        src.w = width / zoom;
        src.h = height / zoom;
        cx = clamp(cx, src.w / 2, width - src.w / 2);
        cy = clamp(cy, src.h / 2, height - src.h / 2);
        src.x = cx - src.w / 2;
        src.y = cy - src.h / 2;
        if (!SDL_GetRenderOutputSize(renderer, &out_w, &out_h)) goto error;
        if (out_w > 0 && out_h > 0) {
            float scale = SDL_min((float)out_w / width, (float)out_h / height);
            dst.w = width * scale;
            dst.h = height * scale;
            dst.x = (out_w - dst.w) / 2;
            dst.y = (out_h - dst.h) / 2;
            if (!SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255) ||
                !SDL_RenderClear(renderer) ||
                !SDL_RenderTexture(renderer, texture, &src, &dst) ||
                !SDL_RenderPresent(renderer)) goto error;
        }
        if (smoke) {
            SDL_zero(event);
            event.type = SDL_EVENT_QUIT;
            if (!SDL_PushEvent(&event)) goto error;
            smoke = 0;
        }
        if (!SDL_WaitEvent(&event)) goto error;
        switch (event.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            running = false;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            zoom = clamp(zoom * (event.wheel.y > 0 ? 1.25f :
                                 event.wheel.y < 0 ? 0.8f : 1.0f), 1.0f, 16.0f);
            break;
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.key) {
            case SDLK_ESCAPE: running = false; break;
            case SDLK_PLUS: case SDLK_EQUALS: zoom = clamp(zoom * 1.25f, 1.0f, 16.0f); break;
            case SDLK_MINUS: zoom = clamp(zoom * 0.8f, 1.0f, 16.0f); break;
            case SDLK_0: zoom = 1.0f; cx = width / 2.0f; cy = height / 2.0f; break;
            case SDLK_LEFT: cx -= src.w * 0.1f; break;
            case SDLK_RIGHT: cx += src.w * 0.1f; break;
            case SDLK_UP: cy -= src.h * 0.1f; break;
            case SDLK_DOWN: cy += src.h * 0.1f; break;
            }
            break;
        }
    }
    status = 0;
    goto cleanup;

error:
    fprintf(stderr, "fplotlib SDL3: %s\n", SDL_GetError());
cleanup:
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (video_initialized) SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return status;
}
