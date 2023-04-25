#include "runtime.h"

FivemTranslatorRuntime* FivemTranslatorRuntime::instance = nullptr;

alt::IResource::Impl* FivemTranslatorRuntime::CreateImpl(alt::IResource* resource) {
    auto resource_impl = new FivemTranslatorResource(resource);
    this->resources.push_back(resource_impl);
    return resource_impl;
}

void FivemTranslatorRuntime::DestroyImpl(alt::IResource::Impl* impl) {
    auto resource_impl = dynamic_cast<FivemTranslatorResource*>(impl);
    resources.erase(std::remove(resources.begin(), resources.end(), resource_impl), resources.end());
    delete resource_impl;
}

FivemTranslatorResource* FivemTranslatorRuntime::GetResource(lua_State* state) const {
    for (auto resource : resources)
        if (resource->GetState() == state)
            return resource;
    // This means that the state is probably a thread
    for (auto resource : resources)
        for (auto thread : resource->GetThreads())
            if (thread == state)
                return resource;
    return nullptr;
}