// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaBackendSdk.h"
#include "XsollaBackendSdkGameSubsystem.h" // We need to include our subsystem header

DEFINE_LOG_CATEGORY(LogXsollaBackendModule);

#define LOCTEXT_NAMESPACE "FXsollaBackendSdkModule"

// This is where you would do any initialization when the module loads
void FXsollaBackendSdkModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing depends on the LoadingPhase specified in the .uplugin file.
    UE_LOG(LogXsollaBackendModule, Verbose, TEXT("XsollaBackendSdk module has started up!"));
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
