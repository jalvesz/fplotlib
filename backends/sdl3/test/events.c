#include <SDL3/SDL.h>

int test_resize(void)
{
    int count;
    SDL_Window **windows = SDL_GetWindows(&count);
    bool ok = count == 1 && SDL_SetWindowSize(windows[0], 200, 150);
    SDL_free(windows);
    return !ok;
}

int test_quit(void)
{
    SDL_Event event;
    SDL_zero(event);
    event.type = SDL_EVENT_QUIT;
    return !SDL_PushEvent(&event);
}

static Uint32 post_quit(void *unused, SDL_TimerID id, Uint32 interval)
{
    (void)unused;
    (void)id;
    (void)interval;
    test_quit();
    return 0;
}

int test_schedule_quit(void)
{
    return SDL_AddTimer(200, post_quit, NULL) == 0;
}
