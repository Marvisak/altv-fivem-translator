#include "runtime.h"
#include "resource.h"

alt::IResource::Impl* FivemTranslatorRuntime::CreateImpl(alt::IResource* resource) {
    return new FivemTranslatorResource(resource);
}

void FivemTranslatorRuntime::DestroyImpl(alt::IResource::Impl* impl) {
    delete reinterpret_cast<FivemTranslatorResource*>(impl);
}