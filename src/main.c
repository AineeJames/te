#include "grid.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define GLYPH_WH 16

int main(int argc, char *argv[]) {
  InitWindow(0, 0, "te");
  SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
  SetWindowMonitor(0);
  ToggleFullscreen();

  int screen_w = GetScreenWidth();
  int screen_h = GetScreenHeight();
  int grid_w = screen_w / GLYPH_WH;
  int grid_h = screen_h / GLYPH_WH;

  Grid *grid = grid_init(grid_w, grid_h);
  grid_fill(grid, (Cell){0, 15, 0});
  grid_set(grid, 10, 10, (Cell){1, 4, 0});

  Texture fontAtlas = LoadTexture("assets/images/Mx437_IBM_BIOS_16px.png");

  Shader shader = LoadShader(NULL, "assets/shaders/shader.glsl");

  int fontAtlasTextureLoc = GetShaderLocation(shader, "fontAtlasTexture");
  int gridTextureLoc = GetShaderLocation(shader, "gridTexture");
  int cellSizeLoc = GetShaderLocation(shader, "cellSize");
  int gridSizeLoc = GetShaderLocation(shader, "gridSize");

  int cell_size[2] = {GLYPH_WH, GLYPH_WH};
  SetShaderValue(shader, cellSizeLoc, cell_size, SHADER_UNIFORM_IVEC2);

  int grid_size[2] = {grid->w, grid->h};
  SetShaderValue(shader, gridSizeLoc, grid_size, SHADER_UNIFORM_IVEC2);

  Texture gridTexture = grid_render_texture(grid);

  Texture dummy =
      LoadTextureFromImage(GenImageColor(screen_w, screen_h, BLANK));

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
