// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "Api/XsollaAccelByteAuthApi.h"

#include "Api/AccelByteUserApi.h"
#include "Api/XsollaAccelByteAuthCallbackProxy.h"
#include "Auth/XsollaAccelByteAuthUtils.h"
#include "Core/AccelByteError.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Models/AccelByteOauth2Models.h"
#include "OnlineSubsystem.h"
#include "XsollaLoginSubsystem.h"
#include "XsollaLoginTypes.h"

namespace
{
    const TCHAR* AccelByteApiClientMissingCode = TEXT("accelbyte-apiclient-missing");
    const TCHAR* AccelByteApiClientMissingDescription = TEXT("AccelByte ApiClient is not available.");
    const TCHAR* AccelByteUserApiMissingCode = TEXT("accelbyte-user-api-missing");
    const TCHAR* AccelByteUserApiMissingDescription = TEXT("AccelByte User API is not available.");
}

namespace AccelByte
{
namespace Api
{

XsollaAccelByteAuth::XsollaAccelByteAuth(Credentials const& InCredentialsRef
    , Settings const& InSettingsRef
    , FHttpRetrySchedulerBase& InHttpRef
    , TSharedPtr<FApiClient, ESPMode::ThreadSafe> const& InApiClient
    , UWorld* InWorld)
    : FApiBase(InCredentialsRef, InSettingsRef, InHttpRef, InApiClient)
    , ApiClientWeak(InApiClient)
    , World(InWorld)
{
}

XsollaAccelByteAuth::~XsollaAccelByteAuth()
{
    if (SilentLoginCompleteDelegateHandle.IsValid())
    {
        ClearSilentLoginDelegate(0);
    }

    for (UXsollaAccelByteAuthCallbackProxy* Proxy : ActiveCallbackProxies)
    {
        if (Proxy)
        {
            Proxy->RemoveFromRoot();
        }
    }
    ActiveCallbackProxies.Empty();
}

void XsollaAccelByteAuth::LoginWithXsollaAccount(const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess, const FOnXsollaAccelByteLoginCancelled& OnLoginCancelled, const FOnXsollaAccelByteLoginFailed& OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginCancelled = OnLoginCancelled;
    OnXsollaLoginFailed = OnLoginFailed;

    UWorld* WorldPtr = World.Get();
    UGameInstance* GameInstance = WorldPtr ? WorldPtr->GetGameInstance() : nullptr;
    UXsollaLoginSubsystem* LoginSubsystem = GameInstance ? GameInstance->GetSubsystem<UXsollaLoginSubsystem>() : nullptr;

    if (!LoginSubsystem)
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingCode(), XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingDescription());
        return;
    }

    UXsollaAccelByteAuthCallbackProxy* CallbackProxy = CreateCallbackProxy();

    FOnAuthUpdate OnAuthUpdate;
    OnAuthUpdate.BindDynamic(CallbackProxy, &UXsollaAccelByteAuthCallbackProxy::HandleAuthUpdate);

    FOnAuthCancel OnAuthCancel;
    OnAuthCancel.BindDynamic(CallbackProxy, &UXsollaAccelByteAuthCallbackProxy::HandleAuthCancel);

    FOnAuthError OnAuthError;
    OnAuthError.BindDynamic(CallbackProxy, &UXsollaAccelByteAuthCallbackProxy::HandleAuthError);

    TWeakPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthWeak = AsShared();
    TWeakObjectPtr<UXsollaAccelByteAuthCallbackProxy> CallbackProxyWeak(CallbackProxy);
    CallbackProxy->OnAuthUpdate = [XsollaAuthWeak, CallbackProxyWeak](const FXsollaLoginData& LoginData)
    {
        if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
        {
            XsollaAuthPtr->ReleaseCallbackProxy(CallbackProxyWeak.Get());
            XsollaAuthPtr->HandleXsollaLoginSuccess(LoginData);
        }
    };
    CallbackProxy->OnAuthCancel = [XsollaAuthWeak, CallbackProxyWeak]()
    {
        if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
        {
            XsollaAuthPtr->ReleaseCallbackProxy(CallbackProxyWeak.Get());
            XsollaAuthPtr->HandleXsollaLoginCancelled();
        }
    };
    CallbackProxy->OnAuthError = [XsollaAuthWeak, CallbackProxyWeak](const FString& Code, const FString& Description)
    {
        if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
        {
            XsollaAuthPtr->ReleaseCallbackProxy(CallbackProxyWeak.Get());
            XsollaAuthPtr->HandleXsollaLoginFailed(Code, Description);
        }
    };

    LoginSubsystem->AuthWithXsollaWidget(WorldPtr, CallbackProxy->LoginWidget, OnAuthUpdate, OnAuthCancel, OnAuthError);
}

void XsollaAccelByteAuth::LoginWithXsollaSilentAuth(const FName& SubsystemName, const FOnlineAccountCredentials& Credentials, const FString& InAppId, const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess, const FOnXsollaAccelByteLoginFailed& OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    SilentAuthSubsystemName = SubsystemName;
    AppId = InAppId;

    if (SubsystemName.IsEqual(XsollaAccelByteAuthUtils::SteamSubsystemName()) && InAppId.IsEmpty())
    {
        GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"), AppId, GEngineIni);
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginFailed = OnLoginFailed;

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName);
    if (!Subsystem)
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::SubsystemAuthFailedCode(SilentAuthSubsystemName), XsollaAccelByteAuthUtils::SubsystemInitializationFailedDescription(SilentAuthSubsystemName));
        return;
    }

    IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
    if (!IdentityInterface.IsValid())
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::SubsystemAuthFailedCode(SilentAuthSubsystemName), XsollaAccelByteAuthUtils::SubsystemIdentityMissingDescription(SilentAuthSubsystemName));
        return;
    }

    TWeakPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthWeak = AsShared();
    SilentLoginCompleteDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0,
        FOnLoginCompleteDelegate::CreateLambda([XsollaAuthWeak](int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
        {
            if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
            {
                XsollaAuthPtr->OnSilentAuthLoginComplete(LocalUserNum, bLoginWasSuccessful, UserId, Error);
            }
        }));

    if (!IdentityInterface->Login(0, Credentials))
    {
        ClearSilentLoginDelegate(0);
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::SubsystemAuthFailedCode(SilentAuthSubsystemName), TEXT("Silent auth login request was not started."));
    }
}

void XsollaAccelByteAuth::LoginWithXsollaSessionTicket(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& InAppId, const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess, const FOnXsollaAccelByteLoginFailed& OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginFailed = OnLoginFailed;
    LoginWithXsollaSessionTicketInternal(Provider, SessionTicket, Code, InAppId);
}

void XsollaAccelByteAuth::LoginWithXsollaAccessToken(const FString& AccessToken, const FOnXsollaAccelByteLoginSuccess& OnLoginSuccess, const FOnXsollaAccelByteLoginFailed& OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginFailed = OnLoginFailed;
    LoginToAccelByteWithToken(AccessToken);
}

bool XsollaAccelByteAuth::TryBeginLogin(const FOnXsollaAccelByteLoginFailed& OnLoginFailed)
{
    if (bLoginInProgress)
    {
        OnLoginFailed.ExecuteIfBound(XsollaAccelByteAuthUtils::LoginInProgressCode(), XsollaAccelByteAuthUtils::LoginInProgressDescription());
        return false;
    }

    bLoginInProgress = true;
    return true;
}

UXsollaAccelByteAuthCallbackProxy* XsollaAccelByteAuth::CreateCallbackProxy()
{
    UXsollaAccelByteAuthCallbackProxy* CallbackProxy = NewObject<UXsollaAccelByteAuthCallbackProxy>();
    CallbackProxy->AddToRoot();
    ActiveCallbackProxies.Add(CallbackProxy);
    return CallbackProxy;
}

void XsollaAccelByteAuth::ReleaseCallbackProxy(UXsollaAccelByteAuthCallbackProxy* Proxy)
{
    if (!Proxy)
    {
        return;
    }

    if (ActiveCallbackProxies.RemoveSingleSwap(Proxy) > 0)
    {
        Proxy->OnAuthUpdate = TFunction<void(const FXsollaLoginData&)>();
        Proxy->OnAuthError = TFunction<void(const FString&, const FString&)>();
        Proxy->OnAuthCancel = TFunction<void()>();
        Proxy->RemoveFromRoot();
    }
}

void XsollaAccelByteAuth::LoginWithXsollaSessionTicketInternal(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& InAppId)
{
    UWorld* WorldPtr = World.Get();
    UGameInstance* GameInstance = WorldPtr ? WorldPtr->GetGameInstance() : nullptr;
    UXsollaLoginSubsystem* LoginSubsystem = GameInstance ? GameInstance->GetSubsystem<UXsollaLoginSubsystem>() : nullptr;

    if (!LoginSubsystem)
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingCode(), XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingDescription());
        return;
    }

    UXsollaAccelByteAuthCallbackProxy* CallbackProxy = CreateCallbackProxy();

    FOnAuthUpdate OnAuthUpdate;
    OnAuthUpdate.BindDynamic(CallbackProxy, &UXsollaAccelByteAuthCallbackProxy::HandleAuthUpdate);

    FOnAuthError OnAuthError;
    OnAuthError.BindDynamic(CallbackProxy, &UXsollaAccelByteAuthCallbackProxy::HandleAuthError);

    TWeakPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthWeak = AsShared();
    TWeakObjectPtr<UXsollaAccelByteAuthCallbackProxy> CallbackProxyWeak(CallbackProxy);
    CallbackProxy->OnAuthUpdate = [XsollaAuthWeak, CallbackProxyWeak](const FXsollaLoginData& LoginData)
    {
        if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
        {
            XsollaAuthPtr->ReleaseCallbackProxy(CallbackProxyWeak.Get());
            XsollaAuthPtr->HandleXsollaLoginSuccess(LoginData);
        }
    };
    CallbackProxy->OnAuthError = [XsollaAuthWeak, CallbackProxyWeak](const FString& Code, const FString& Description)
    {
        if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
        {
            XsollaAuthPtr->ReleaseCallbackProxy(CallbackProxyWeak.Get());
            XsollaAuthPtr->HandleXsollaLoginFailed(Code, Description);
        }
    };

    LoginSubsystem->AuthenticateWithSessionTicket(Provider, SessionTicket, Code, InAppId, XsollaAccelByteAuthUtils::XsollaState(), OnAuthUpdate, OnAuthError);
}

void XsollaAccelByteAuth::HandleXsollaLoginSuccess(const FXsollaLoginData& LoginData)
{
    LoginToAccelByte(LoginData);
}

void XsollaAccelByteAuth::HandleXsollaLoginFailed(const FString& Code, const FString& Description)
{
    const FOnXsollaAccelByteLoginFailed LoginFailed = OnXsollaLoginFailed;
    ClearLoginDelegates();
    LoginFailed.ExecuteIfBound(Code, Description);
}

void XsollaAccelByteAuth::HandleXsollaLoginCancelled()
{
    const FOnXsollaAccelByteLoginCancelled LoginCancelled = OnXsollaLoginCancelled;
    ClearLoginDelegates();
    LoginCancelled.ExecuteIfBound();
}

void XsollaAccelByteAuth::LoginToAccelByte(const FXsollaLoginData& LoginData)
{
    LoginToAccelByteWithToken(LoginData.AuthToken.JWT, LoginData);
}

void XsollaAccelByteAuth::LoginToAccelByteWithToken(const FString& AccessToken, TOptional<FXsollaLoginData> LoginData)
{
    if (AccessToken.IsEmpty())
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::XsollaTokenEmptyCode(), XsollaAccelByteAuthUtils::XsollaTokenEmptyDescription());
        return;
    }

    const TSharedPtr<FApiClient, ESPMode::ThreadSafe> PinnedApiClient = ApiClientWeak.Pin();
    if (!PinnedApiClient.IsValid())
    {
        HandleXsollaLoginFailed(AccelByteApiClientMissingCode, AccelByteApiClientMissingDescription);
        return;
    }

    const UserPtr UserApi = PinnedApiClient->GetUserApi().Pin();
    if (!UserApi.IsValid())
    {
        HandleXsollaLoginFailed(AccelByteUserApiMissingCode, AccelByteUserApiMissingDescription);
        return;
    }

    const FString PlatformId = XsollaAccelByteAuthUtils::GetConfiguredXsollaPlatformId();
    const bool bCreateHeadlessAccount = XsollaAccelByteAuthUtils::ShouldCreateHeadlessAccount();

    TWeakPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthWeak = AsShared();
    const TOptional<FXsollaLoginData> LoginDataCopy = LoginData;
    const FString AccessTokenCopy = AccessToken;

    UserApi->LoginWithOtherPlatformIdV4(PlatformId, AccessToken,
        THandler<FAccelByteModelsLoginQueueTicketInfo>::CreateLambda([XsollaAuthWeak, LoginDataCopy, AccessTokenCopy](const FAccelByteModelsLoginQueueTicketInfo&)
        {
            const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin();
            if (!XsollaAuthPtr.IsValid())
            {
                return;
            }

            const FXsollaAccelByteLoginResult LoginResult = XsollaAccelByteAuthUtils::MakeLoginResult(LoginDataCopy, AccessTokenCopy);

            const FOnXsollaAccelByteLoginSuccess LoginSuccess = XsollaAuthPtr->OnXsollaLoginSuccess;
            XsollaAuthPtr->ClearLoginDelegates();
            LoginSuccess.ExecuteIfBound(LoginResult);
        }),
        FOAuthErrorHandler::CreateLambda([XsollaAuthWeak](int32 ErrorCode, const FString& ErrorMessage, const FErrorOAuthInfo&)
        {
            if (const TSharedPtr<XsollaAccelByteAuth, ESPMode::ThreadSafe> XsollaAuthPtr = XsollaAuthWeak.Pin())
            {
                XsollaAuthPtr->HandleXsollaLoginFailed(FString::Printf(TEXT("accelbyte-oidc-login-failed-%d"), ErrorCode), ErrorMessage);
            }
        }),
        bCreateHeadlessAccount);
}

void XsollaAccelByteAuth::ClearLoginDelegates()
{
    XsollaAccelByteAuthUtils::ClearLoginDelegates(OnXsollaLoginSuccess, OnXsollaLoginFailed, OnXsollaLoginCancelled);
    bLoginInProgress = false;
}

void XsollaAccelByteAuth::ClearSilentLoginDelegate(int32 LocalUserNum)
{
    if (!SilentLoginCompleteDelegateHandle.IsValid())
    {
        return;
    }

    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName))
    {
        if (IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface())
        {
            IdentityInterface->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, SilentLoginCompleteDelegateHandle);
        }
    }

    SilentLoginCompleteDelegateHandle.Reset();
}

void XsollaAccelByteAuth::OnSilentAuthLoginComplete(int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    ClearSilentLoginDelegate(LocalUserNum);

    if (!bLoginWasSuccessful)
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::SilentAuthFailedCode(), Error);
        return;
    }

    const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName);
    if (!Subsystem)
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::SilentAuthFailedCode(), Error);
        return;
    }

    const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
    if (!IdentityInterface.IsValid())
    {
        HandleXsollaLoginFailed(XsollaAccelByteAuthUtils::SilentAuthFailedCode(), XsollaAccelByteAuthUtils::SilentAuthIdentityMissingDescription());
        return;
    }

    const FString SessionTicket = IdentityInterface->GetAuthToken(LocalUserNum);
    LoginWithXsollaSessionTicketInternal(SilentAuthSubsystemName.ToString().ToLower(), SessionTicket, TEXT(""), AppId);
}

}
}
