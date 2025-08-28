#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/AccelByteInstance.h"
#include "Core/AccelByteApiClient.h"
#include "Core/AccelByteServerApiClient.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemAccelByte.h"
#include "XsollaWrapperSdkGameSubsystem.generated.h"

namespace Xsolla = AccelByte;
using namespace Xsolla;

using FXsollaSdkInstancePtr = FAccelByteInstancePtr;
using FOnlineSubsystemXsolla = FOnlineSubsystemAccelByte;

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

    FApiClientPtr GetGameSdkApiClient();

    FServerApiClientPtr GetGameSdkServerApiClient();

    UFUNCTION(BlueprintCallable, Category = "XsollaWrapperSdk")
    void DoSomething(FString Message);

protected:

    FXsollaSdkInstancePtr XsollaSdkInstance;
    
};
