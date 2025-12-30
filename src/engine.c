#include "engine.h"
#include "globals.h"
#include "grid.h"
#include "lauxlib.h"
#include "lua.h"
#include "lua_api.h"
#include "lualib.h"
#include "renderer.h"
#include "slog.h"
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
  register_lua_api(engine);

  SetTraceLogLevel(LOG_NONE);
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
  InitWindow(0, 0, "te");
  SetWindowMonitor(0);
  ToggleFullscreen();
  SetTargetFPS(60);

  int sw = GetScreenWidth();
  int sh = GetScreenHeight();
  int w = sw / GLYPH_W;
  int h = sh / GLYPH_H;

  engine->grid = grid_init(w, h);
  grid_fill(engine->grid, CELL_EMPTY);

  const char *main_path = TextFormat("%s/main.lua", engine->game_path);
  if (luaL_dofile(engine->L, main_path) != LUA_OK) {
    slog(FATAL, "Failed to load main.lua: %s", lua_tostring(engine->L, -1));
  }

  call_load(engine->L);

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
    float dt = GetFrameTime();

    /* --- Input --- */
    if (IsKeyPressed(KEY_ESCAPE)) {
      engine->running = false;
    }

    /* --- Update --- */
    call_update(engine->L, dt);

    /* --- Draw --- */
    call_draw(engine->L);

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
