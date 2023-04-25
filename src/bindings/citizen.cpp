#include "bindings.h"
#include "../runtime.h"

int Wait(lua_State* state) {
    lua_pushvalue(state, 1);
    lua_yield(state, 1);
    return 0;
}

int CreateThread(lua_State* state) {
    lua_State* thread = lua_newthread(state);
    int thread_ref = luaL_ref(state, LUA_REGISTRYINDEX);

    lua_pushvalue(state, 1);
    lua_xmove(state, thread, 1);
    FivemTranslatorRuntime::Instance()->GetResource(state)->AddThread(thread_ref, 0);

    return 0;
}

int SetTimeout(lua_State* state) {
    lua_State* thread = lua_newthread(state);
    int thread_ref = luaL_ref(state, LUA_REGISTRYINDEX);

    lua_pushvalue(state, 2);
    lua_xmove(state, thread, 1);

	uint32_t timeout = (uint32_t)lua_tointeger(state, 1);
    FivemTranslatorRuntime::Instance()->GetResource(state)->AddThread(thread_ref, timeout + alt::ICore::Instance().GetNetTime());

    return 0;
}

int Trace(lua_State* state) {
	alt::ICore::Instance().LogColored(luaL_checkstring(state, 1));
	return 0;
}

int InvokeNative(lua_State* state) {
	#ifdef ALT_SERVER_API
	return luaL_error(state, "Citizen.InvokeNative isn't supported on the server");
	#endif
}

// FiveM has many more Citizen functions, but only these (apart from InvokeNative and CreateThreadNow) are mentioned in the documentation
// The rest are functions which are probably used somewhere, but I have never seen them used in actual code
static const struct luaL_Reg citizenLib[] = {
	{ "Trace", Trace },
    // They call the exact same thing because there is no reason for them to be seperate, FiveM has such a complex (and useless) system for this
	// that they need 2 functions to figure out if the function should launch now or 5 CPU ticks later
	{ "CreateThread", CreateThread },
	{ "CreateThreadNow", CreateThread },
	{ "Wait", Wait },
	{ "SetTimeout", SetTimeout },
	{ "InvokeNative", InvokeNative },
	{ nullptr, nullptr }
};

void AddCitizenFunctions(lua_State* state) {
    lua_newtable(state);
    luaL_setfuncs(state, citizenLib, 0);
    lua_setglobal(state, "Citizen");

    LUA_ADD_FUNCTION(state, "Wait", Wait);
    LUA_ADD_FUNCTION(state, "CreateThread", CreateThread);
    LUA_ADD_FUNCTION(state, "SetTimeout", SetTimeout);
}