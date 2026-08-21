// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaAccelByteSubsystem.h"

#include "Auth/XsollaAccelByteAuth.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogXsollaAccelByteSubsystem);

UXsollaAccelByteSubsystem* UXsollaAccelByteSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    return GameInstance ? GameInstance->GetSubsystem<UXsollaAccelByteSubsystem>() : nullptr;
}

void UXsollaAccelByteSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Auth = NewObject<UXsollaAccelByteAuth>(this);
    Auth->Initialize();

    UE_LOG(LogXsollaAccelByteSubsystem, Verbose, TEXT("XsollaAccelByteSubsystem has been initialized."));
}

void UXsollaAccelByteSubsystem::Deinitialize()
{
    UE_LOG(LogXsollaAccelByteSubsystem, Verbose, TEXT("XsollaAccelByteSubsystem has been deinitialized."));

    Auth = nullptr;

    Super::Deinitialize();
}

UXsollaAccelByteAuth* UXsollaAccelByteSubsystem::GetAuth() const
{
    return Auth;
}