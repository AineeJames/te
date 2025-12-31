#ifndef LUA_API_H_
#define LUA_API_H_

#include "engine.h"
#include "lua.h"

void register_lua_api(Engine *engine);
void call_load(lua_State *L);
void call_update(lua_State *L, double dt);
void call_draw(lua_State *L);
void call_keypressed(lua_State *L, const char *key);

#endif // LUA_API_H_
