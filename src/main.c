#include <ncurses.h>
#include "menu.h"
#include "sudoku.h"
#include "sudoku_ui.h"
#include "window.h"

typedef enum Difficulty {
	EASY,
	MEDIUM,
	HARD,
	EMPTY
} Difficulty;

void play_board(Difficulty difficulty) {
	// generate board
	// ...

	// example board
	SudokuGrid board;
	switch (difficulty) {
		case EASY: {

			int grid[9][9] = {
				{0,0,0, 1,7,0, 3,8,2},
				{2,0,0, 6,9,3, 0,0,0},
				{7,0,0, 0,0,0, 0,0,0},

				{4,5,0, 7,0,0, 9,3,0},
				{0,0,7, 0,3,0, 0,0,4},
				{8,0,0, 9,0,0, 2,6,7},

				{6,0,4, 5,0,0, 0,2,3},
				{0,8,5, 3,6,2, 7,4,0},
				{3,0,0, 0,8,0, 0,5,1}
			};

			initialize_grid(&board, grid);
			// memcpy(board.grid, grid, sizeof(grid));
			break;
		}
		case MEDIUM: {
			break;
		}
		case HARD: {
			break;
		}
		case EMPTY: {
			int grid[9][9];
			for (int row = 0; row < 9; row++) {
				for (int col = 0; col < 9; col++) {
					grid[row][col] = 0;
				}
			}

			initialize_grid(&board, grid);
			// memcpy(board.grid, grid, sizeof(grid));
			break;
		}
	}

	Window* board_window = initialize_board_window();
	
	run_board(board_window, &board);

	delete_window(board_window);
}

void play_easy() { play_board(EASY); }
void play_medium() { play_board(MEDIUM); }
void play_hard() { play_board(HARD); }
void play_empty() { play_board(EMPTY); }

void run_difficulty_menu() {
	MenuWindow difficulty_menu;
	char difficulty_menu_title[] = "- SELECT DIFFICULTY -";
	MenuChoice difficulty_menu_choices[] = {
		{"1 Easy", play_easy},
		{"2 Medium", play_medium},
		{"3 Hard", play_hard},
		{"4 Empty", play_empty},
		{"5 Back", menu_quit}
	};
	const int difficulty_menu_num_choices = 5;

	initialize_menu(
		&difficulty_menu,
		difficulty_menu_title,
		difficulty_menu_choices,
		difficulty_menu_num_choices
	);

	run_menu(&difficulty_menu);

	delete_window(difficulty_menu.window);
}

void run_main_menu() {

	// initialize main menu
	MenuWindow main_menu;

	char main_menu_title[] = "-= SUDOKU =-";
	MenuChoice main_menu_choices[] = {
		{"1 Play Random Board", run_difficulty_menu},
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

	delete_window(main_menu.window);
}

int main() {
	
	// initialization
    initscr();              // start curses mode
	// raw();
	cbreak();               // disable line buffering
	keypad(stdscr, TRUE);   // for arrow keys and function keys
	noecho();               // prevent echoing user chars
	curs_set(0);			// hide cursor

	run_main_menu();

	// TESTING
	// play_board(EASY);

	endwin();

    return 0;
}
