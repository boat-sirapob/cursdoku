#include <ncurses.h>
#include <string.h>
#include "menu.h"

const int MENU_PADDING_X = 3;
const int MENU_PADDING_Y = 2;

// hacky??
void menu_quit() {}
void menu_stub() {}

void initialize_menu(MenuWindow* menu, char title[], MenuChoice choices[], int num_choices) {

    // calculate width and height of menu
    menu->width = 0;
    int cur_width;
    for (int i = 0; i < num_choices; i++) {
		cur_width = strlen(choices[i].text);
		if (cur_width > menu->width) {
			menu->width = cur_width;
		}
	}
    menu->width = menu->width + MENU_PADDING_X*2;
    menu->height = num_choices + 1 + MENU_PADDING_Y*2;

    int screen_width;
	int screen_height;
	getmaxyx(stdscr, screen_height, screen_width);

    menu->window = newwin(menu->height, menu->width, (screen_height-menu->height) / 2, (screen_width-menu->width) / 2);
	keypad(menu->window, TRUE);

    menu->title = title;
    menu->choices = choices;
    menu->num_choices = num_choices;
}

void print_menu(MenuWindow* menu,  int highlight) {
	// draw border around window
	wborder(menu->window, '|', '|', '-', '-', '+', '+', '+', '+');

	// draw title
	wattron(menu->window, A_BOLD);
	const int TITLE_X = (menu->width - strlen(menu->title)) / 2;
	mvwprintw(menu->window, MENU_PADDING_Y, TITLE_X, "%s", menu->title);
	wattroff(menu->window, A_BOLD);

	// draw choices
	for (int i = 0; i < menu->num_choices; i++) {
		if (i == highlight) {
			wattron(menu->window, A_REVERSE);
		}
        mvwprintw(menu->window, MENU_PADDING_Y + 1 + i, MENU_PADDING_X, "%s", menu->choices[i].text);
        wattroff(menu->window, A_REVERSE);
	}

	wrefresh(menu->window);
}

void handle_menu_resize(MenuWindow* menu) {
    int screen_height;
    int screen_width;

    getmaxyx(stdscr, screen_height, screen_width);
    
    // mvwprintw(stdscr, 0, 0, "%d %d", menu_x, menu_y);

    mvwin(menu->window, (screen_height - menu->height) / 2, (screen_width - menu->width) / 2);
}

void run_menu(MenuWindow* menu) {
	bool running = true;
	int chr;
	int selected = 0;
	while (running) {

		print_menu(menu, selected);

		chr = wgetch(menu->window);
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
				handle_menu_resize(menu);
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
