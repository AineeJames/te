#ifndef GRID_H_
#define GRID_H_

#include "raylib.h"
#include <stddef.h>

typedef struct {
  unsigned char glyph, fg, bg;
} Cell;

typedef struct {
  size_t w, h;
  Cell cells[]; // flexible array member
} Grid;

Grid *grid_init(int w, int h);
void grid_set(Grid *grid, size_t x, size_t y, Cell cell);
void grid_fill(Grid *grid, Cell cell);
Texture grid_render_texture(Grid *grid);
void grid_free(Grid *grid);

#endif // GRID_H_
