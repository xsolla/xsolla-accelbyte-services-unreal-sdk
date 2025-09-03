// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

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
    