#pragma once

#define LUA_DISABLE_FUNCTION(state, function_name) { \
	lua_pushnil(state); \
	lua_setglobal(state, function_name); \
} 

#define LUA_ADD_FUNCTION(state, function_name, function) { \
	lua_pushcfunction(state, function); \
	lua_setglobal(state, function_name); \
} 

#define LUA_SET_TABLE_ELEMENT_INT(state, index, value) { \
    lua_pushinteger(state, value); \
    lua_rawseti(state, -2, index + 1); \
}

#define LUA_SET_TABLE_ELEMENT_STRING(state, index, value) { \
    lua_pushstring(state, value); \
    lua_rawseti(state, -2, index + 1); \
}

#define LUA_SET_TABLE_PAIR_STRING(state, key, value) { \
    lua_pushstring(state, key); \
    lua_pushstring(state, value); \
    lua_settable(state, -3); \
}

#define LUA_SET_TABLE_PAIR_STRING_INT(state, key, value) { \
    lua_pushstring(state, key); \
    lua_pushinteger(state, value); \
    lua_settable(state, -3); \
}
