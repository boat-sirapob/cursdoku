#include <ncurses.h>
#include <string.h>
#include "menu.h"
#include "window.h"

const int MENU_PADDING_X = 3;
const int MENU_PADDING_Y = 2;

// hacky??
void menu_quit() {}
void menu_stub() {}

void initialize_menu(MenuWindow* menu, char title[], MenuChoice choices[], int num_choices) {

    // calculate width and height of menu
    int width = strlen(title);
    int cur_width;
    for (int i = 0; i < num_choices; i++) {
		cur_width = strlen(choices[i].text);
		if (cur_width > width) {
			width = cur_width;
		}
	}
    width = width + MENU_PADDING_X*2;
    int height = num_choices + 1 + MENU_PADDING_Y*2;

	Window* window = initialize_window(width, height);
	center_window(window);

	menu->window = window;
    menu->title = title;
    menu->choices = choices;
    menu->num_choices = num_choices;
}

void print_menu(MenuWindow* menu,  int highlight) {
	// draw border around window
	wborder(menu->window->window, '|', '|', '-', '-', '+', '+', '+', '+');

	// draw title
	wattron(menu->window->window, A_BOLD);
	const int TITLE_X = (menu->window->width - strlen(menu->title)) / 2;
	mvwprintw(menu->window->window, MENU_PADDING_Y, TITLE_X, "%s", menu->title);
	wattroff(menu->window->window, A_BOLD);

	// draw choices
	for (int i = 0; i < menu->num_choices; i++) {
		if (i == highlight) {
			wattron(menu->window->window, A_REVERSE);
		}
        mvwprintw(menu->window->window, MENU_PADDING_Y + 1 + i, MENU_PADDING_X, "%s", menu->choices[i].text);
        wattroff(menu->window->window, A_REVERSE);
	}

	wrefresh(menu->window->window);
}

void run_menu(MenuWindow* menu) {
	clear();
	refresh();

	bool running = true;
	int chr;
	int selected = 0;
	while (running) {

		print_menu(menu, selected);

		chr = wgetch(menu->window->window);
		clear();

		switch (chr) {
			case KEY_UP: {
				if (selected > 0) {
					selected -= 1;
				}
				break;
			}
			case KEY_DOWN: {
				if (selected < menu->num_choices-1) {
					selected += 1;
				}
				break;
			}
			case '\n': {
                const menu_callback callback = menu->choices[selected].callback;
                if (callback == menu_quit) {
                    running = false;
                } else if (callback == menu_stub) {
                    mvprintw(0, 0, "Menu not implemented: %s", menu->choices[selected].text);
                } else {
                    menu->choices[selected].callback();
                }
                break;
			}
			case KEY_RESIZE: {
				center_window(menu->window);
				break;
			}
			case KEY_EXIT: {
				running = false;
				break;
			}
			default: {
				break;
			}
		}

		refresh();
	}
}
