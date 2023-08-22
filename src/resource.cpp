#include "resource.h"
#include "bindings/bindings.h"
#include "events/events.h"

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
    auto type = ev->GetType();

    if (!event_handlers.contains(type))
        return;
    
    auto event_handler = event_handlers[type];
    auto event_name = event_handler.name;
    if (!events.contains(event_name))
        return;

    auto event = this->events[event_name];
    if (event.handlers.empty())
        return;
    
    if (!event.safe_for_net && type == 
    #ifdef ALT_SERVER_API
    alt::CEvent::Type::CLIENT_SCRIPT_EVENT
    #elif ALT_CLIENT_API
    alt::CEvent::Type::SERVER_SCRIPT_EVENT
    #endif
    ) 
        return;
    
    for (auto handler : event.handlers) {

        lua_State* thread = lua_newthread(this->state);
        int thread_ref = luaL_ref(this->state, LUA_REGISTRYINDEX);

		lua_rawgeti(this->state, LUA_REGISTRYINDEX, handler.second);
        lua_pushvalue(this->state, -1);

        lua_xmove(this->state, thread, 1);

        int narg = event_handler.args(ev, thread);

        this->AddThread(thread_ref, 0, narg);

    }
}

void FivemTranslatorResource::OnTick() {
    std::vector<int> dead_threads;
    for (auto& thread : this->threads) {
        if (thread.timeout > alt::ICore::Instance().GetNetTime())
            continue;

        lua_rawgeti(this->state, LUA_REGISTRYINDEX, thread.thread_ref);
        auto lua_thread = lua_tothread(this->state, -1);
        auto status = lua_status(lua_thread);

        bool remove = false;

        if (status == LUA_OK) {
            lua_Debug ar;
            if (lua_getstack(lua_thread, 0, &ar) <= 0 && lua_gettop(lua_thread) == 0)
                remove = true;
        } else if (status != LUA_YIELD) {
            remove = true;
        }

        if (!remove) {
            int nres;
            auto result = lua_resume(lua_thread, this->state, thread.narg, &nres);

            if (result == LUA_YIELD) {
                if (lua_isnumber(lua_thread, -1)) {
                    uint32_t timeout = (uint32_t)lua_tonumber(lua_thread, -1);
                    lua_pop(lua_thread, 1);
                    thread.timeout = alt::ICore::Instance().GetNetTime() + timeout;
                } 
            } else {
                if (result != LUA_OK)
                    ShowError(lua_thread);
                remove = true;
            }
        }

        // Remove the thread we pushed on to the stack
        lua_pop(this->state, 1);
        if (remove) {
            dead_threads.push_back(thread.thread_ref);
            luaL_unref(this->state, LUA_REGISTRYINDEX, thread.thread_ref);
        }
    }

    for (auto dead_thread : dead_threads)
        this->threads.erase(std::remove_if(this->threads.begin(), this->threads.end(), [&](Thread thread) {
            return thread.thread_ref == dead_thread;
        }));
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
        this->ShowError(this->state);
        return false;
    }

    if (lua_pcall(this->state, 0, LUA_MULTRET, 0) != LUA_OK) {
        this->ShowError(this->state);
        return false;
    }

    return true;
}

void FivemTranslatorResource::RegisterEvent(std::string event_name, bool safe_for_net) {
    if (this->events.contains(event_name)) {
        if (safe_for_net)
            this->events[event_name].safe_for_net = true;
        return;
    }

	for (auto event_handler : event_handlers)
		if (event_handler.second.name == event_name)
			if (!alt::ICore::Instance().IsEventEnabled(event_handler.first))
				alt::ICore::Instance().ToggleEvent(event_handler.first, true);
    
    this->events[event_name] = {0, {}, safe_for_net};
}

int FivemTranslatorResource::AddEvent(std::string event_name, int callback) {
    if (!this->events.contains(event_name))
        return 0;
    
    this->events[event_name].handlers[this->events[event_name].handler_index] = callback;
    return this->events[event_name].handler_index++;
}

void FivemTranslatorResource::SetDefaultFunctions() {
    LUA_DISABLE_FUNCTION(this->state, "dofile");
    LUA_DISABLE_FUNCTION(this->state, "loadfile");
    LUA_DISABLE_FUNCTION(this->state, "require");

    AddMainFunctions(this->state);
    AddCitizenFunctions(this->state);
    AddPlayerFunctions(this->state);
}