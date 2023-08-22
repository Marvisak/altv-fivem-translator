#pragma once

#include "main.h"

struct Event {
    int handler_index;
    std::map<int, int> handlers;
    bool safe_for_net;
};

struct Thread {
    int thread_ref;
    uint32_t timeout;
    int narg;
};

class FivemTranslatorResource : public alt::IResource::Impl {
    alt::IResource* resource;
    lua_State* state;
    std::vector<Thread> threads;
    ix::HttpClient* http_client;
    
    std::map<std::string, Event> events;
public:
    FivemTranslatorResource(alt::IResource* resource) : resource(resource) {
        http_client = new ix::HttpClient(true);
    }

    ~FivemTranslatorResource() {
        delete http_client;
    }

    bool Start() override;
    bool Stop() override;
    void OnEvent(const alt::CEvent* ev) override;
    void OnTick() override;

    bool LoadLuaFile(const std::string& file_name);
    void SetDefaultFunctions();

    void ShowError(lua_State* state) {
        std::string error = "Error occured";
        if (lua_isstring(state, -1))
            error = lua_tostring(state, -1);
        // The module sometimes crashes without this check, don't ask me why
        if (this)
            alt::ICore::Instance().LogError(this->resource->GetName() + ": " + error);
        else 
            alt::ICore::Instance().LogError(error);
    }

    void AddThread(int thread_ref, uint32_t timeout, int narg) {
        this->threads.push_back({thread_ref, timeout, narg});
    }
    int AddEvent(std::string event_name, int callback);
    void RegisterEvent(std::string event_name, bool safe_for_net);

    std::vector<lua_State*> GetThreads() {
        std::vector<lua_State*> thread_states;
        for (auto thread : threads) {
            lua_rawgeti(this->state, LUA_REGISTRYINDEX, thread.thread_ref);
            thread_states.push_back(lua_tothread(this->state, -1));
        }
         
        return thread_states;
    }

    lua_State* GetState() const {
        return this->state;
    }

    ix::HttpClient* GetHttpClient() {
        return this->http_client;
    }
};