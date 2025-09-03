// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaWrapperSdk.h"
#include "XsollaWrapperSdkGameSubsystem.h" // We need to include our subsystem header

#define LOCTEXT_NAMESPACE "FXsollaWrapperSdkModule"

// This is where you would do any initialization when the module loads
void FXsollaWrapperSdkModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing depends on the LoadingPhase specified in the .uplugin file.
    UE_LOG(LogTemp, Warning, TEXT("XsollaWrapperSdk module has started up!"));
}

// This is where you would do any cleanup when the module unloads
void FXsollaWrapperSdkModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.
    UE_LOG(LogTemp, Warning, TEXT("XsollaWrapperSdk module has shut down!"));
}

// This macro registers our module with the Unreal Engine
IMPLEMENT_MODULE(FXsollaWrapperSdkModule, XsollaWrapperSdk)

#undef LOCTEXT_NAMESPACE
