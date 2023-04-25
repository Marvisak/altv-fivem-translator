#include "args.h"

alt::IPlayer* ArgReader::GetPlayer() {
    alt::IPlayer* player = nullptr;
    if (this->index > this->num_args)
        return player;
    if (lua_isnumber(this->state, this->index)) {
        auto id = (uint16_t)lua_tonumber(this->state, this->index);
        auto entity = alt::ICore::Instance().GetEntityByID(id);
        if (entity && entity->GetType() == alt::IBaseObject::Type::PLAYER)
            player = dynamic_cast<alt::IPlayer*>(entity);
    }
    this->index++;
    return player;
}

const char* ArgReader::GetString() {
    const char* str = nullptr;
    if (this->index > this->num_args)
        return str;
    if (lua_isstring(this->state, this->index))
        str = lua_tostring(this->state, this->index);
    this->index++;
    return str; 
}

int ArgReader::GetFunction() {
    int function = -1;
    if (this->index > this->num_args)
        return function;
    if (lua_isfunction(this->state, this->index)) {
        lua_pushvalue(this->state, this->index);
        function = luaL_ref(this->state, LUA_REGISTRYINDEX); 
    }
    this->index++;
    return function;
}

std::map<std::string, std::string> ArgReader::GetStringMap() {
    std::map<std::string, std::string> map;
    if (this->index > this->num_args)
        return map;
    if (lua_istable(this->state, this->index)) {
        lua_pushnil(this->state);
        while (lua_next(this->state, this->index) != 0) {
            map[luaL_checkstring(this->state, -2)] = luaL_checkstring(this->state, -1);
            lua_pop(this->state, 1);
        }
    }
    this->index++;
    return map;
}