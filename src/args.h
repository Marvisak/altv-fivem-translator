#include "main.h"

class ArgReader {
    lua_State* state;
    int index = 1;
    int num_args;
public:
    ArgReader(lua_State* state) : state(state) {
        this->num_args = lua_gettop(state);
    }
    alt::IPlayer* GetPlayer();
    const char* GetString();
    int GetFunction();

    std::map<std::string, std::string> GetStringMap();
};