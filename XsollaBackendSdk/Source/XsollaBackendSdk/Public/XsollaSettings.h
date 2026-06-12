// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "XsollaSettings.generated.h"

/**
 * @brief Xsolla Backend SDK configuration.
 *
 * Values set here are forwarded into the underlying AccelByte UE SDK settings
 * at PostConfigInit, before the SDK or OSS reads them.
 *
 * Config section in DefaultEngine.ini:
 *   [/Script/XsollaBackendSdk.XsollaSettings]
 *
 * Appears in the Unreal Editor under:
 *   Project Settings -> Plugins -> Xsolla Backend SDK
 *
 * ===================================================================
 *  HOW CONFIGURATION FLOWS
 * ===================================================================
 *
 * 1. At PostConfigInit the tiny XsollaBackendSdkConfig module copies
 *    every non-empty value from the Xsolla section above into the
 *    AccelByte ini sections:
 *      [/Script/AccelByteUe4Sdk.AccelByteSettings]        (client)
 *      [/Script/AccelByteUe4Sdk.AccelByteServerSettings]  (server)
 *    This happens before AccelByteUe4Sdk loads at PreDefault, so the
 *    SDK reads the Xsolla-authored values automatically.
 *
 * 2. On the non-OSS path (no OnlineSubsystemAccelByte active),
 *    UXsollaBackendSdkGameSubsystem::Initialize() additionally
 *    constructs AccelByte::Settings / ServerSettings structs from
 *    this config object and passes them to the settings-accepting
 *    CreateAccelByteInstance() overload, ensuring the direct-SDK
 *    instance also honours these values.
 *
 * 3. All credentials and base URLs now flow from this single Xsolla
 *    config class. Developers should NOT edit the AccelByte ini
 *    sections directly.
 *
 * ===================================================================
 *  IMPORTANT — ACCELBYTE-NAMED RUNTIME KEYS (DO NOT RENAME)
 * ===================================================================
 *
 * The following runtime keys MUST keep their AccelByte names.
 * Renaming them will break login and online-subsystem look-up:
 *
 *   [OnlineSubsystemAccelByte]          — OSS configuration section
 *   DefaultPlatformService=AccelByte    — in [OnlineSubsystem]
 *
 * These are internal engine identifiers; the Xsolla wrapper leaves
 * them as-is.
 */
UCLASS(Config = Engine, DefaultConfig, meta = (DisplayName = "Xsolla Backend SDK Settings"))
class XSOLLABACKENDSDK_API UXsollaSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UXsollaSettings();

	//~ Begin UDeveloperSettings Interface
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
	//~ End UDeveloperSettings Interface

	// ---------------------------------------------------------------
	// Client Settings
	// ---------------------------------------------------------------

	/** Client ID used by the game client to authenticate with the Xsolla backend. */
	UPROPERTY(Config, EditAnywhere, Category = "Client")
	FString ClientId;

	/** Game namespace. */
	UPROPERTY(Config, EditAnywhere, Category = "Client")
	FString Namespace;

	/** Publisher namespace that owns the game namespace. */
	UPROPERTY(Config, EditAnywhere, Category = "Client")
	FString PublisherNamespace;

	/** OAuth redirect URI for the client. */
	UPROPERTY(Config, EditAnywhere, Category = "Client")
	FString RedirectURI;

	/** Base URL of the Xsolla backend (e.g. https://example.gamingservices.xsolla.com). */
	UPROPERTY(Config, EditAnywhere, Category = "Client")
	FString BaseUrl;

	// ---------------------------------------------------------------
	// Server Settings
	// ---------------------------------------------------------------

	/** Client ID used by the dedicated server / admin to authenticate. */
	UPROPERTY(Config, EditAnywhere, Category = "Server")
	FString ServerClientId;

	/** Client secret for the server credential pair. */
	UPROPERTY(Config, EditAnywhere, Category = "Server")
	FString ServerClientSecret;

	/** Server-side game namespace. */
	UPROPERTY(Config, EditAnywhere, Category = "Server")
	FString ServerNamespace;

	/** Publisher namespace for the server. */
	UPROPERTY(Config, EditAnywhere, Category = "Server")
	FString ServerPublisherNamespace;

	/** Base URL used by the dedicated server (may differ from the client URL). */
	UPROPERTY(Config, EditAnywhere, Category = "Server")
	FString ServerBaseUrl;
};
