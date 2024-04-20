#include "window.h"
#include "sudoku.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>

const int SUDOKU_WINDOW_HEIGHT = 9 + 2;
const int SUDOKU_WINDOW_WIDTH = 9*2 + 3;

const bool SHOW_POSSIBLE_FLAG = false;

#define CHANGEABLE_COLOR 1
#define INVALID_COLOR 2

Window* initialize_board_window() {
	Window* window = initialize_window(SUDOKU_WINDOW_WIDTH, SUDOKU_WINDOW_HEIGHT);
	center_window(window);

	return window;
}

void print_board(Window* board_window, SudokuGrid* board, int selected_x, int selected_y) {
	WINDOW* window = board_window->window;

	// vertical lines
	mvwvline(window, 0, 6, ACS_VLINE, SUDOKU_WINDOW_HEIGHT);
	mvwvline(window, 0, 14, ACS_VLINE, SUDOKU_WINDOW_HEIGHT);

	// horizontal lines
	mvwhline(window, 3, 0, ACS_HLINE, SUDOKU_WINDOW_WIDTH);
	mvwhline(window, 7, 0, ACS_HLINE, SUDOKU_WINDOW_WIDTH);

	// intersection points
	mvwaddch(window, 3, 6, ACS_PLUS);
	mvwaddch(window, 3, 14, ACS_PLUS);
	mvwaddch(window, 7, 6, ACS_PLUS);
	mvwaddch(window, 7, 14, ACS_PLUS);

	// check validity
	uint16_t invalid_cols = 0b000000000;
	uint16_t invalid_rows = 0b000000000;
	uint16_t invalid_blocks = 0b000000000;

	// check cols
	for (int col = 0; col < 9; col++) {
		invalid_cols |= (!is_col_valid(board, col)) << col;
	}

	// check rows
	for (int row = 0; row < 9; row++) {
		invalid_rows |= (!is_row_valid(board, row)) << row;
	}

	// check blocks
	for (int block_row = 0; block_row < 3; block_row++) {
		for (int block_col = 0; block_col < 3; block_col++) {
			invalid_blocks |= (!is_block_valid(board, block_col, block_row)) << (block_row*3 + block_col);
		}
	}

	// handling colors
	bool color_flag;
	if (has_colors()) {
		color_flag = true;
		
		start_color();
		use_default_colors();
		
		init_pair(CHANGEABLE_COLOR, COLOR_MAGENTA, -1);
		init_pair(INVALID_COLOR, COLOR_RED, -1);
	}

	// draw each number
	bool invalid_row_flag = false;

	int cell;
	int draw_cell;
	int y_offset;
	int x_offset;
	// each row
	for (int y = 0; y < 9; y++) {
		// offset for horizontal grid lines
		y_offset = y / 3;

		// color invalid rows
		invalid_row_flag = false;
		if ((invalid_rows & (1 << y)) && color_flag) {
			invalid_row_flag = true;
		}

		// each col
		for (int x = 0; x < 9; x++) {
			// offset for vertical grid lines
			x_offset = x / 3;

			// color invalid cells
			if (color_flag && (
					invalid_row_flag || 							// invalid row
					(invalid_cols & (1 << x)) ||					// invalid column
					(invalid_blocks & (1 << (x/3 + 3*(y/3)))))) { 	// invalid block
				wattron(window, COLOR_PAIR(INVALID_COLOR));
			}
			
			// color changable cells
			else if (cell_is_changeable(board, x, y)) {
				wattron(window, COLOR_PAIR(CHANGEABLE_COLOR));
			}

			// highlight selected cell
			if (x == selected_x && y == selected_y) {
				wattron(window, A_REVERSE);
			}

			// print the cell
			cell = board->grid[y][x];
			
			if (cell == 0) { draw_cell = ' '; }
			else { draw_cell = '0' + cell; }

			mvwaddch(window, y+y_offset, (x+x_offset)*2, draw_cell);

			// reset attributes
			wattroff(window, A_REVERSE);
			if (color_flag) {
				wattroff(window, COLOR_PAIR(CHANGEABLE_COLOR));
				wattroff(window, COLOR_PAIR(INVALID_COLOR));
			}
		}
	}

	// board details
	// 1. print number of cells filled
	mvprintw(0, 0, "Filled Cells: %d/81", count_filled_cells(board));
	// 2. print number of cols completed
	mvprintw(1, 0, "Filled Cols: %d/9", count_filled_cols(board));
	// 3. print number of rows completed
	mvprintw(2, 0, "Filled Rows: %d/9", count_filled_rows(board));

	// show possible numbers of current cell
	if (SHOW_POSSIBLE_FLAG) {
		uint16_t possible_values = get_possible_values(board, selected_x, selected_y);

		int center_x = (getmaxx(stdscr) - (9*2-1)) / 2;

		mvprintw(4, 0, "POSSIBLE_VALUES: %d", possible_values);

		for (int num = 1; num <= 9; num++) {
			if (!(possible_values & (1 << (num-1)))) {
				attron(A_DIM);
			} else {
				attron(A_BOLD);
				attron(A_UNDERLINE);
			}

			mvaddch(getmaxy(stdscr)-1, center_x + (num-1) * 2, '0' + num);
			
			// reset attrs
			attroff(A_DIM);
			attroff(A_BOLD);
			attroff(A_UNDERLINE);
		}
	}

	refresh();
}

void run_board(Window* window, SudokuGrid* board) {
    clear();
	mvprintw(0, 0, "PRESS ESC TO EXIT");
	refresh();

	int selected_x = 0;
	int selected_y = 0;

	bool running = true;
	int chr;
	while (running) {
		// display board
		print_board(window, board, selected_x, selected_y);

		chr = wgetch(window->window);
		clear();

		// handle input
		switch (chr) {
			case KEY_UP: {
				if (selected_y > 0) { selected_y -= 1; }
				break;
			}
			case KEY_DOWN: {
				if (selected_y < 9-1) { selected_y += 1; }
				break;
			}
			case KEY_LEFT: {
				if (selected_x > 0) { selected_x -= 1; }
				break;
			}
			case KEY_RIGHT: {
				if (selected_x < 9-1) { selected_x += 1; }
				break;
			}
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9': {
				set_cell(board, selected_x, selected_y, chr-'0');
				break;
			}
			case KEY_BACKSPACE: {
				set_cell(board, selected_x, selected_y, 0);
				break;
			}
			case '\n': {
				// TODO: let user select number to fill current cell
                break;
			}
			case 's': {
				// solve the board
				if (solve_board(board, window) == -1) {
					mvprintw(getmaxy(stdscr), 0, "Board is invalid - not solvable");
					refresh();
				}
				break;
			}
			case KEY_RESIZE: {
				center_window(window);
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
		// mvprintw(0, 0, "SELECTED: %d %d", selected_x, selected_y);

		refresh();
	}
}
