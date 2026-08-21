// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaAccelByteSdk.h"

DEFINE_LOG_CATEGORY(LogXsollaAccelByteModule);

#define LOCTEXT_NAMESPACE "FXsollaAccelByteSdkModule"

void FXsollaAccelByteSdkModule::StartupModule()
{
    UE_LOG(LogXsollaAccelByteModule, Verbose, TEXT("XsollaAccelByteSdk module has started up."));
}

void FXsollaAccelByteSdkModule::ShutdownModule()
{
    UE_LOG(LogXsollaAccelByteModule, Verbose, TEXT("XsollaAccelByteSdk module has shut down."));
}

IMPLEMENT_MODULE(FXsollaAccelByteSdkModule, XsollaAccelByteSdk)

#undef LOCTEXT_NAMESPACE