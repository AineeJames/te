#include "lua_api.h"
#include "colors.h"
#include "grid.h"
#include "input/keystring.h"
#include "lauxlib.h"
#include "lua.h"
#include "renderer.h"
#include "slog.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <string.h>

// te.graphics.setCell(cell, x, y)
static int l_setCell(lua_State *L) {
  int cell = luaL_checkinteger(L, 1) - 1;

  // Lua -> C index conversion
  lua_Number _x = luaL_checknumber(L, 2) - 1;
  lua_Number _y = luaL_checknumber(L, 3) - 1;
  int x = floor(_x);
  int y = floor(_y);

  lua_getglobal(L, "te");
  lua_getfield(L, -1, "__engine");
  Engine *engine = (Engine *)lua_touserdata(L, -1);
  lua_pop(L, 2); // pop te.__engine

  if (cell < 0 || cell >= 256 || x < 0 || x >= engine->grid->w || y < 0 ||
      y >= engine->grid->h)
    return 0;

  grid_set(engine->grid, (size_t)x, (size_t)y,
           (Cell){.glyph = cell,
                  .bg = engine->renderer->bg,
                  .fg = engine->renderer->fg});

  return 0;
}

// te.graphics.print(text, x, y)
static int l_print(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);

  // Lua -> C index conversion
  lua_Number _x = luaL_checknumber(L, 2) - 1;
  lua_Number _y = luaL_checknumber(L, 3) - 1;
  int x = floor(_x);
  int y = floor(_y);

  lua_getglobal(L, "te");
  lua_getfield(L, -1, "__engine");
  Engine *engine = (Engine *)lua_touserdata(L, -1);
  lua_pop(L, 2); // pop te.__engine

  for (int i = 0; i < strlen(text); i++) {
    if (x + i < 0 || x + i >= engine->grid->w || y < 0 || y >= engine->grid->h)
      return 0;

    grid_set(engine->grid, x + i, y,
             (Cell){
                 .glyph = text[i],
                 .fg = engine->renderer->fg,
                 .bg = engine->renderer->bg,
             });
  }

  return 0;
}

// te.graphics.clear()
static int l_clear(lua_State *L) {
  lua_getglobal(L, "te");
  lua_getfield(L, -1, "__engine");
  Engine *engine = (Engine *)lua_touserdata(L, -1);
  lua_pop(L, 2); // pop te.__engine

  grid_fill(engine->grid, CELL_EMPTY);

  return 0;
}

// te.graphics.setColor()
static int l_setColor(lua_State *L) {
  VGA_Color fg = luaL_checkinteger(L, 1);
  VGA_Color bg = luaL_checkinteger(L, 2);

  lua_getglobal(L, "te");
  lua_getfield(L, -1, "__engine");
  Engine *engine = (Engine *)lua_touserdata(L, -1);
  lua_pop(L, 2); // pop te.__engine

  engine->renderer->fg = fg;
  engine->renderer->bg = bg;

  return 0;
}

// w, h = te.window.getDimensions()
static int l_getDimensions(lua_State *L) {
  lua_getglobal(L, "te");
  lua_getfield(L, -1, "__engine");
  Engine *engine = (Engine *)lua_touserdata(L, -1);
  lua_pop(L, 2); // pop te.__engine

  int w = engine->grid->w;
  int h = engine->grid->h;

  // Push the results onto Lua stack
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);

  return 2; // number of return values
}

static int l_getFPS(lua_State *L) {
  int fps = GetFPS();

  lua_pushinteger(L, fps);

  return 1;
}

// te.keyboard.isDown(key)
static int l_isDown(lua_State *L) {
  const char *key_str = luaL_checkstring(L, 1);

  int keycode = string_to_keycode(key_str);
  bool pressed = false;

  if (keycode != KEY_NULL) {
    pressed = IsKeyDown(keycode);
  } else {
    slog(WARNING, "Unknown key: %s", key_str);
  }

  lua_pushboolean(L, pressed);

  return 1;
}

// te.event.quit(exit_code)
static int l_quit(lua_State *L) {
  int exit_code = luaL_checkinteger(L, 1);

  lua_getglobal(L, "te");
  lua_getfield(L, -1, "__engine");
  Engine *engine = (Engine *)lua_touserdata(L, -1);
  lua_pop(L, 2); // pop te.__engine

  engine->exit_code = exit_code;
  engine->running = false;

  return 0;
}

void register_lua_api(Engine *engine) {
  lua_State *L = engine->L;

  // te = {}
  lua_newtable(L);

  // te.__engine = Engine*
  lua_pushlightuserdata(L, engine);
  lua_setfield(L, -2, "__engine");

  // te.graphics = {}
  lua_newtable(L);
  // te.graphics.setCell()
  lua_pushcfunction(L, l_setCell);
  lua_setfield(L, -2, "setCell");
  // te.graphics.print()
  lua_pushcfunction(L, l_print);
  lua_setfield(L, -2, "print");
  // te.graphics.clear()
  lua_pushcfunction(L, l_clear);
  lua_setfield(L, -2, "clear");
  // te.graphics.setColor()
  lua_pushcfunction(L, l_setColor);
  lua_setfield(L, -2, "setColor");
  lua_setfield(L, -2, "graphics");

  // te.window = {}
  lua_newtable(L);
  // te.window.getDimensions()
  lua_pushcfunction(L, l_getDimensions);
  lua_setfield(L, -2, "getDimensions");
  // te.window.getFPS()
  lua_pushcfunction(L, l_getFPS);
  lua_setfield(L, -2, "getFPS");
  lua_setfield(L, -2, "window");

  // te.keyboard = {}
  lua_newtable(L);
  // te.window.getDimensions()
  lua_pushcfunction(L, l_isDown);
  lua_setfield(L, -2, "isDown");
  lua_setfield(L, -2, "keyboard");

  // te.event = {}
  lua_newtable(L);
  // te.event.quit(exit_code)
  lua_pushcfunction(L, l_quit);
  lua_setfield(L, -2, "quit");
  lua_setfield(L, -2, "event");

  lua_setglobal(L, "te");

  // Define globals
#define X(name)                                                                \
  lua_pushinteger(L, VGA_##name);                                              \
  lua_setglobal(L, #name);
  VGA_COLOR_LIST
#undef X
};

void call_load(lua_State *L) {
  lua_getglobal(L, "te");
  lua_getfield(L, -1, "load");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return;
  }

  // 0 args, 0 return values
  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
    slog(ERROR, "failed calling te.load: %s", lua_tostring(L, -1));
    lua_pop(L, 1);
  }

  lua_pop(L, 1); // pop te table
}

void call_update(lua_State *L, double dt) {
  lua_getglobal(L, "te");
  lua_getfield(L, -1, "update");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return;
  }

  // Push arguments
  lua_pushnumber(L, dt);

  // Call with 1 arg, 0 return values
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
    slog(ERROR, "failed calling te.update: %s", lua_tostring(L, -1));
    lua_pop(L, 1);
  }

  lua_pop(L, 1); // pop te table
}

void call_draw(lua_State *L) {
  lua_getglobal(L, "te");
  lua_getfield(L, -1, "draw");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return;
  }

  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
    slog(ERROR, "failed calling te.draw: %s", lua_tostring(L, -1));
    lua_pop(L, 1);
  }

  lua_pop(L, 1); // pop te table
}

void call_keypressed(lua_State *L, const char *key) {
  lua_getglobal(L, "te");
  lua_getfield(L, -1, "keypressed");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return;
  }

  // Push arguments
  lua_pushstring(L, key);

  // Call with 1 arg, 0 return values
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
    slog(ERROR, "failed calling te.keypressed: %s", lua_tostring(L, -1));
    lua_pop(L, 1);
  }

  lua_pop(L, 1); // pop te table
}
