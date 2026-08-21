// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "XsollaLoginTypes.h"

#include "XsollaAccelByteAuthCallbackProxy.generated.h"

class UXsollaLoginBrowserWrapper;

UCLASS()
class UXsollaAccelByteAuthCallbackProxy : public UObject
{
    GENERATED_BODY()

public:
    TFunction<void(const FXsollaLoginData&)> OnAuthUpdate;
    TFunction<void(const FString&, const FString&)> OnAuthError;
    TFunction<void()> OnAuthCancel;

    UPROPERTY()
    UXsollaLoginBrowserWrapper* LoginWidget = nullptr;

    UFUNCTION()
    void HandleAuthUpdate(const FXsollaLoginData& LoginData);

    UFUNCTION()
    void HandleAuthError(const FString& Code, const FString& Description);

    UFUNCTION()
    void HandleAuthCancel();
};
