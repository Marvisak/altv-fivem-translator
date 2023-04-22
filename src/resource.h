#include "main.h"

class FivemTranslatorResource : public alt::IResource::Impl {
    alt::IResource* resource;
    lua_State* state;
public:
    FivemTranslatorResource(alt::IResource* resource) : resource(resource) {}
    bool Start() override;
    bool Stop() override;
    void OnEvent(const alt::CEvent* ev) override;

    bool LoadLuaFile(const std::string& file_name);
    void SetDefaultFunctions();
};