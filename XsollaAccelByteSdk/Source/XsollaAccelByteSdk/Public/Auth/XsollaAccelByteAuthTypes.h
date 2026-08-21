// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"

struct XSOLLAACCELBYTESDK_API FXsollaAccelByteLoginResult
{
    int32 LocalUserNum = 0;
    FUniqueNetIdPtr UserNetId;
    FString XsollaAccessToken;
    FString XsollaRefreshToken;
    int64 XsollaTokenExpiresAt = 0;
};

DECLARE_DELEGATE_OneParam(FOnXsollaAccelByteLoginSuccess, const FXsollaAccelByteLoginResult&);
DECLARE_DELEGATE_TwoParams(FOnXsollaAccelByteLoginFailed, const FString&, const FString&);
DECLARE_DELEGATE(FOnXsollaAccelByteLoginCancelled);
