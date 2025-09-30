// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "Interfaces/OnlineIdentityInterface.h"
#include "XsollaLoginBrowserWrapper.h"
#include "XsollaLoginSubsystem.h"

#include "XsollaAuth.generated.h"

#define XSOLLA_PLATFORM TEXT("xsolla")
#define XSOLLA_STATE TEXT("xsollabackend")

DECLARE_LOG_CATEGORY_EXTERN(LogXsollaAuth, Warning, All);

USTRUCT()
struct XSOLLABACKENDSDK_API FLoginUser
{
	GENERATED_BODY()
	int32 LocalUserNum;
	FUniqueNetIdPtr UserNetId;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnXsollaLoginSuccess, const FXsollaLoginData, LoginData, FLoginUser, LoginUser);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnXsollaLoginFailed, const FString&, Code, const FString&, Description);
DECLARE_DYNAMIC_DELEGATE(FOnXsollaLoginCancelled);


UCLASS()
class XSOLLABACKENDSDK_API UXsollaAuth : public UObject
{
	GENERATED_BODY()
public:
	void Initialize();
	void AuthWithXsollaWidget(const FOnXsollaLoginSuccess OnLoginSuccess = FOnXsollaLoginSuccess(), const FOnXsollaLoginCancelled OnLoginCancelled = FOnXsollaLoginCancelled(), const FOnXsollaLoginFailed OnLoginFailed = FOnXsollaLoginFailed());
	void SilentSubsystemAuth(const FName& SubsystemName, const FOnlineAccountCredentials& Credentials, const FString& InAppId = TEXT(""), const FOnXsollaLoginSuccess OnLoginSuccess = FOnXsollaLoginSuccess(), const FOnXsollaLoginFailed OnLoginFailed = FOnXsollaLoginFailed());
	void AuthWithSessionTicket(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& AppId, const FOnXsollaLoginSuccess OnLoginSuccess = FOnXsollaLoginSuccess(), const FOnXsollaLoginFailed OnLoginFailed = FOnXsollaLoginFailed());
private:
	UXsollaLoginBrowserWrapper* LoginWidget;
	FName SilentAuthSubsystemName;
	FString AppId;

	UPROPERTY()
	UXsollaLoginSubsystem* LoginSubsystem;

	FOnXsollaLoginSuccess OnXsollaLoginSuccess;
	FOnXsollaLoginFailed OnXsollaLoginFailed;
	FOnXsollaLoginCancelled OnXsollaLoginCancelled;
    
	FOnAuthUpdate OnAuthUpdate;
	FOnAuthError OnAuthError;
	FOnAuthCancel OnAuthCancel;

	UFUNCTION()
	void HandleLoginSuccess(const FXsollaLoginData& LoginData);
    
	UFUNCTION()
	void HandleLoginFailed(const FString& Code, const FString& Description);
    
	UFUNCTION()
	void HandleLoginCancelled();

	void LoginToGameService(const FXsollaLoginData& LoginData);
	void ClearLoginDelegate();

	void AuthWithSessionTicketInternal(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& AppId);
	void OnSilentAuthLoginComplete(int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};
