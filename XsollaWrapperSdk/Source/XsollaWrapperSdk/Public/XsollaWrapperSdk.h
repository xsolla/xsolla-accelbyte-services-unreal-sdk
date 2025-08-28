
#pragma once

#include "Modules/ModuleManager.h"

// Define our main module class
class FXsollaWrapperSdkModule : public IModuleInterface
{
public:

    // IModuleInterface implementation
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
    