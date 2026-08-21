// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "Auth/XsollaAccelByteAuth.h"

#include "Auth/XsollaAccelByteAuthUtils.h"
#include "Core/AccelByteUtilities.h"
#include "Models/AccelByteOauth2Models.h"
#include "OnlineSubsystemAccelByteTypes.h"
#include "InterfaceModels/OnlineIdentityInterfaceAccelByteModels.h"
#include "OnlineSubsystem.h"

DEFINE_LOG_CATEGORY(LogXsollaAccelByteAuth);

namespace
{
    const TCHAR* AccelByteSubsystemMissingCode = TEXT("accelbyte-subsystem-missing");
    const TCHAR* AccelByteSubsystemMissingDescription = TEXT("OnlineSubsystemAccelByte is not available.");
    const TCHAR* AccelByteIdentityMissingCode = TEXT("accelbyte-identity-missing");
    const TCHAR* AccelByteIdentityMissingDescription = TEXT("OnlineSubsystemAccelByte identity interface is not available.");
    const TCHAR* AccelByteOidcLoginFailedCode = TEXT("accelbyte-oidc-login-failed");
    const TCHAR* AccelByteOidcLoginNotStartedCode = TEXT("accelbyte-oidc-login-not-started");
    const TCHAR* AccelByteOidcLoginNotStartedDescription = TEXT("OnlineSubsystemAccelByte rejected the Xsolla OIDC login request.");
}

void UXsollaAccelByteAuth::Initialize()
{
    OnAuthUpdate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UXsollaAccelByteAuth, HandleLoginSuccess));
    OnAuthError.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UXsollaAccelByteAuth, HandleLoginFailed));
    OnAuthCancel.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UXsollaAccelByteAuth, HandleLoginCancelled));

    const UGameInstance* GameInstance = GetTypedOuter<UGameInstance>();
    if (GameInstance)
    {
        LoginSubsystem = GameInstance->GetSubsystem<UXsollaLoginSubsystem>();
    }
}

void UXsollaAccelByteAuth::LoginWithXsollaAccount(const FOnXsollaAccelByteLoginSuccess OnLoginSuccess, const FOnXsollaAccelByteLoginCancelled OnLoginCancelled, const FOnXsollaAccelByteLoginFailed OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginCancelled = OnLoginCancelled;
    OnXsollaLoginFailed = OnLoginFailed;

    if (LoginSubsystem)
    {
        LoginSubsystem->AuthWithXsollaWidget(GetWorld(), LoginWidget, OnAuthUpdate, OnAuthCancel, OnAuthError);
    }
    else
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingCode(), XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingDescription());
    }
}

void UXsollaAccelByteAuth::LoginWithXsollaSilentAuth(const FName& SubsystemName, const FOnlineAccountCredentials& Credentials, const FString& InAppId, const FOnXsollaAccelByteLoginSuccess OnLoginSuccess, const FOnXsollaAccelByteLoginFailed OnLoginFailed)
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

    if (AppId.IsEmpty())
    {
        UE_LOG(LogXsollaAccelByteAuth, Warning, TEXT("AppId is empty. Login using subsystem may fail."));
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginFailed = OnLoginFailed;

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName);
    if (!Subsystem)
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::SubsystemAuthFailedCode(SilentAuthSubsystemName), XsollaAccelByteAuthUtils::SubsystemInitializationFailedDescription(SilentAuthSubsystemName));
        return;
    }

    IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
    if (!IdentityInterface.IsValid())
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::SubsystemAuthFailedCode(SilentAuthSubsystemName), XsollaAccelByteAuthUtils::SubsystemIdentityMissingDescription(SilentAuthSubsystemName));
        return;
    }

    SilentLoginCompleteDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &UXsollaAccelByteAuth::OnSilentAuthLoginComplete));
    if (!IdentityInterface->Login(0, Credentials))
    {
        ClearSilentLoginDelegate(0);
        HandleLoginFailed(XsollaAccelByteAuthUtils::SubsystemAuthFailedCode(SilentAuthSubsystemName), TEXT("Silent auth login request was not started."));
    }
}

void UXsollaAccelByteAuth::LoginWithXsollaSessionTicket(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& InAppId, const FOnXsollaAccelByteLoginSuccess OnLoginSuccess, const FOnXsollaAccelByteLoginFailed OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginFailed = OnLoginFailed;
    LoginWithXsollaSessionTicketInternal(Provider, SessionTicket, Code, InAppId);
}

void UXsollaAccelByteAuth::LoginWithXsollaAccessToken(const FString& AccessToken, const FOnXsollaAccelByteLoginSuccess OnLoginSuccess, const FOnXsollaAccelByteLoginFailed OnLoginFailed)
{
    if (!TryBeginLogin(OnLoginFailed))
    {
        return;
    }

    OnXsollaLoginSuccess = OnLoginSuccess;
    OnXsollaLoginFailed = OnLoginFailed;
    LoginToAccelByteWithToken(AccessToken);
}

bool UXsollaAccelByteAuth::TryBeginLogin(const FOnXsollaAccelByteLoginFailed& OnLoginFailed)
{
    if (bLoginInProgress)
    {
        OnLoginFailed.ExecuteIfBound(XsollaAccelByteAuthUtils::LoginInProgressCode(), XsollaAccelByteAuthUtils::LoginInProgressDescription());
        return false;
    }

    bLoginInProgress = true;
    return true;
}

void UXsollaAccelByteAuth::LoginWithXsollaSessionTicketInternal(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& InAppId)
{
    if (!LoginSubsystem)
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingCode(), XsollaAccelByteAuthUtils::XsollaLoginSubsystemMissingDescription());
        return;
    }

    LoginSubsystem->AuthenticateWithSessionTicket(Provider, SessionTicket, Code, InAppId, XsollaAccelByteAuthUtils::XsollaState(), OnAuthUpdate, OnAuthError);
}

void UXsollaAccelByteAuth::HandleLoginSuccess(const FXsollaLoginData& LoginData)
{
    LoginToAccelByte(LoginData);
}

void UXsollaAccelByteAuth::HandleLoginFailed(const FString& Code, const FString& Description)
{
    const FOnXsollaAccelByteLoginFailed LoginFailed = OnXsollaLoginFailed;
    ClearLoginDelegates();
    LoginFailed.ExecuteIfBound(Code, Description);
}

void UXsollaAccelByteAuth::HandleLoginCancelled()
{
    const FOnXsollaAccelByteLoginCancelled LoginCancelled = OnXsollaLoginCancelled;
    ClearLoginDelegates();
    LoginCancelled.ExecuteIfBound();
}

void UXsollaAccelByteAuth::LoginToAccelByte(const FXsollaLoginData& LoginData)
{
    LoginToAccelByteWithToken(LoginData.AuthToken.JWT, LoginData);
}

void UXsollaAccelByteAuth::LoginToAccelByteWithToken(const FString& AccessToken, TOptional<FXsollaLoginData> LoginData)
{
    if (AccessToken.IsEmpty())
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::XsollaTokenEmptyCode(), XsollaAccelByteAuthUtils::XsollaTokenEmptyDescription());
        return;
    }

    IOnlineSubsystem* AccelByteOSS = IOnlineSubsystem::Get(FName(XsollaAccelByteAuthUtils::AccelByteSubsystemName()));
    if (!AccelByteOSS)
    {
        HandleLoginFailed(AccelByteSubsystemMissingCode, AccelByteSubsystemMissingDescription);
        return;
    }

    IOnlineIdentityPtr IdentityInterface = AccelByteOSS->GetIdentityInterface();
    if (!IdentityInterface.IsValid())
    {
        HandleLoginFailed(AccelByteIdentityMissingCode, AccelByteIdentityMissingDescription);
        return;
    }

    const FString PlatformId = XsollaAccelByteAuthUtils::GetConfiguredXsollaPlatformId();
    const bool bCreateHeadlessAccount = XsollaAccelByteAuthUtils::ShouldCreateHeadlessAccount();

    const FOnlineAccountCredentialsAccelByte Credential(EAccelByteLoginType::OIDC, PlatformId, AccessToken, bCreateHeadlessAccount);
    AccelByteLoginCompleteDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateWeakLambda(this, [this, LoginData, AccessToken](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
    {
        UXsollaAccelByteAuth* SelfPtr = this;
        const TOptional<FXsollaLoginData> LoginDataCopy = LoginData;
        const FString AccessTokenCopy = AccessToken;

        SelfPtr->ClearAccelByteLoginDelegate(LocalUserNum);

        if (bWasSuccessful)
        {
            const FXsollaAccelByteLoginResult LoginResult = XsollaAccelByteAuthUtils::MakeLoginResult(LoginDataCopy, AccessTokenCopy, LocalUserNum, UserId.AsShared());

            const FOnXsollaAccelByteLoginSuccess LoginSuccess = SelfPtr->OnXsollaLoginSuccess;
            SelfPtr->ClearLoginDelegates();
            LoginSuccess.ExecuteIfBound(LoginResult);
        }
        else
        {
            SelfPtr->HandleLoginFailed(AccelByteOidcLoginFailedCode, Error);
        }
    }));

    if (!IdentityInterface->Login(0, Credential))
    {
        ClearAccelByteLoginDelegate(0);
        HandleLoginFailed(AccelByteOidcLoginNotStartedCode, AccelByteOidcLoginNotStartedDescription);
    }
}

void UXsollaAccelByteAuth::ClearLoginDelegates()
{
    XsollaAccelByteAuthUtils::ClearLoginDelegates(OnXsollaLoginSuccess, OnXsollaLoginFailed, OnXsollaLoginCancelled);
    bLoginInProgress = false;
}

void UXsollaAccelByteAuth::ClearSilentLoginDelegate(int32 LocalUserNum)
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

void UXsollaAccelByteAuth::ClearAccelByteLoginDelegate(int32 LocalUserNum)
{
    if (!AccelByteLoginCompleteDelegateHandle.IsValid())
    {
        return;
    }

    if (IOnlineSubsystem* AccelByteOSS = IOnlineSubsystem::Get(FName(XsollaAccelByteAuthUtils::AccelByteSubsystemName())))
    {
        if (IOnlineIdentityPtr IdentityInterface = AccelByteOSS->GetIdentityInterface())
        {
            IdentityInterface->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, AccelByteLoginCompleteDelegateHandle);
        }
    }

    AccelByteLoginCompleteDelegateHandle.Reset();
}

void UXsollaAccelByteAuth::OnSilentAuthLoginComplete(int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    UE_LOG(LogXsollaAccelByteAuth, Verbose, TEXT("%s subsystem login complete with status %s"), *SilentAuthSubsystemName.ToString(), bLoginWasSuccessful ? TEXT("success") : TEXT("failed"));
    ClearSilentLoginDelegate(LocalUserNum);

    if (!bLoginWasSuccessful)
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::SilentAuthFailedCode(), Error);
        return;
    }

    const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName);
    if (!Subsystem)
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::SilentAuthFailedCode(), Error);
        return;
    }

    const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
    if (!IdentityInterface.IsValid())
    {
        HandleLoginFailed(XsollaAccelByteAuthUtils::SilentAuthFailedCode(), XsollaAccelByteAuthUtils::SilentAuthIdentityMissingDescription());
        return;
    }

    const FString SessionTicket = IdentityInterface->GetAuthToken(LocalUserNum);
    LoginWithXsollaSessionTicketInternal(SilentAuthSubsystemName.ToString().ToLower(), SessionTicket, TEXT(""), AppId);
}


