// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "XsollaAccelByteSettings.generated.h"

UCLASS(Config = Engine, DefaultConfig, meta = (DisplayName = "Xsolla AccelByte SDK"))
class XSOLLAACCELBYTESDK_API UXsollaAccelByteSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UXsollaAccelByteSettings();

    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;
    virtual FName GetSectionName() const override;

    UPROPERTY(Config, EditAnywhere, Category = "Xsolla AccelByte")
    FString XsollaPlatformId = TEXT("xsolla");

    UPROPERTY(Config, EditAnywhere, Category = "Xsolla AccelByte")
    bool bCreateHeadlessAccount = true;
};
