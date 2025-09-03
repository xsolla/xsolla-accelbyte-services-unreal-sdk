// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "XsollaMapping.h"

#include "XsollaWrapperSdkGameSubsystem.generated.h"

#define GET_OSS_INTERFACE(ReturnType, Function)\
ReturnType Function() \
{ \
    FOnlineSubsystemXsolla* Subsystem = GetXsollaOnlineSubsystem();\
    if (Subsystem) \
    { \
        return Subsystem->Function(); \
    } \
    return nullptr; \
}

UCLASS()
class XSOLLAWRAPPERSDK_API UXsollaWrapperSdkGameSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    static UXsollaWrapperSdkGameSubsystem* Get(const UObject* WorldContextObject);

    // USubsystem implementation - required overrides
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    FOnlineSubsystemXsolla* GetXsollaOnlineSubsystem();

    FXsollaSdkInstancePtr GetXsollaSdkInstance() const;

    FApiClientPtr GetGameSdkApiClient();

    FServerApiClientPtr GetGameSdkServerApiClient();

    GET_OSS_INTERFACE(IOnlineAchievementsPtr, GetAchievementsInterface)
    GET_OSS_INTERFACE(FOnlineAgreementXsollaPtr, GetAgreementInterface)
    GET_OSS_INTERFACE(FOnlineAnalyticsXsollaPtr, GetAnalyticsInterface)
    GET_OSS_INTERFACE(FOnlineAuthXsollaPtr, GetAuthInterface)
    GET_OSS_INTERFACE(IOnlineChatPtr, GetChatInterface)
    GET_OSS_INTERFACE(FOnlineCloudSaveXsollaPtr, GetCloudSaveInterface)
    GET_OSS_INTERFACE(IOnlineEntitlementsPtr, GetEntitlementsInterface)
    GET_OSS_INTERFACE(IOnlineExternalUIPtr, GetExternalUIInterface)
    GET_OSS_INTERFACE(IOnlineFriendsPtr, GetFriendsInterface)
    GET_OSS_INTERFACE(FOnlineGameStandardEventXsollaPtr, GetGameStandardEventInterface)
    GET_OSS_INTERFACE(IOnlineGroupsPtr, GetGroupsInterface)
    GET_OSS_INTERFACE(IOnlineIdentityPtr, GetIdentityInterface)
    GET_OSS_INTERFACE(IOnlineLeaderboardsPtr, GetLeaderboardsInterface)
    GET_OSS_INTERFACE(FOnlinePredefinedEventXsollaPtr, GetPredefinedEventInterface)
    GET_OSS_INTERFACE(IOnlinePresencePtr, GetPresenceInterface)
    GET_OSS_INTERFACE(IOnlinePurchasePtr, GetPurchaseInterface)
    GET_OSS_INTERFACE(IOnlineSessionPtr, GetSessionInterface)
    GET_OSS_INTERFACE(IOnlineStatsPtr, GetStatsInterface)
    GET_OSS_INTERFACE(IOnlineStoreV2Ptr, GetStoreV2Interface)
    GET_OSS_INTERFACE(IOnlineTimePtr, GetTimeInterface)
    GET_OSS_INTERFACE(IOnlineUserPtr, GetUserInterface)
    GET_OSS_INTERFACE(FOnlineUserCacheXsollaPtr, GetUserCache)
    GET_OSS_INTERFACE(IOnlineUserCloudPtr, GetUserCloudInterface)
    GET_OSS_INTERFACE(IOnlineVoicePtr, GetVoiceInterface)
    GET_OSS_INTERFACE(IVoiceChatPtr, GetVoiceChatInterface)
    GET_OSS_INTERFACE(FOnlineWalletXsollaPtr, GetWalletInterface)
protected:

    FXsollaSdkInstancePtr XsollaSdkInstance;
};
