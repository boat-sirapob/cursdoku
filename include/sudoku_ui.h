#ifndef SUDOKU_UI_H
#define SUDOKU_UI_H

#include "window.h"
#include "sudoku.h"

Window* initialize_board_window();
void print_board(Window* window, SudokuGrid* board);
void run_board(Window* window, SudokuGrid* board);

#endif

