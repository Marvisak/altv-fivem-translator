#include "main.h"

class FivemTranslatorResource : public alt::IResource::Impl {
    alt::IResource* resource;
    lua_State* state;
    std::map<int, uint32_t> threads;
    ix::HttpClient* http_client;
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

    void AddThread(int thread, int timeout) {
        this->threads[thread] = timeout;
    }

    std::vector<lua_State*> GetThreads() {
        std::vector<lua_State*> thread_states;
        for (auto thread : threads) {
            lua_rawgeti(this->state, LUA_REGISTRYINDEX, thread.first);
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