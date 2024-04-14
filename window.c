#include "window.h"
#include <ncurses.h>
#include <stdlib.h>

Window* initialize_window(int width, int height) {

	Window* window = (Window*)malloc(sizeof(Window));

    window->width = width;
    window->height = height;

    window->window = newwin(window->height, window->width, 0, 0);
    keypad(window->window, TRUE);

    return window;
}

void center_window(Window* window) {
    int screen_height;
    int screen_width;

    getmaxyx(stdscr, screen_height, screen_width);
    
    // mvwprintw(stdscr, 0, 0, "%d %d", menu_x, menu_y);

    mvwin(window->window, (screen_height - window->height) / 2, (screen_width - window->width) / 2);
}

void hcenter_window(Window* window, int y) {
    int screen_width = getmaxx(stdscr);

    mvwin(window->window, y, (screen_width - window->width) / 2);
}

void vcenter_window(Window* window, int x) {
    int screen_height = getmaxy(stdscr);

    mvwin(window->window, (screen_height - window->height) / 2, x);
}

void delete_window(Window* window) {
	delwin(window->window);
	free(window);
}