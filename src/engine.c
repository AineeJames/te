#include "engine.h"
#include "globals.h"
#include "grid.h"
#include "input/keystring.h"
#include "lauxlib.h"
#include "lua.h"
#include "lua_api.h"
#include "lualib.h"
#include "renderer.h"
#include "slog.h"
#include <assert.h>
#include <raylib.h>
#include <stdlib.h>

static void slog_engine_handler(Slog_Record *record) {
  Engine *engine = record->ctx;

  printf("[te::");
  switch (record->level) {
  case SLOG_DEBUG:
    printf("debug");
    break;
  case SLOG_INFO:
    printf("info");
    break;
  case SLOG_WARNING:
    printf("warn");
    break;
  case SLOG_ERROR:
    printf("error");
    break;
  case SLOG_FATAL:
    printf("fatal");
    engine->exit_code = 1;
    engine->running = false;
    break;
  }

  printf("] ");
  vprintf(record->fmt, record->args);
  printf("\n");
}

Engine *engine_init(const char *game_path) {
  Engine *engine = malloc(sizeof(Engine));
  assert(engine);
  slog_set_handler(slog_engine_handler, .ctx = engine);

  engine->running = true;
  engine->exit_code = 0;
  engine->game_path = game_path;

  engine->L = luaL_newstate();
  assert(engine->L);
  luaL_openlibs(engine->L);
  register_lua_api(engine);

  SetTraceLogLevel(LOG_NONE);
  InitWindow(0, 0, "te");
  SetWindowMonitor(0);
  ToggleFullscreen();

  int sw = GetScreenWidth();
  int sh = GetScreenHeight();
  int w = sw / GLYPH_W;
  int h = sh / GLYPH_H;

  engine->grid = grid_init(w, h);
  grid_fill(engine->grid, CELL_EMPTY);

  const char *main_path = TextFormat("%s/main.lua", engine->game_path);
  if (luaL_dofile(engine->L, main_path) != LUA_OK) {
    fatal("Failed to load main.lua: %s", lua_tostring(engine->L, -1));
    return engine;
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

  info("Initialized te successfully!");
  return engine;
}

void handle_all_keypresses(Engine *engine) {
  int key;

  while ((key = GetKeyPressed()) != 0) {
    const char *keyStr = keycode_to_string(key);
    if (keyStr != NULL) {
      call_keypressed(engine->L, keyStr);
    }
  }
}

int engine_run(Engine *engine) {
  while (engine->running) {
    float dt = GetFrameTime();

    /* --- Input --- */
    handle_all_keypresses(engine);

    /* --- Update --- */
    call_update(engine->L, dt);

    /* --- Draw --- */
    call_draw(engine->L);

    render_frame(engine);
  }

  return engine->exit_code;
}

void engine_free(Engine *engine) {
  if (engine->L)
    lua_close(engine->L);
  if (engine->renderer)
    renderer_free(engine->renderer);
  if (engine->grid)
    grid_free(engine->grid);
  CloseWindow();
  free(engine);
}
