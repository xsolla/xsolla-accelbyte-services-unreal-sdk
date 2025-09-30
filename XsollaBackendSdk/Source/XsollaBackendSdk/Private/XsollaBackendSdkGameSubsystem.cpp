// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaBackendSdkGameSubsystem.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"

DEFINE_LOG_CATEGORY(LogXsollaBackendSubsystem);

// Define the static Get function
UXsollaBackendSdkGameSubsystem* UXsollaBackendSdkGameSubsystem::Get(const UObject* WorldContextObject)
{
    if (WorldContextObject)
    {
        // Get the UGameInstance from the world context object
        if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
        {
            // Use the GameInstance to get the subsystem instance
            return GameInstance->GetSubsystem<UXsollaBackendSdkGameSubsystem>();
        }
    }
    return nullptr;
}

// This function is called when the subsystem is created
void UXsollaBackendSdkGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FOnlineSubsystemAccelByte* Subsystem = GetXsollaOnlineSubsystem();
    if (Subsystem)
    {
        XsollaSdkInstance = Subsystem->GetAccelByteInstance().Pin();
    }
    else
    {
        XsollaSdkInstance = IAccelByteUe4SdkModuleInterface::Get().CreateAccelByteInstance();
    }
    InterfaceManager = NewObject<UXsollaInterfaceManager>(this);
    InterfaceManager->Initialize(this);

    UE_LOG(LogXsollaBackendSubsystem, Verbose, TEXT("XsollaBackendSdkGameSubsystem has been initialized!"));
}

// This function is called when the subsystem is destroyed
void UXsollaBackendSdkGameSubsystem::Deinitialize()
{
    UE_LOG(LogXsollaBackendSubsystem, Verbose, TEXT("XsollaBackendSdkGameSubsystem has been deinitialized!"));
    Super::Deinitialize();
}

FOnlineSubsystemXsolla* UXsollaBackendSdkGameSubsystem::GetXsollaOnlineSubsystem()
{
    return static_cast<FOnlineSubsystemXsolla*>(Online::GetSubsystem(GetWorld(), ACCELBYTE_SUBSYSTEM));
}

FXsollaSdkInstancePtr UXsollaBackendSdkGameSubsystem::GetXsollaSdkInstance() const
{
    if (!XsollaSdkInstance.IsValid())
    {
        UE_LOG(LogXsollaBackendSubsystem, Warning, TEXT("XsollaSdkInstance is null or invalid."));
        return nullptr;
    }

    return XsollaSdkInstance;
}

UXsollaInterfaceManager* UXsollaBackendSdkGameSubsystem::GetInterfaceManager() const
{
    return InterfaceManager;
}

FApiClientPtr UXsollaBackendSdkGameSubsystem::GetGameSdkApiClient()
{
    FApiClientPtr ApiClient;
    FOnlineSubsystemXsolla* Subsytem = GetXsollaOnlineSubsystem();
    if (Subsytem)
    {
        ApiClient = Subsytem->GetApiClient(Subsytem->GetLocalUserNumCached());
    }
    else
    {
        ApiClient = XsollaSdkInstance->GetApiClient();
    }

    return ApiClient;
}

FServerApiClientPtr UXsollaBackendSdkGameSubsystem::GetGameSdkServerApiClient()
{
    FServerApiClientPtr ServerApiClient;

    ServerApiClient = XsollaSdkInstance->GetServerApiClient();

    return ServerApiClient;
}
