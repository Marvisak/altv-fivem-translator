#include "bindings.h"
#include "../args.h"

#ifdef ALT_SERVER_API
int GetPlayers(lua_State* state) {
    auto players = alt::ICore::Instance().GetBaseObjects(alt::IBaseObject::Type::PLAYER);
    lua_createtable(state, (int)players.size(), 0);
    for (int i = 0; i < players.size(); i++) {
        LUA_SET_TABLE_ELEMENT_INT(state, i, players[i]->GetID());
    }
    return 1;
}

int GetPlayerIdentifiers(lua_State* state) {
    auto args = ArgReader(state);
    auto player = args.GetPlayer();
    lua_createtable(state, 2, 0);
    if (!player) 
        return 1;

    auto ip = player->GetIP();
    auto ipv6_start_index = ip.find("::ffff:");
    if (ipv6_start_index != std::string::npos)
        ip.erase(ipv6_start_index, 7);
    LUA_SET_TABLE_ELEMENT_STRING(state, 0, ("ip:" + ip).c_str());
    // The FiveM's license is something much different, but it's definitelly better than nothing
    LUA_SET_TABLE_ELEMENT_STRING(state, 1, ("license:" + std::to_string(player->GetSocialID())).c_str());
    if (player->GetDiscordId() != 0)
        LUA_SET_TABLE_ELEMENT_STRING(state, 2, ("discord:" + std::to_string(player->GetDiscordId())).c_str());
    return 1;
}
#endif

void AddPlayerFunctions(lua_State* state) {
    #ifdef ALT_SERVER_API
    LUA_ADD_FUNCTION(state, "GetPlayers", GetPlayers);
    LUA_ADD_FUNCTION(state, "GetPlayerIdentifiers", GetPlayerIdentifiers);
    #endif
}