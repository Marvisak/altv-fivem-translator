#include "resource.h"
#include "bindings/bindings.h"

// https://github.com/citizenfx/fivem/blob/master/code/components/citizen-scripting-lua/src/LuaScriptRuntime.cpp#L164
static const luaL_Reg lualibs[] = {
	{ "_G", luaopen_base },
	{ LUA_TABLIBNAME, luaopen_table },
	{ LUA_STRLIBNAME, luaopen_string },
	{ LUA_MATHLIBNAME, luaopen_math },
	{ LUA_DBLIBNAME, luaopen_debug },
	{ LUA_COLIBNAME, luaopen_coroutine },
	{ LUA_UTF8LIBNAME, luaopen_utf8 },
#ifdef ALT_SERVER_API
	{ LUA_IOLIBNAME, luaopen_io },
	{ LUA_OSLIBNAME, luaopen_os },
#endif
	{ "msgpack", luaopen_cmsgpack },
	{ "json", luaopen_rapidjson },
	{ NULL, NULL }
};    

bool FivemTranslatorResource::Start() {
    // Check if fxmanifest.lua is in the folder, just to warn the user so that they don't come crying that the module isn't working
    auto fxmanifest_path = std::filesystem::path(this->resource->GetPath()) / "fxmanifest.lua";
    if (std::filesystem::exists(fxmanifest_path))
        alt::ICore::Instance().LogWarning("fxmanifest.lua is unsupported");

    this->state = luaL_newstate();
    if (!this->state) {
        alt::ICore::Instance().LogError("Unknown error occured");
        return false;
    }

	auto lib = lualibs;
	for (; lib->func; lib++) {
		luaL_requiref(this->state, lib->name, lib->func, 1);
		lua_pop(this->state, 1);
	}    

    this->SetDefaultFunctions();

#ifdef ALT_SERVER_API
    auto scripts = this->resource->GetConfig()["server_scripts"];
#elif ALT_CLIENT_API
    auto scripts = this->resource->GetConfig()["client_scripts"];
#endif
    auto shared = this->resource->GetConfig()["shared_scripts"];
    auto package = this->resource->GetPackage();

    if (scripts)
        for (auto script : scripts->AsList())
            if (!this->LoadLuaFile(script->AsString()))
                return false;

    if (shared)
        for (auto script : shared->AsList())
            if (!this->LoadLuaFile(script->AsString()))
                return false;

    // TODO Add importing resource via @resource/file

    return true;
}

bool FivemTranslatorResource::Stop() {
    lua_close(this->state);
    return true;
}

void FivemTranslatorResource::OnEvent(const alt::CEvent* ev) {
}

bool FivemTranslatorResource::LoadLuaFile(const std::string& file_name) {
    auto package = this->resource->GetPackage(); 

    if (!package->FileExists(file_name)) {
        alt::ICore::Instance().LogError("File " + file_name + " doesn't exist");
        return false;
    }
    
    auto file = package->OpenFile(file_name);
    size_t file_length = package->GetFileSize(file);
    std::string file_content(file_length, 0);
    package->ReadFile(file, file_content.data(), file_content.size());
    package->CloseFile(file);

    if (luaL_loadbuffer(this->state, file_content.c_str(), file_length, file_name.c_str()) != LUA_OK) {
        alt::ICore::Instance().LogError("Failed to parse " + file_name);
        return false;
    }

    if (lua_pcall(this->state, 0, LUA_MULTRET, 0) != LUA_OK) {
        if (lua_isstring(this->state, -1))
            alt::ICore::Instance().LogError(lua_tostring(this->state, -1)); 
        return false;
    }

    return true;
}

void FivemTranslatorResource::SetDefaultFunctions() {
    LUA_DISABLE_FUNCTION(this->state, "dofile");
    LUA_DISABLE_FUNCTION(this->state, "loadfile");
    LUA_DISABLE_FUNCTION(this->state, "require");

    AddMainFunctions(this->state);
    AddCitizenFunctions(this->state);
    AddPlayerFunctions(this->state);
}