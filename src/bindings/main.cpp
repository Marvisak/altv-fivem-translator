#include "bindings.h"

// https://github.com/citizenfx/fivem/blob/master/code/components/citizen-scripting-lua/src/LuaScriptRuntime.cpp#L195
int print(lua_State* state) {
    const int n = lua_gettop(state);
	lua_getglobal(state, "tostring");

    std::stringstream stream;
	for (int i = 1; i <= n; i++)
	{
		lua_pushvalue(state, -1);
		lua_pushvalue(state, i); 
		lua_call(state, 1, 1);

		size_t l = 0;
		const char* s = lua_tolstring(state, -1, &l);
		if (s == NULL)
			return luaL_error(state, "'tostring' must return a string to 'print'");

		if (i > 1)
			stream << "\t";
		stream << s;
		lua_pop(state, 1);
	}
	alt::ICore::Instance().LogColored(stream.str());
    return 0;
}


void AddMainFunctions(lua_State* state) {
	LUA_ADD_FUNCTION(state, "print", print);
}