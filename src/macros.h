#pragma once

#define LUA_DISABLE_FUNCTION(state, function_name) { \
	lua_pushnil(state); \
	lua_setglobal(state, function_name); \
} 

#define LUA_ADD_FUNCTION(state, function_name, function) { \
	lua_pushcfunction(state, function); \
	lua_setglobal(state, function_name); \
} 