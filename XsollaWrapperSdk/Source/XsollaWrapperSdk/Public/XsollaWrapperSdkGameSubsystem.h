// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "XsollaMapping.h"
#include "Core/XsollaInterfaceManager.h"

#include "XsollaWrapperSdkGameSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXsollaWrapperSubsystem, Warning, All);

class UXsollaInterfaceManager;

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
    UXsollaInterfaceManager* GetInterfaceManager() const;

    FApiClientPtr GetGameSdkApiClient();
    FServerApiClientPtr GetGameSdkServerApiClient();
protected:
    FXsollaSdkInstancePtr XsollaSdkInstance;

    UPROPERTY()
    UXsollaInterfaceManager* InterfaceManager;
};
