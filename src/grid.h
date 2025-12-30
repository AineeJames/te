#ifndef GRID_H_
#define GRID_H_

#include "colors.h"
#include "raylib.h"
#include <stddef.h>

typedef struct {
  unsigned char glyph, fg, bg;
} Cell;

#define CELL_EMPTY                                                             \
  (Cell) { .glyph = 0, .fg = VGA_BLACK, .bg = VGA_BLACK }

typedef struct {
  size_t w, h;
  Cell cells[]; // flexible array member
} Grid;

Grid *grid_init(int w, int h);
void grid_set(Grid *grid, size_t x, size_t y, Cell cell);
void grid_fill(Grid *grid, Cell cell);
Texture grid_render_texture(Grid *grid);
void grid_free(Grid *grid);
void grid_print(Grid *grid, size_t x, size_t y, const char *text);

#endif // GRID_H_
