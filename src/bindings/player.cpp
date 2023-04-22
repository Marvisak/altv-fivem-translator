#include "bindings.h"

int GetPlayers(lua_State* state) {
    auto players = alt::ICore::Instance().GetPlayers();
    lua_createtable(state, (int)players.size(), 0);
    for (int i = 0; i < players.size(); i++) {
        lua_pushinteger(state, players[i]->GetID());
        lua_rawseti(state, -2, i+1);
    }
    return 1;
}

void AddPlayerFunctions(lua_State* state) {
    LUA_ADD_FUNCTION(state, "GetPlayers", GetPlayers);
}