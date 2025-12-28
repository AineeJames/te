#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define WINDOW_W 1600
#define WINDOW_H 1200
#define GLYPH_WH 8
#define GRID_W (WINDOW_W / GLYPH_WH)
#define GRID_H (WINDOW_H / GLYPH_WH)

typedef struct {
  unsigned char glyph;
  unsigned char fg, bg;
} Cell;

typedef struct {
  size_t w, h;
  Cell cells[]; // flexible array member
} Grid;

static Grid *grid_init(int w, int h) {
  Grid *grid = malloc(sizeof(Grid) + (w * h * sizeof(Cell)));
  if (!grid)
    return NULL;

  grid->w = w;
  grid->h = h;

  return grid;
}

static void grid_set(Grid *grid, size_t x, size_t y, Cell cell) {
  grid->cells[y * grid->w + x] = cell;
}

static void grid_fill(Grid *grid, Cell cell) {
  for (int y = 0; y < grid->h; y++) {
    for (int x = 0; x < grid->w; x++) {
      grid_set(grid, x, y, cell);
    }
  }
}

static Texture grid_render_texture(Grid *grid) {
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

static void grid_free(Grid *grid) { free(grid); }

int main(int argc, char *argv[]) {

  Grid *grid = grid_init(GRID_W, GRID_H);
  grid_fill(grid, (Cell){'$', 15, 0});
  grid_set(grid, 10, 10, (Cell){'A', 4, 0});

  InitWindow(WINDOW_W, WINDOW_H, "CP437 Tilemap Test");

  Texture fontAtlas = LoadTexture("./MxPlus_IBM_BIOS_8px.png");
  Shader shader = LoadShader(NULL, "shader.glsl");

  int resolutionLoc = GetShaderLocation(shader, "resolution");
  int fontAtlasTextureLoc = GetShaderLocation(shader, "fontAtlasTexture");
  int gridTextureLoc = GetShaderLocation(shader, "gridTexture");
  int cellSizeLoc = GetShaderLocation(shader, "cellSize");
  int gridSizeLoc = GetShaderLocation(shader, "gridSize");

  int cell_size[2] = {GLYPH_WH, GLYPH_WH};
  SetShaderValue(shader, cellSizeLoc, cell_size, SHADER_UNIFORM_IVEC2);

  int grid_size[2] = {grid->w, grid->h};
  SetShaderValue(shader, gridSizeLoc, grid_size, SHADER_UNIFORM_IVEC2);

  Texture gridTexture = grid_render_texture(grid);

  // Create a fullscreen quad to render onto
  Texture dummy =
      LoadTextureFromImage(GenImageColor(WINDOW_W, WINDOW_H, WHITE));

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginShaderMode(shader);
    SetShaderValueTexture(shader, fontAtlasTextureLoc, fontAtlas);
    SetShaderValueTexture(shader, gridTextureLoc, gridTexture);
    DrawTexture(dummy, 0, 0, WHITE);
    EndShaderMode();

    DrawFPS(10, 10);
    EndDrawing();
  }

  UnloadTexture(fontAtlas);
  UnloadTexture(gridTexture);
  UnloadTexture(dummy);
  UnloadShader(shader);
  CloseWindow();

  grid_free(grid);

  return EXIT_SUCCESS;
}
