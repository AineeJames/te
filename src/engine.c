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

#ifdef __linux__
#include <fcntl.h>
#include <sys/inotify.h>
#endif

void init_engine_lua_script(Engine *engine) {
  const char *main_path = TextFormat("%s/main.lua", engine->game_path);
  if (luaL_dofile(engine->L, main_path) != LUA_OK) {
    slog(FATAL, "Failed to load main.lua: %s", lua_tostring(engine->L, -1));
  }

  call_load(engine->L);
}

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

void add_lua_file_watch(Engine *engine, const char *path) {

#ifdef __linux__
  int inotify_err =
      inotify_add_watch(engine->watch_handle, path,
                        IN_ATTRIB | IN_MODIFY | IN_CREATE | IN_DELETE);
  assert(inotify_err != -1 && "failed to add watch");
#endif
}

void init_lua_file_watch(Engine *engine) {
#ifdef __linux__
  int inotifyFd = inotify_init(); /* Create inotify instance */
  assert(inotifyFd != -1 && "inotify init failed");
  int wd;
  // TODO go through lua script path and add all lua files
  int flags = fcntl(inotifyFd, F_GETFL, 0);
  fcntl(inotifyFd, F_SETFL, flags | O_NONBLOCK);
  engine->watch_handle = inotifyFd;
  add_lua_file_watch(engine, TextFormat("%s/main.lua", engine->game_path));
#endif
}

bool poll_lua_file_change(Engine *engine) {
  char buffer[BUF_LEN] = {0};
  assert(engine->watch_handle != 0);
  bool change = false;

#ifdef __linux__
  int length = read(engine->watch_handle, buffer, BUF_LEN);

  int i = 0;
  while (i < length) {
    struct inotify_event *event = (struct inotify_event *)&buffer[i];

    printf("File event\n");
    if (event->mask & IN_MODIFY) {
      printf("File was modified.\n");
      change = true;
    }
    if (event->mask & IN_CREATE) {
      printf("File was created.\n");
      change = true;
    }
    if (event->mask & IN_DELETE) {
      printf("File was deleted.\n");
      change = true;
    }
    if (event->mask & IN_ATTRIB) {
      printf("File was changed\n");
      change = true;
    }
    if (event->mask & IN_IGNORED) {
      printf("File was removed from inotify adding back\n");
      add_lua_file_watch(engine, TextFormat("%s/main.lua", engine->game_path));
      change = true;
    }

    i += EVENT_SIZE + event->len;
  }
#endif

  return change;
}

Engine *engine_init(const char *game_path) {
  Engine *engine = malloc(sizeof(Engine));
  assert(engine);

  engine->running = false;
  engine->exit_code = 0;
  engine->game_path = game_path;

  init_lua_file_watch(engine);

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

  init_engine_lua_script(engine);

  /*
  const char *main_path = TextFormat("%s/main.lua", engine->game_path);
  if (luaL_dofile(engine->L, main_path) != LUA_OK) {
    slog(FATAL, "Failed to load main.lua: %s", lua_tostring(engine->L, -1));
  }
  */

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

static const char *keycode_to_string(int key) {
  static char
      buf[8]; // temporary buffer for function keys or other generated strings

  // letters
  if (key >= KEY_A && key <= KEY_Z) {
    buf[0] = (char)('a' + (key - KEY_A));
    buf[1] = 0;
    return buf;
  }
  // numbers
  if (key >= KEY_ZERO && key <= KEY_NINE) {
    buf[0] = (char)('0' + (key - KEY_ZERO));
    buf[1] = 0;
    return buf;
  }
  // function keys
  if (key >= KEY_F1 && key <= KEY_F12) {
    snprintf(buf, sizeof(buf), "f%d", key - KEY_F1 + 1);
    return buf;
  }
  // arrows
  switch (key) {
  case KEY_RIGHT:
    return "right";
  case KEY_LEFT:
    return "left";
  case KEY_UP:
    return "up";
  case KEY_DOWN:
    return "down";
  case KEY_SPACE:
    return "space";
  case KEY_ENTER:
    return "enter";
  case KEY_TAB:
    return "tab";
  case KEY_BACKSPACE:
    return "backspace";
  case KEY_DELETE:
    return "delete";
  case KEY_LEFT_SHIFT:
  case KEY_RIGHT_SHIFT:
    return "shift";
  case KEY_LEFT_CONTROL:
  case KEY_RIGHT_CONTROL:
    return "ctrl";
  case KEY_LEFT_ALT:
  case KEY_RIGHT_ALT:
    return "alt";
  case KEY_LEFT_SUPER:
  case KEY_RIGHT_SUPER:
    return "super";
  case KEY_CAPS_LOCK:
    return "capslock";
  case KEY_NUM_LOCK:
    return "numlock";
  case KEY_SCROLL_LOCK:
    return "scrolllock";
  case KEY_PAUSE:
    return "pause";
  // punctuation / symbols
  case KEY_COMMA:
    return ",";
  case KEY_PERIOD:
    return ".";
  case KEY_SEMICOLON:
    return ";";
  case KEY_APOSTROPHE:
    return "'";
  case KEY_SLASH:
    return "/";
  case KEY_BACKSLASH:
    return "\\";
  case KEY_MINUS:
    return "-";
  case KEY_EQUAL:
    return "=";
  case KEY_LEFT_BRACKET:
    return "[";
  case KEY_RIGHT_BRACKET:
    return "]";
  case KEY_GRAVE:
    return "`";
  default:
    return NULL; // unhandled key
  }
}

void handle_all_keypresses(Engine *engine) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    engine->running = false;
  }

  int key;
  while ((key = GetKeyPressed()) != 0) {
    const char *keyStr = keycode_to_string(key);
    if (keyStr != NULL) {
      call_keypressed(engine->L, keyStr);
    }
  }
}

int engine_run(Engine *engine) {
  engine->running = true;

  while (engine->running) {
    float dt = GetFrameTime();

    if (poll_lua_file_change(engine)) {
      init_engine_lua_script(engine);
    }

    /* --- Input --- */
    if (IsKeyPressed(KEY_ESCAPE)) {
      engine->running = false;
    }
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
  lua_close(engine->L);
  renderer_free(engine->renderer);
  grid_free(engine->grid);
  CloseWindow();
  free(engine);
}
