#include "sudoku.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ncurses.h>

// -- DEBUG --
char* toBinary(int n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len);
    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\0';
    return binary;
}
// -----------

void initialize_grid(SudokuGrid* board, int grid[9][9]) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            board->grid[row][col] = grid[row][col];
            board->initial_grid[row][col] = grid[row][col];
        }
    }
}

bool cell_is_changeable(SudokuGrid* board, int x, int y) {
    return board->initial_grid[y][x] == 0;
}

void set_cell(SudokuGrid* board, int x, int y, int value) {
    if (cell_is_changeable(board, x, y)) {
        board->grid[y][x] = value;
    }
}

bool is_row_valid(SudokuGrid* board, int row) {
    uint16_t valid = 0b111111111; // 123456789
    
    for (int col = 0; col < 9; col++) {
        int cell_val = board->grid[row][col];
        if (cell_val == 0) { continue; }

        uint16_t mask = 1 << (cell_val-1);
        if ((valid & mask) == 0) { return false; }
        valid ^= mask;
    }

    mvprintw(4, 0, "ROW VALID: %d", valid);
    refresh();

    return true;
}

bool is_col_valid(SudokuGrid* board, int col) {
    uint16_t valid = 0b111111111; // 123456789

    for (int row = 0; row < 9; row++) {
        int cell_val = board->grid[row][col];
        if (cell_val == 0) { continue; }

        uint16_t mask = 1 << (cell_val-1);
        if ((valid & mask) == 0) { return false; }
        valid ^= mask;
    }


    return true;
}

bool is_block_valid(SudokuGrid* board, int x, int y) {
    uint16_t valid = 0b111111111; // 123456789

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            
            int cell_val = board->grid[row+y*3][col+x*3];
            if (cell_val == 0) { continue; }

            uint16_t mask = 1 << (cell_val-1);
            if ((valid & mask) == 0) { return false; }
            valid ^= mask;
        }
    }

    return true;
}

uint16_t get_possible_values(SudokuGrid* board, int x, int y) {
    uint16_t possible = 0b111111111; // 123456789

    // check the col
    for (int row = 0; row < 9; row++) {
        if (row == y) { continue; }

        int cell_val = board->grid[row][x];
        if (cell_val == 0) { continue; }

        uint16_t mask = 1 << (cell_val-1);
        if (possible & mask) {
            possible ^= mask;
        }
    }
    
    // check the row
    for (int col = 0; col < 9; col++) {
        if (col == x) { continue; }

        int cell_val = board->grid[y][col];
        if (cell_val == 0) { continue; }

        uint16_t mask = 1 << (cell_val-1);
        if (possible & mask) {
            possible ^= mask;
        }
    }

    // check the block
    int block_row = y / 3;
    int block_col = x / 3;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int grid_row = block_row*3 + row;
            int grid_col = block_col*3 + col;

            if (grid_row == y && grid_col == x) { continue; }

            int cell_val = board->grid[grid_row][grid_col];
            if (cell_val == 0) { continue; }

            uint16_t mask = 1 << (cell_val-1);
            if (possible & mask) {
                possible ^= mask;
            }
        }
    }

    return possible;
}
