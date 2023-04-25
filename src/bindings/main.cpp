#include "bindings.h"
#include "../runtime.h"
#include "../args.h"

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

int PerformHttpRequest(lua_State* state) {
	auto http_client = FivemTranslatorRuntime::Instance()->GetResource(state)->GetHttpClient();
	auto args = ArgReader(state);
	auto url = args.GetString();
	if (!url)
		return luaL_error(state, "Required argument 'url' is not string");
	auto callback = args.GetFunction();
	if (callback == -1)
		return luaL_error(state, "Required argument 'callback' is not function");
	auto method = args.GetString();
	if (!method)
		method = ix::HttpClient::kGet.c_str();

	auto request = http_client->createRequest(url, method);

	// User-Agent used by FiveM
	request->extraHeaders["User-Agent"] = "FXServer/PerformHttpRequest";
	// Without this some APIs don't want to accept the requests (I have no idea why)
	request->extraHeaders["Origin"] = "";

	auto data = args.GetString();
	if (data)
		request->body = data;

	auto headers = args.GetStringMap();
	if (!headers.empty())
		request->extraHeaders.merge(headers);

	http_client->performRequest(request, [state, callback](const ix::HttpResponsePtr& response) {
		lua_rawgeti(state, LUA_REGISTRYINDEX, callback);
		lua_pushinteger(state, response->statusCode);
		lua_pushstring(state, response->body.c_str());

		auto headers = response->headers;
		lua_createtable(state, 0, (int)headers.size());
		for (auto pair : headers) {
			// For some reason FiveM makes all of the fields lowercase, so for the sake of compatibility lets do that too
			auto key = pair.first;
			std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){ return std::tolower(c); });
			LUA_SET_TABLE_PAIR_STRING(state, key.c_str(), pair.second.c_str());
		}
		if (lua_pcall(state, 3, 0, 0) != LUA_OK)
			FivemTranslatorRuntime::Instance()->GetResource(state)->ShowError(state);	
	 	luaL_unref(state, LUA_REGISTRYINDEX, callback);
	});
	return 0;
}


void AddMainFunctions(lua_State* state) {
	LUA_ADD_FUNCTION(state, "print", print);
	LUA_ADD_FUNCTION(state, "PerformHttpRequest", PerformHttpRequest);
}