#include "grid.h"
#include <assert.h>
#include <stdlib.h>

Grid *grid_init(int w, int h) {
  Grid *grid = malloc(sizeof(Grid) + (w * h * sizeof(Cell)));
  assert(grid != NULL);

  grid->w = w;
  grid->h = h;

  return grid;
}

void grid_set(Grid *grid, size_t x, size_t y, Cell cell) {
  grid->cells[y * grid->w + x] = cell;
}

void grid_fill(Grid *grid, Cell cell) {
  for (int y = 0; y < grid->h; y++) {
    for (int x = 0; x < grid->w; x++) {
      grid_set(grid, x, y, cell);
    }
  }
}

Texture grid_render_texture(Grid *grid) {
  Image image = GenImageColor(grid->w, grid->h, BLANK);
  unsigned char *image_data = image.data;

  for (int y = 0; y < grid->h; y++) {
    for (int x = 0; x < grid->w; x++) {
      Cell cell = grid->cells[y * grid->w + x];

      int pixel_offset = (y * grid->w + x) * 4;
      image_data[pixel_offset + 0] = cell.glyph; // R: character
      image_data[pixel_offset + 1] = cell.fg;    // G: fg color
      image_data[pixel_offset + 2] = cell.bg;    // B: bg color
      image_data[pixel_offset + 3] = 255;        // A: alpha
    }
  }

  Texture texture = LoadTextureFromImage(image);
  UnloadImage(image);

  return texture;
}

void grid_free(Grid *grid) { free(grid); }
