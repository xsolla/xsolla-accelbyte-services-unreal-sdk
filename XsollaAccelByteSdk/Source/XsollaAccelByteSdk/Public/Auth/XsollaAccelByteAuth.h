// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Auth/XsollaAccelByteAuthTypes.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/Optional.h"
#include "XsollaLoginBrowserWrapper.h"
#include "XsollaLoginSubsystem.h"
#include "XsollaLoginTypes.h"

#include "XsollaAccelByteAuth.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXsollaAccelByteAuth, Warning, All);


UCLASS(BlueprintType)
class XSOLLAACCELBYTESDK_API UXsollaAccelByteAuth : public UObject
{
    GENERATED_BODY()

public:
    void Initialize();

    void LoginWithXsollaAccount(const FOnXsollaAccelByteLoginSuccess OnLoginSuccess = FOnXsollaAccelByteLoginSuccess(), const FOnXsollaAccelByteLoginCancelled OnLoginCancelled = FOnXsollaAccelByteLoginCancelled(), const FOnXsollaAccelByteLoginFailed OnLoginFailed = FOnXsollaAccelByteLoginFailed());

    void LoginWithXsollaSilentAuth(const FName& SubsystemName, const FOnlineAccountCredentials& Credentials, const FString& InAppId = TEXT(""), const FOnXsollaAccelByteLoginSuccess OnLoginSuccess = FOnXsollaAccelByteLoginSuccess(), const FOnXsollaAccelByteLoginFailed OnLoginFailed = FOnXsollaAccelByteLoginFailed());

    void LoginWithXsollaSessionTicket(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& AppId, const FOnXsollaAccelByteLoginSuccess OnLoginSuccess = FOnXsollaAccelByteLoginSuccess(), const FOnXsollaAccelByteLoginFailed OnLoginFailed = FOnXsollaAccelByteLoginFailed());

    void LoginWithXsollaAccessToken(const FString& AccessToken, const FOnXsollaAccelByteLoginSuccess OnLoginSuccess = FOnXsollaAccelByteLoginSuccess(), const FOnXsollaAccelByteLoginFailed OnLoginFailed = FOnXsollaAccelByteLoginFailed());

private:
    bool TryBeginLogin(const FOnXsollaAccelByteLoginFailed& OnLoginFailed);

    UPROPERTY()
    UXsollaLoginBrowserWrapper* LoginWidget = nullptr;

    UPROPERTY()
    UXsollaLoginSubsystem* LoginSubsystem = nullptr;

    FName SilentAuthSubsystemName;
    FString AppId;
    FDelegateHandle SilentLoginCompleteDelegateHandle;
    FDelegateHandle AccelByteLoginCompleteDelegateHandle;
    bool bLoginInProgress = false;

    FOnXsollaAccelByteLoginSuccess OnXsollaLoginSuccess;
    FOnXsollaAccelByteLoginFailed OnXsollaLoginFailed;
    FOnXsollaAccelByteLoginCancelled OnXsollaLoginCancelled;

    FOnAuthUpdate OnAuthUpdate;
    FOnAuthError OnAuthError;
    FOnAuthCancel OnAuthCancel;

    UFUNCTION()
    void HandleLoginSuccess(const FXsollaLoginData& LoginData);

    UFUNCTION()
    void HandleLoginFailed(const FString& Code, const FString& Description);

    UFUNCTION()
    void HandleLoginCancelled();

    void LoginToAccelByte(const FXsollaLoginData& LoginData);
    void LoginToAccelByteWithToken(const FString& AccessToken, TOptional<FXsollaLoginData> LoginData = TOptional<FXsollaLoginData>());
    void ClearLoginDelegates();
    void ClearSilentLoginDelegate(int32 LocalUserNum);
    void ClearAccelByteLoginDelegate(int32 LocalUserNum);
    void LoginWithXsollaSessionTicketInternal(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& AppId);
    void OnSilentAuthLoginComplete(int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};


