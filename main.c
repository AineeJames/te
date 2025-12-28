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
  size_t fg, bg;
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

  // TODO: is this needed?
  /* grid->image = GenImageColor(w, h, BLANK); */
  /* grid->texture = LoadTextureFromImage(grid->image); */

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
      image_data[pixel_offset + 1] = cell.glyph; // G: fg color
      image_data[pixel_offset + 2] = cell.glyph; // B: bg color
      image_data[pixel_offset + 3] = 255;        // A: alpha
    }
  }

  Texture texture = LoadTextureFromImage(image);

  return texture;
}

int main(int argc, char *argv[]) {
  InitWindow(WINDOW_W, WINDOW_H, "CP437 Tilemap Test");
  Texture fontAtlas = LoadTexture("./MxPlus_IBM_BIOS_8px.png");
  Shader shader = LoadShader(NULL, "shader.glsl");

  int resolutionLoc = GetShaderLocation(shader, "resolution");
  int fontAtlasTextureLoc = GetShaderLocation(shader, "fontAtlasTexture");
  int gridTextureLoc = GetShaderLocation(shader, "gridTexture");
  int cellSizeLoc = GetShaderLocation(shader, "cellSize");
  int gridSizeLoc = GetShaderLocation(shader, "gridSize");

  int resolution[2] = {WINDOW_W, WINDOW_H};
  SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_IVEC2);

  int cell_size[2] = {GLYPH_WH, GLYPH_WH};
  SetShaderValue(shader, cellSizeLoc, cell_size, SHADER_UNIFORM_IVEC2);

  int grid_size[2] = {GRID_W, GRID_H};
  SetShaderValue(shader, gridSizeLoc, grid_size, SHADER_UNIFORM_IVEC2);

  // Create grid texture - must be GRID_W x GRID_H
  Image gridImage = GenImageColor(GRID_W, GRID_H, BLACK);
  unsigned char *gridData = (unsigned char *)gridImage.data;

  // Fill with repeating "Hello, World!" with colors
  const char *text = "Hello, World!";
  int textLen = strlen(text);

  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      int index = (y * GRID_W + x) % textLen;
      char ch = text[index];

      // Vary colors based on position for visual interest
      unsigned char fg = x % 16; // FG color (0-15 VGA palette)
      unsigned char bg = y % 16;

      int pixelOffset = (y * GRID_W + x) * 4;
      gridData[pixelOffset + 0] = (unsigned char)ch; // R: character
      gridData[pixelOffset + 1] = fg;                // G: foreground color
      gridData[pixelOffset + 2] = bg;                // B: background color
      gridData[pixelOffset + 3] = 255;               // A: alpha
    }
  }

  Texture gridTexture = LoadTextureFromImage(gridImage);
  UnloadImage(gridImage);

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

  return EXIT_SUCCESS;
}
