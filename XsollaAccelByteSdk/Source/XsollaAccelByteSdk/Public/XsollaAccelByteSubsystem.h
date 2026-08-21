// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "XsollaAccelByteSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXsollaAccelByteSubsystem, Warning, All);

class UXsollaAccelByteAuth;

UCLASS()
class XSOLLAACCELBYTESDK_API UXsollaAccelByteSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UXsollaAccelByteSubsystem* Get(const UObject* WorldContextObject);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UXsollaAccelByteAuth* GetAuth() const;

private:
    UPROPERTY()
    UXsollaAccelByteAuth* Auth = nullptr;
};