#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include "window.h"

typedef void (*menu_callback)(void);

typedef struct MenuChoice {
    char* text;
    menu_callback callback;
} MenuChoice;

typedef struct MenuWindow {
    Window* window;

    char* title;
    MenuChoice* choices;
    int num_choices;
} MenuWindow;

void menu_quit();
void menu_stub();

void initialize_menu(MenuWindow* menu, char title[], MenuChoice choices[], int num_choices);
void print_menu(MenuWindow* menu,  int highlight);
void run_menu(MenuWindow* menu);

#endif // MENU_H