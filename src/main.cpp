#include "main.h"
#include "cpp-sdk/version/version.h"
#include "runtime.h"

EXPORT bool altMain(alt::ICore* core) {
    alt::ICore::SetInstance(core);
    
    auto runtime = new FivemTranslatorRuntime();
    core->RegisterScriptRuntime("fivem", runtime);

    core->LogInfo("FiveM to alt:V translation layer started");
    return true;
}

EXPORT const char* GetSDKHash() {
    return ALT_SDK_VERSION;
}