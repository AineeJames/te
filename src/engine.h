#ifndef ENGINE_H_
#define ENGINE_H_

#include "grid.h"
#include "lua.h"

typedef struct Renderer Renderer;

typedef struct {
  bool running;
  int exit_code;
  const char *game_path;

  lua_State *L;
  Renderer *renderer;
  Grid *grid;
  int watch_handle;
} Engine;

Engine *engine_init(const char *game_path);
int engine_run(Engine *engine);
void engine_free(Engine *engine);
void render_frame(Engine *engine);

#endif
