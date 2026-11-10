#ifndef FP_WINDOW_H
#define FP_WINDOW_H

typedef struct fp_window fp_window;

fp_window *fp_window_create(const char *title, const unsigned char *pixels, int width, int height);
int fp_window_present(fp_window *window, const unsigned char *pixels, int width, int height);
/* Returns 1 while open, 0 on close, and -1 on error. */
int fp_window_poll(fp_window *window);
void fp_window_destroy(fp_window *window);
void fp_window_pause(void);

#endif
