// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Core/AccelByteApiBase.h"
#include "Core/AccelByteApiClient.h"
#include "Auth/XsollaAccelByteAuthTypes.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/Optional.h"
#include "XsollaLoginTypes.h"

class UXsollaAccelByteAuthCallbackProxy;

namespace AccelByte
{
namespace Api
{

class XSOLLAACCELBYTESDK_API XsollaAccelByteAuth final
    : public FApiBase
    , public TSharedFromThis<XsollaAccelByteAuth, ESPMode::ThreadSafe>
{
public:
    XsollaAccelByteAuth(Credentials const& InCredentialsRef
        , Settings const& InSettingsRef
        , FHttpRetrySchedulerBase& InHttpRef
        , TSharedPtr<FApiClient, ESPMode::ThreadSafe> const& InApiClient
        , UWorld* InWorld);

    virtual ~XsollaAccelByteAuth();

    void LoginWithXsollaAccount(const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess = FOnXsollaAccelByteLoginSuccess()
        , const FOnXsollaAccelByteLoginCancelled& OnLoginCancelled = FOnXsollaAccelByteLoginCancelled()
        , const FOnXsollaAccelByteLoginFailed& OnLoginFailed = FOnXsollaAccelByteLoginFailed());

    void LoginWithXsollaSilentAuth(const FName& SubsystemName
        , const FOnlineAccountCredentials& Credentials
        , const FString& InAppId = TEXT("")
        , const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess = FOnXsollaAccelByteLoginSuccess()
        , const FOnXsollaAccelByteLoginFailed& OnLoginFailed = FOnXsollaAccelByteLoginFailed());

    void LoginWithXsollaSessionTicket(const FString& Provider
        , const FString& SessionTicket
        , const FString& Code
        , const FString& InAppId
        , const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess = FOnXsollaAccelByteLoginSuccess()
        , const FOnXsollaAccelByteLoginFailed& OnLoginFailed = FOnXsollaAccelByteLoginFailed());

    void LoginWithXsollaAccessToken(const FString& AccessToken
        , const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess = FOnXsollaAccelByteLoginSuccess()
        , const FOnXsollaAccelByteLoginFailed& OnLoginFailed = FOnXsollaAccelByteLoginFailed());

private:
    bool TryBeginLogin(const FOnXsollaAccelByteLoginFailed& OnLoginFailed);
    UXsollaAccelByteAuthCallbackProxy* CreateCallbackProxy();
    void ReleaseCallbackProxy(UXsollaAccelByteAuthCallbackProxy* Proxy);
    void LoginWithXsollaSessionTicketInternal(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& InAppId);
    void HandleXsollaLoginSuccess(const FXsollaLoginData& LoginData);
    void HandleXsollaLoginFailed(const FString& Code, const FString& Description);
    void HandleXsollaLoginCancelled();
    void LoginToAccelByte(const FXsollaLoginData& LoginData);
    void LoginToAccelByteWithToken(const FString& AccessToken, TOptional<FXsollaLoginData> LoginData = TOptional<FXsollaLoginData>());
    void ClearLoginDelegates();
    void ClearSilentLoginDelegate(int32 LocalUserNum);
    void OnSilentAuthLoginComplete(int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

    TWeakPtr<FApiClient, ESPMode::ThreadSafe> ApiClientWeak;
    TWeakObjectPtr<UWorld> World;
    TArray<UXsollaAccelByteAuthCallbackProxy*> ActiveCallbackProxies;
    bool bLoginInProgress = false;

    FString AppId;
    FName SilentAuthSubsystemName;
    FDelegateHandle SilentLoginCompleteDelegateHandle;

    FOnXsollaAccelByteLoginSuccess OnXsollaLoginSuccess;
    FOnXsollaAccelByteLoginFailed OnXsollaLoginFailed;
    FOnXsollaAccelByteLoginCancelled OnXsollaLoginCancelled;
};

using XsollaAccelByteAuthPtr = TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe>;
using XsollaAccelByteAuthWPtr = TWeakPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe>;

}
}
