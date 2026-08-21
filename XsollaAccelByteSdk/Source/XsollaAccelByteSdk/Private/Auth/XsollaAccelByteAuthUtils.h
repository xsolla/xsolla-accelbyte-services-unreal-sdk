// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "Auth/XsollaAccelByteAuthTypes.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/Optional.h"
#include "XsollaAccelByteSettings.h"
#include "XsollaLoginTypes.h"

namespace XsollaAccelByteAuthUtils
{
inline const TCHAR* XsollaState()
{
    return TEXT("xsolla_accelbyte_auth");
}

inline const TCHAR* AccelByteSubsystemName()
{
    return TEXT("AccelByte");
}

inline FName SteamSubsystemName()
{
    return FName(TEXT("Steam"));
}

inline FString XsollaLoginSubsystemMissingCode()
{
    return TEXT("xsolla-login-subsystem-missing");
}

inline FString XsollaLoginSubsystemMissingDescription()
{
    return TEXT("Xsolla login subsystem is not available.");
}

inline FString XsollaTokenEmptyCode()
{
    return TEXT("xsolla-token-empty");
}

inline FString XsollaTokenEmptyDescription()
{
    return TEXT("Xsolla access token is empty.");
}

inline FString SilentAuthFailedCode()
{
    return TEXT("silent-auth-failed");
}

inline FString LoginInProgressCode()
{
    return TEXT("xsolla-login-in-progress");
}

inline FString LoginInProgressDescription()
{
    return TEXT("A Xsolla AccelByte login request is already in progress.");
}

inline FString SubsystemAuthFailedCode(const FName& SubsystemName)
{
    return FString::Printf(TEXT("%s-auth-failed"), *SubsystemName.ToString());
}

inline FString SubsystemInitializationFailedDescription(const FName& SubsystemName)
{
    return FString::Printf(TEXT("%s subsystem initialization failed."), *SubsystemName.ToString());
}

inline FString SubsystemIdentityMissingDescription(const FName& SubsystemName)
{
    return FString::Printf(TEXT("%s identity interface is not available."), *SubsystemName.ToString());
}

inline FString SilentAuthIdentityMissingDescription()
{
    return TEXT("Silent auth identity interface is not available.");
}

inline FString GetConfiguredXsollaPlatformId()
{
    const UXsollaAccelByteSettings* Settings = GetDefault<UXsollaAccelByteSettings>();
    return Settings && !Settings->XsollaPlatformId.IsEmpty() ? Settings->XsollaPlatformId : TEXT("xsolla");
}

inline bool ShouldCreateHeadlessAccount()
{
    const UXsollaAccelByteSettings* Settings = GetDefault<UXsollaAccelByteSettings>();
    return !Settings || Settings->bCreateHeadlessAccount;
}

inline void ClearLoginDelegates(FOnXsollaAccelByteLoginSuccess& OnLoginSuccess, FOnXsollaAccelByteLoginFailed& OnLoginFailed, FOnXsollaAccelByteLoginCancelled& OnLoginCancelled)
{
    OnLoginSuccess.Unbind();
    OnLoginFailed.Unbind();
    OnLoginCancelled.Unbind();
}

inline FXsollaAccelByteLoginResult MakeLoginResult(const TOptional<FXsollaLoginData>& LoginData, const FString& AccessToken, int32 LocalUserNum = 0, FUniqueNetIdPtr UserNetId = nullptr)
{
    FXsollaAccelByteLoginResult LoginResult;
    LoginResult.LocalUserNum = LocalUserNum;
    LoginResult.UserNetId = UserNetId;

    if (LoginData.IsSet())
    {
        LoginResult.XsollaAccessToken = LoginData.GetValue().AuthToken.JWT;
        LoginResult.XsollaRefreshToken = LoginData.GetValue().AuthToken.RefreshToken;
        LoginResult.XsollaTokenExpiresAt = LoginData.GetValue().AuthToken.ExpiresAt;
    }
    else
    {
        LoginResult.XsollaAccessToken = AccessToken;
    }

    return LoginResult;
}
}
