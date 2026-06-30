// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaBackendSdk.h"
#include "XsollaBackendSdkGameSubsystem.h" // We need to include our subsystem header

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

DEFINE_LOG_CATEGORY(LogXsollaBackendModule);

#define LOCTEXT_NAMESPACE "FXsollaBackendSdkModule"

// This is where you would do any initialization when the module loads
void FXsollaBackendSdkModule::StartupModule()
{
    UE_LOG(LogXsollaBackendModule, Verbose, TEXT("XsollaBackendSdk module has started up!"));

#if WITH_EDITOR
    // Hide AccelByte settings here so developers configure everything through Xsolla Backend SDK Settings only.
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("AccelByte Unreal Engine 4 Client SDK"));
        SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("AccelByte Unreal Engine 4 Server SDK"));
    }
#endif
}

// This is where you would do any cleanup when the module unloads
void FXsollaBackendSdkModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.
    UE_LOG(LogXsollaBackendModule, Verbose, TEXT("XsollaBackendSdk module has been shut down!"));
}

// This macro registers our module with the Unreal Engine
IMPLEMENT_MODULE(FXsollaBackendSdkModule, XsollaBackendSdk)

#undef LOCTEXT_NAMESPACE
