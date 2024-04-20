#include "sudoku.h"
#include "sudoku_ui.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define BOARD_SOLVABLE_CODE 0
#define BOARD_UNSOLVABLE_CODE -1

const bool DISPLAY_SOLVE = true;
const int SOLVE_DELAY_MS = 50;

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

bool is_board_valid(SudokuGrid* board) {
    // check cols
    for (int col = 0; col < 9; col++) {
        if (!is_col_valid(board, col)) { return false; };
    }

    // check rows
    for (int row = 0; row < 9; row++) {
        if (!is_row_valid(board, row)) { return false; };
    }

    // check block
    for (int block_row = 0; block_row < 3; block_row++) {
        for (int block_col = 0; block_col < 3; block_col++) {
            if (!is_block_valid(board, block_col, block_row)) { return false; };
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

int count_filled_cells(SudokuGrid* board) {
    int count = 0;
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (board->grid[row][col] != 0) { count += 1; }
        }
    }

    return count;
}

int count_filled_rows(SudokuGrid* board) {
    int count = 0;
    for (int row = 0; row < 9; row++) {
        bool filled = true;
        for (int col = 0; col < 9; col++) {
            if (board->grid[row][col] == 0) {
                filled = false;
                break;
            }
        }
        if (filled) { count += 1; }
    }

    return count;
}

int count_filled_cols(SudokuGrid* board) {
    int count = 0;
    for (int col = 0; col < 9; col++) {
        bool filled = true;
        for (int row = 0; row < 9; row++) {
            if (board->grid[row][col] == 0) {
                filled = false;
                break;
            }
        }
        if (filled) { count += 1; }
    }

    return count;
}

// TODO: move this to utils file
void sleep_ms(int ms) {
    struct timespec time_sleep;

    const int sec = ms / (1000*1000*100);

    time_sleep.tv_sec = sec;
    time_sleep.tv_nsec = (ms-sec) * 1000*1000; // ns = ms * 10^6

    nanosleep(&time_sleep, &time_sleep);

}

bool solve_helper(SudokuGrid* board, Window* board_window, int starting_cell) {
    // assuming current board state is valid

    // starting_cell = 0 to 81
    // x = cell MOD 9
    // y = cell DIV 9

    // TODO: maybe go through and fill each cell with only one possible value first?

    // go to first empty cell
    while (starting_cell < 9*9) {
        const int x = starting_cell % 9;
        const int y = starting_cell / 9;
        const int cell_val = board->grid[y][x];
        if (cell_val == 0) {
            break;
        }
        starting_cell += 1;
    }
    if (starting_cell == 9*9) {
        // went through every cell of the board
        // thus solved so we return
        return true;
    }

    const int empty_cell_col = starting_cell % 9;
    const int empty_cell_row = starting_cell / 9;

    // get possible values
    const uint16_t possible_vals = get_possible_values(board, empty_cell_col, empty_cell_row);

    mvprintw(5, 0, "POSSIBLE: %s", toBinary(possible_vals, 9));
    refresh();

    // try each possible value
    for (int val = 1; val <= 9; val++) {
        uint16_t mask = 1 << (val-1);
        if ((possible_vals & mask) == 0) { continue; }

        board->grid[empty_cell_row][empty_cell_col] = val;
        
        // display solve process
        if (DISPLAY_SOLVE && board_window != NULL) {
            print_board(board_window, board, -1, -1);
            wrefresh(board_window->window);
            
            sleep_ms(SOLVE_DELAY_MS);        
        }

        if (solve_helper(board, board_window, starting_cell + 1)) {
            return true;
        }
    }
    if (DISPLAY_SOLVE && board_window != NULL) {
        print_board(board_window, board, -1, -1);
        wrefresh(board_window->window);
        
        sleep_ms(SOLVE_DELAY_MS);        
    }

    // went through every value, not solvable so backtrack
    board->grid[empty_cell_row][empty_cell_col] = 0;
    return false;
}

int solve_board(SudokuGrid* board, Window* board_window) {
    // check first if board is valid
    if (!is_board_valid(board)) {
        return BOARD_UNSOLVABLE_CODE;
    }

    // board valid so solve the board starting from the top left
    solve_helper(board, board_window, 0);

    return BOARD_SOLVABLE_CODE;
}
