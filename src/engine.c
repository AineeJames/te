#include "engine.h"
#include "colors.h"
#include "globals.h"
#include "lauxlib.h"
#include "lualib.h"
#include "renderer.h"
#include <assert.h>
#include <raylib.h>
#include <stdlib.h>

Engine *engine_init(const char *game_path) {
  Engine *engine = malloc(sizeof(Engine));
  assert(engine);

  engine->running = false;
  engine->exit_code = 0;
  engine->game_path = game_path;

  engine->L = luaL_newstate();
  assert(engine->L);
  luaL_openlibs(engine->L);

  SetTraceLogLevel(LOG_NONE);
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

  return engine->exit_code;
}

void engine_free(Engine *engine) {
  lua_close(engine->L);
  renderer_free(engine->renderer);
  grid_free(engine->grid);
  CloseWindow();
  free(engine);
}
