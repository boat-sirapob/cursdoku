CC := gcc
CFLAGS := -Wall
LDFLAGS := -lncurses
SOURCES := main.c menu.c window.c sudoku.c sudoku_ui.c
TARGET := cursedoku

$(TARGET):
	$(CC) $(CFLAGS) -o build/$(TARGET) $(SOURCES) $(LDFLAGS)