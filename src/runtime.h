#include "main.h"

class FivemTranslatorRuntime : public alt::IScriptRuntime {
public:
    alt::IResource::Impl* CreateImpl(alt::IResource* resource) override;
    void DestroyImpl(alt::IResource::Impl* impl) override;
    bool RequiresMain() const override {
        return false;
    }
};
