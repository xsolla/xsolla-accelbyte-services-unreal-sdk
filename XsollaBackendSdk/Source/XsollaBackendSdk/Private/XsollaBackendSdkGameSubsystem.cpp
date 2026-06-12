// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaBackendSdkGameSubsystem.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"

#include "XsollaSettings.h"
#include "Core/AccelByteSettings.h"
#include "Core/AccelByteServerSettings.h"

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
        // ---------------------------------------------------------------
        // Non-OSS path: build AccelByte::Settings / ServerSettings from
        // UXsollaSettings and pass them to the settings-accepting overload
        // so the direct-SDK path also honors the Xsolla config.
        //
        // The PostConfigInit module (XsollaBackendSdkConfig) already
        // forwarded these values into the AccelByte ini sections, so the
        // no-arg overload would pick them up too. However, constructing
        // the structs explicitly here makes the non-OSS path independent
        // of ini-level forwarding and guarantees the values match the
        // Xsolla CDO at the moment of instance creation.
        // ---------------------------------------------------------------
        const UXsollaSettings* XsollaConfig = GetDefault<UXsollaSettings>();

        // Start from the module's existing global settings so that any
        // value NOT overridden by UXsollaSettings retains its default.
        AccelByte::Settings ClientSettings = IAccelByteUe4SdkModuleInterface::Get().GetClientSettings();
        AccelByte::ServerSettings SrvSettings = IAccelByteUe4SdkModuleInterface::Get().GetServerSettings();

        // --- Client overrides (only if non-empty) ---
        if (!XsollaConfig->ClientId.IsEmpty())            { ClientSettings.ClientId = XsollaConfig->ClientId; }
        if (!XsollaConfig->Namespace.IsEmpty())           { ClientSettings.Namespace = XsollaConfig->Namespace; }
        if (!XsollaConfig->PublisherNamespace.IsEmpty())   { ClientSettings.PublisherNamespace = XsollaConfig->PublisherNamespace; }
        if (!XsollaConfig->RedirectURI.IsEmpty())          { ClientSettings.RedirectURI = XsollaConfig->RedirectURI; }
        if (!XsollaConfig->BaseUrl.IsEmpty())              { ClientSettings.BaseUrl = XsollaConfig->BaseUrl; }

        // --- Server overrides (only if non-empty) ---
        if (!XsollaConfig->ServerClientId.IsEmpty())           { SrvSettings.ClientId = XsollaConfig->ServerClientId; }
        if (!XsollaConfig->ServerClientSecret.IsEmpty())       { SrvSettings.ClientSecret = XsollaConfig->ServerClientSecret; }
        if (!XsollaConfig->ServerNamespace.IsEmpty())           { SrvSettings.Namespace = XsollaConfig->ServerNamespace; }
        if (!XsollaConfig->ServerPublisherNamespace.IsEmpty())  { SrvSettings.PublisherNamespace = XsollaConfig->ServerPublisherNamespace; }
        if (!XsollaConfig->ServerBaseUrl.IsEmpty())             { SrvSettings.BaseUrl = XsollaConfig->ServerBaseUrl; }

        XsollaSdkInstance = IAccelByteUe4SdkModuleInterface::Get().CreateAccelByteInstance(
            ClientSettings, SrvSettings);
    }
    InterfaceManager = NewObject<UXsollaInterfaceManager>(this);
    InterfaceManager->Initialize();

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
