#include <ncurses.h>
#include "menu.h"

int main() {
	
	// initialization
    initscr();              // start curses mode
	// raw();
	cbreak();               // disable line buffering
	keypad(stdscr, TRUE);   // for arrow keys and function keys
	noecho();               // prevent echoing user chars
	curs_set(0);			// hide cursor

	// initialize main menu
	MenuWindow main_menu;

	char main_menu_title[] = "-= SUDOKU =-";
	MenuChoice main_menu_choices[] = {
		{"1 Play Random Board", menu_stub},
		{"2 Solve Board      ", menu_stub},
		{"3 Exit             ", menu_quit}
	};
	const int main_menu_num_choices = 3;

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
