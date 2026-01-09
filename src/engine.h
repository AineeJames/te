#ifndef ENGINE_H_
#define ENGINE_H_

#include "grid.h"
#include "lua.h"

#define ENGINE_MAX_STREAMS 5

typedef struct Renderer Renderer;

typedef struct {
  bool running;
  int exit_code;
  const char *game_path;

  lua_State *L;
  Renderer *renderer;
  Grid *grid;
  int watch_handle;

  Music streams[ENGINE_MAX_STREAMS];
  int stream_count;
} Engine;

Engine *engine_init(const char *game_path);
int engine_run(Engine *engine);
void engine_free(Engine *engine);
void render_frame(Engine *engine);

#endif
