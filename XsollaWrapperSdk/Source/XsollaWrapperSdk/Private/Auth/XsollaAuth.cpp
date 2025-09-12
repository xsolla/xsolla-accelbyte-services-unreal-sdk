// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "Auth/XsollaAuth.h"
#include "XsollaMapping.h"
#include "XsollaWrapperSdkGameSubsystem.h"
#include "Core/XsollaInterfaceManager.h"

#include "OnlineIdentityInterfaceAccelByte.h"
#include "InterfaceModels/OnlineIdentityInterfaceAccelByteModels.h"

DEFINE_LOG_CATEGORY(LogXsollaAuth);

void UXsollaAuth::Initialize()
{
	OnAuthUpdate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UXsollaAuth, HandleLoginSuccess));
	OnAuthError.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UXsollaAuth, HandleLoginFailed));
	OnAuthCancel.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UXsollaAuth, HandleLoginCancelled));

	const UGameInstance* GameInstance = GetTypedOuter<UGameInstance>();
	if(GameInstance)
	{
		LoginSubsystem = GameInstance->GetSubsystem<UXsollaLoginSubsystem>();
	}
}

void UXsollaAuth::AuthWithXsollaWidget(const FOnXsollaLoginSuccess OnLoginSuccess, const FOnXsollaLoginCancelled OnLoginCancelled, const FOnXsollaLoginFailed OnLoginFailed)
{
	OnXsollaLoginSuccess = OnLoginSuccess;
	OnXsollaLoginCancelled = OnLoginCancelled;
	OnXsollaLoginFailed = OnLoginFailed;
    
	if (LoginSubsystem)
	{
		LoginSubsystem->AuthWithXsollaWidget(GetWorld(), LoginWidget, OnAuthUpdate, OnAuthCancel, OnAuthError);
	}
}

void UXsollaAuth::SilentSubsystemAuth(const FName& SubsystemName, const FOnlineAccountCredentials& Credentials, const FString& InAppId, const FOnXsollaLoginSuccess OnLoginSuccess, const FOnXsollaLoginFailed OnLoginFailed)
{
	SilentAuthSubsystemName = SubsystemName;
	AppId = InAppId;

	if(SubsystemName.IsEqual(STEAM_SUBSYSTEM) && InAppId.IsEmpty())
	{
		GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"),AppId, GEngineIni);
	}

	if(AppId.IsEmpty())
	{
		UE_LOG(LogXsollaAuth, Warning, TEXT("AppId is empty. Login using subsystem may failed!!"));
	}
	
	OnXsollaLoginSuccess = OnLoginSuccess;
	OnXsollaLoginFailed = OnLoginFailed;
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName);
	if(Subsystem)
	{
		IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();

		IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &UXsollaAuth::OnSilentAuthLoginComplete));
		IdentityInterface->Login(0, Credentials);
	}
	else
	{
		UE_LOG(LogXsollaAuth, Warning, TEXT("Cannot find %s OSS!!"), *SilentAuthSubsystemName.ToString());
	}
}

void UXsollaAuth::AuthWithSessionTicket(const FString& Provider, const FString& SessionTicket, const FString& Code, const FString& InAppId,
	const FOnXsollaLoginSuccess OnLoginSuccess, const FOnXsollaLoginFailed OnLoginFailed)
{
	OnXsollaLoginSuccess = OnLoginSuccess;
	OnXsollaLoginFailed = OnLoginFailed;
	AuthWithSessionTicketInternal(Provider, SessionTicket, Code, InAppId);
}

void UXsollaAuth::AuthWithSessionTicketInternal(const FString& Provider, const FString& SessionTicket, const FString& Code,
	const FString& InAppId)
{
	LoginSubsystem->AuthenticateWithSessionTicket(Provider, SessionTicket, Code, InAppId, XSOLLA_STATE, OnAuthUpdate, OnAuthError);
}

void UXsollaAuth::HandleLoginSuccess(const FXsollaLoginData& LoginData)
{
	LoginToGameService(LoginData);
}

void UXsollaAuth::HandleLoginFailed(const FString& Code, const FString& Description)
{
	OnXsollaLoginFailed.ExecuteIfBound(Code, Description);
	ClearLoginDelegate();
}

void UXsollaAuth::HandleLoginCancelled()
{
	OnXsollaLoginCancelled.ExecuteIfBound();
	ClearLoginDelegate();
}

void UXsollaAuth::LoginToGameService(const FXsollaLoginData& LoginData)
{
	const UXsollaWrapperSdkGameSubsystem* Subsystem = GetTypedOuter<UXsollaWrapperSdkGameSubsystem>();
	const UXsollaInterfaceManager* InterfaceManager = Subsystem->GetInterfaceManager();
	const FOnlineIdentityXsollaPtr IdentityInterface = StaticCastSharedPtr<FOnlineIdentityXsolla>(InterfaceManager->GetIdentityInterface());
	
	if(IdentityInterface)
	{
		const FOnlineAccountCredentialsAccelByte Credential(EAccelByteLoginType::OIDC, XSOLLA_PLATFORM, LoginData.AuthToken.JWT);
		IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateLambda([this, LoginData](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
		{
			if(bWasSuccessful)
			{
				FLoginUser LoginUser;
				LoginUser.LocalUserNum = LocalUserNum;
				LoginUser.UserNetId = UserId.AsShared();
				OnXsollaLoginSuccess.ExecuteIfBound(LoginData, LoginUser);
				ClearLoginDelegate();
			}
			else
			{
				HandleLoginFailed(TEXT("xsolla subsystem login failed"), Error);
			}
		}));
		IdentityInterface->Login(0, Credential);
	}
}

void UXsollaAuth::ClearLoginDelegate()
{
	OnXsollaLoginSuccess.Unbind();
	OnXsollaLoginFailed.Unbind();
	OnXsollaLoginCancelled.Unbind();
}

void UXsollaAuth::OnSilentAuthLoginComplete(int32 LocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& UserId,
                                       const FString& Error)
{
	UE_LOG(LogXsollaAuth, Verbose, TEXT("%s subsystem login complete with status %s"), *SilentAuthSubsystemName.ToString(), bLoginWasSuccessful ? TEXT("success") : TEXT("failed"));

	if(bLoginWasSuccessful)
	{
		const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(SilentAuthSubsystemName);
	
		if(Subsystem)
		{
			UE_LOG(LogXsollaAuth, Verbose, TEXT("Login to xsolla using %s ticket"), *SilentAuthSubsystemName.ToString());
		
			const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
			IdentityInterface->ClearOnLoginCompleteDelegates(LocalUserNum, this);
		
			// Get session ticket and login to xsolla
			const FString SessionTicket = IdentityInterface->GetAuthToken(LocalUserNum);
			AuthWithSessionTicketInternal(SilentAuthSubsystemName.ToString().ToLower(), SessionTicket, TEXT(""), AppId);
		}
		else
		{
			HandleLoginFailed(TEXT("silent auth failed"), Error);
		}
	}
	else
	{
		HandleLoginFailed(TEXT("silent auth failed"), Error);
	}
}
