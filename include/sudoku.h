#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdbool.h>
#include <stdint.h>
#include "window.h"

typedef struct SudokuGrid {
    int grid[9][9];
    int initial_grid[9][9];
} SudokuGrid;

void initialize_grid(SudokuGrid* board, int grid[9][9]);

bool cell_is_changeable(SudokuGrid* board, int x, int y);
void set_cell(SudokuGrid* board, int x, int y, int value);
bool is_row_valid(SudokuGrid* board, int row);
bool is_col_valid(SudokuGrid* board, int col);
bool is_block_valid(SudokuGrid* board, int x, int y);
uint16_t get_possible_values(SudokuGrid* board, int x, int y);

int count_filled_cells(SudokuGrid* board);
int count_filled_cols(SudokuGrid* board);
int count_filled_rows(SudokuGrid* board);
int solve_board(SudokuGrid* board, Window* board_window);

#endif