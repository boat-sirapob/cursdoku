#ifndef WINDOW_H
#define WINDOW_H

#include <ncurses.h>

typedef struct Window {
    WINDOW* window;
    int width;
    int height;
} Window;

Window* initialize_window(int width, int height);
void center_window(Window* window);
void delete_window(Window* window);

#endif // WINDOW_H