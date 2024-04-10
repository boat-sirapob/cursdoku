#include <ncurses.h>
#include "menu.h"

void do_nothing() {}

char main_menu_title[] = "-= SUDOKU =-";
MenuChoice main_menu_choices[] = {
	{"Play Random Board", do_nothing}, 
	{"Solve Board      ", do_nothing}
};
const int main_menu_num_choices = 2;

int main() {
	
	// initialization
    initscr();              // start curses mode
	cbreak();               // disable line buffering
	keypad(stdscr, TRUE);   // for arrow keys and function keys
	noecho();               // prevent echoing user chars
	curs_set(0);			// hide cursor

	int screen_width;
	int screen_height;
	getmaxyx(stdscr, screen_height, screen_width);

	MenuWindow main_menu;

	initialize_menu(
		&main_menu,
		main_menu_title,
		main_menu_choices,
		main_menu_num_choices
	);

	run_menu(&main_menu);

    // cleanup
	delwin(main_menu.window);
	endwin();

    return 0;
}
