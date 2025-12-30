#include "grid.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define GLYPH_W 16
#define GLYPH_H 16

typedef enum {
  VGA_BLACK = 0,
  VGA_BLUE,
  VGA_GREEN,
  VGA_CYAN,
  VGA_RED,
  VGA_MAGENTA,
  VGA_BROWN, // aka Yellow (dark)
  VGA_LIGHT_GRAY,

  VGA_DARK_GRAY,
  VGA_LIGHT_BLUE,
  VGA_LIGHT_GREEN,
  VGA_LIGHT_CYAN,
  VGA_LIGHT_RED,
  VGA_LIGHT_MAGENTA,
  VGA_YELLOW,
  VGA_WHITE,

  VGA_COLOR_COUNT // = 16
} VGA_Color;

typedef struct {
  Texture texture;
  size_t glyph_w;
  size_t glyph_h;
} GlyphAtlas;

typedef struct {
  Shader shader;
  int glyphAtlasTextureLoc;
  int gridTextureLoc;
  int cellSizeLoc;
  int gridSizeLoc;
} GridShader;

typedef struct {
  GlyphAtlas atlas;
  GridShader grid_shader;
  Texture dummy;
  Texture grid_texture;
} Renderer;

typedef struct Engine Engine;

Renderer *renderer_init(Engine *engine);
void renderer_free(Renderer *renderer);

struct Engine {
  bool running;
  int exit_code;
  const char *game_path;

  lua_State *L;
  Renderer *renderer;
  Grid *grid;
};

Engine *engine_init(const char *game_path);
int engine_run(Engine *engine);
void engine_free(Engine *engine);
void render_frame(Engine *engine);

static void usage(const char *prog_name) {
  printf("usage: %s path/to/game\n", prog_name);
}

static bool verify_game_path(const char *game_path) {
  if (!DirectoryExists(game_path)) {
    fprintf(stderr, "error: game path should be a directory\n");
    return false;
  }

  if (!FileExists(TextFormat("%s/main.lua", game_path))) {
    fprintf(stderr,
            "error: expected a main.lua to exist in the game directory\n");
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {
  const char *prog_name = argv[0];

  if (argc != 2) {
    fprintf(stderr, "error: an incorrect number of arguments was provided\n");
    usage(prog_name);
    return EXIT_FAILURE;
  }

  const char *game_path = argv[1];
  if (!verify_game_path(game_path)) {
    return EXIT_FAILURE;
  }

  Engine *engine = engine_init(game_path);

  int exit_code = engine_run(engine);
  engine_free(engine);

  return exit_code;
}

void render_frame(Engine *engine) {
  int dx = engine->renderer->atlas.glyph_w;
  int dy = engine->renderer->atlas.glyph_h;

  BeginDrawing();
  {
    ClearBackground(BLACK);
    BeginShaderMode(engine->renderer->grid_shader.shader);
    {
      SetShaderValueTexture(engine->renderer->grid_shader.shader,
                            engine->renderer->grid_shader.glyphAtlasTextureLoc,
                            engine->renderer->atlas.texture);
      SetShaderValueTexture(engine->renderer->grid_shader.shader,
                            engine->renderer->grid_shader.gridTextureLoc,
                            engine->renderer->grid_texture);

      DrawTexture(engine->renderer->dummy, 0, 0, WHITE);
    }
    EndShaderMode();

    DrawFPS(0, 0);
  }
  EndDrawing();
}

Renderer *renderer_init(Engine *engine) {
  Renderer *renderer = malloc(sizeof(Renderer));
  assert(renderer);

  renderer->atlas = (GlyphAtlas){
      .texture = LoadTexture("assets/images/Mx437_IBM_BIOS_16px.png"),
      .glyph_w = GLYPH_W,
      .glyph_h = GLYPH_H};

  renderer->grid_shader.shader = LoadShader(NULL, "assets/shaders/shader.glsl");

  // Cache shader locations
  renderer->grid_shader.glyphAtlasTextureLoc =
      GetShaderLocation(renderer->grid_shader.shader, "fontAtlasTexture");
  renderer->grid_shader.gridTextureLoc =
      GetShaderLocation(renderer->grid_shader.shader, "gridTexture");
  renderer->grid_shader.cellSizeLoc =
      GetShaderLocation(renderer->grid_shader.shader, "cellSize");
  renderer->grid_shader.gridSizeLoc =
      GetShaderLocation(renderer->grid_shader.shader, "gridSize");

  renderer->grid_texture = grid_render_texture(engine->grid);

  Image img = GenImageColor(GetScreenWidth(), GetScreenHeight(), WHITE);
  renderer->dummy = LoadTextureFromImage(img);
  UnloadImage(img);

  return renderer;
}

void renderer_free(Renderer *renderer) {
  UnloadTexture(renderer->atlas.texture);
  UnloadShader(renderer->grid_shader.shader);
  UnloadTexture(renderer->dummy);
  free(renderer);
}

Engine *engine_init(const char *game_path) {
  Engine *engine = malloc(sizeof(Engine));
  assert(engine);

  engine->running = false;
  engine->exit_code = 0;
  engine->game_path = game_path;

  engine->L = luaL_newstate();
  assert(engine->L);
  luaL_openlibs(engine->L);

  InitWindow(0, 0, "te");
  SetWindowMonitor(0);
  ToggleFullscreen();

  int sw = GetScreenWidth();
  int sh = GetScreenHeight();
  int w = sw / GLYPH_W;
  int h = sh / GLYPH_H;

  engine->grid = grid_init(w, h);
  Cell fill_cell = {
      .glyph = 0,
      .fg = VGA_BLACK,
      .bg = VGA_BLACK,
  };
  grid_fill(engine->grid, fill_cell);
  grid_print(engine->grid, 10, 10,
             "TODO:\n"
             "\t- Run lua code.\n"
             "\t- Refactor out.\n");

  engine->renderer = renderer_init(engine);

  int cell_size[2] = {engine->renderer->atlas.glyph_w,
                      engine->renderer->atlas.glyph_h};
  SetShaderValue(engine->renderer->grid_shader.shader,
                 engine->renderer->grid_shader.cellSizeLoc, cell_size,
                 SHADER_UNIFORM_IVEC2);

  int grid_size[2] = {engine->grid->w, engine->grid->h};
  SetShaderValue(engine->renderer->grid_shader.shader,
                 engine->renderer->grid_shader.gridSizeLoc, grid_size,
                 SHADER_UNIFORM_IVEC2);

  return engine;
}

int engine_run(Engine *engine) {
  engine->running = true;

  while (engine->running) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      engine->running = false;
    }
    render_frame(engine);
  }

  return 0; // TODO: handle exit codes
}

void engine_free(Engine *engine) {
  lua_close(engine->L);
  renderer_free(engine->renderer);
  grid_free(engine->grid);
  CloseWindow();
  free(engine);
}
