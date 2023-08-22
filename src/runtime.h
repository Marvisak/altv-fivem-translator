#pragma once

#include "main.h"
#include "resource.h"
#include "events/events.h"

class FivemTranslatorRuntime : public alt::IScriptRuntime {
    static FivemTranslatorRuntime* instance;
    std::vector<FivemTranslatorResource*> resources;
public:
    FivemTranslatorRuntime() {
        RegisterEvents();
        this->instance = this;
    }

    alt::IResource::Impl* CreateImpl(alt::IResource* resource) override;
    void DestroyImpl(alt::IResource::Impl* impl) override;

    FivemTranslatorResource* GetResource(lua_State* state) const;

    bool RequiresMain() const override {
        return false;
    }

    static FivemTranslatorRuntime* Instance() {
        return instance;
    }
};
