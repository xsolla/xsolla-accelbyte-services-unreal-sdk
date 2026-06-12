// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaBackendSdkConfig.h"

#include "Misc/ConfigCacheIni.h"

/**
 * MODULE LOAD ORDERING MUST BE VALIDATED ON A REAL UE5.7 BUILD — confirm
 * AccelByte reads these keys AFTER this forwarding runs.
 *
 * Expected load order:
 *   1. PostConfigInit  — this module (XsollaBackendSdkConfig) writes GConfig
 *   2. PreDefault      — AccelByteUe4Sdk reads GConfig in StartupModule
 *   3. Default         — XsollaBackendSdk (main module), OnlineSubsystemAccelByte
 *
 * This module performs ONLY raw GConfig string operations. It does NOT
 * instantiate any UObject CDO, nor depend on any engine subsystem beyond Core.
 */

DEFINE_LOG_CATEGORY_STATIC(LogXsollaBackendConfig, Log, All);

#define LOCTEXT_NAMESPACE "FXsollaBackendSdkConfigModule"

// ---------------------------------------------------------------------------
// Ini section paths
// ---------------------------------------------------------------------------

/** Source: Xsolla wrapper settings, populated from DefaultEngine.ini by
 *  UXsollaSettings (Config = Engine, DefaultConfig). */
static const TCHAR* XsollaSection = TEXT("/Script/XsollaBackendSdk.XsollaSettings");

/** Target: AccelByte client settings, read by AccelByteUe4Sdk at PreDefault. */
static const TCHAR* ABClientSection = TEXT("/Script/AccelByteUe4Sdk.AccelByteSettings");

/** Target: AccelByte server settings, read by AccelByteUe4Sdk at PreDefault. */
static const TCHAR* ABServerSection = TEXT("/Script/AccelByteUe4Sdk.AccelByteServerSettings");

// ---------------------------------------------------------------------------
// Module lifecycle
// ---------------------------------------------------------------------------

void FXsollaBackendSdkConfigModule::StartupModule()
{
	ForwardConfig();
	UE_LOG(LogXsollaBackendConfig, Log, TEXT("XsollaBackendSdkConfig module started — config forwarding complete."));
}

void FXsollaBackendSdkConfigModule::ShutdownModule()
{
}

// ---------------------------------------------------------------------------
// Config forwarding
// ---------------------------------------------------------------------------

void FXsollaBackendSdkConfigModule::ForwardConfig()
{
	if (!GConfig)
	{
		UE_LOG(LogXsollaBackendConfig, Warning,
			TEXT("GConfig is null at PostConfigInit — cannot forward Xsolla settings. "
				 "MODULE LOAD ORDERING MUST BE VALIDATED ON A REAL UE5.7 BUILD."));
		return;
	}

	// Helper lambda: read a key from the Xsolla section and, if non-empty,
	// write it into the destination AccelByte section under a (possibly
	// different) key name.
	auto ForwardKey = [](
		const TCHAR* SrcSection, const TCHAR* SrcKey,
		const TCHAR* DstSection, const TCHAR* DstKey)
	{
		FString Value;
		if (GConfig->GetString(SrcSection, SrcKey, Value, GEngineIni) && !Value.IsEmpty())
		{
			GConfig->SetString(DstSection, DstKey, *Value, GEngineIni);
			UE_LOG(LogXsollaBackendConfig, Verbose,
				TEXT("Forwarded [%s] %s -> [%s] %s"), SrcSection, SrcKey, DstSection, DstKey);
		}
	};

	// -----------------------------------------------------------------
	// Client settings
	//   Xsolla field          -> AccelByteSettings field
	// -----------------------------------------------------------------
	ForwardKey(XsollaSection, TEXT("ClientId"),           ABClientSection, TEXT("ClientId"));
	ForwardKey(XsollaSection, TEXT("Namespace"),          ABClientSection, TEXT("Namespace"));
	ForwardKey(XsollaSection, TEXT("PublisherNamespace"), ABClientSection, TEXT("PublisherNamespace"));
	ForwardKey(XsollaSection, TEXT("RedirectURI"),        ABClientSection, TEXT("RedirectURI"));
	ForwardKey(XsollaSection, TEXT("BaseUrl"),            ABClientSection, TEXT("BaseUrl"));

	// -----------------------------------------------------------------
	// Server settings
	//   Xsolla field                -> AccelByteServerSettings field
	// -----------------------------------------------------------------
	ForwardKey(XsollaSection, TEXT("ServerClientId"),           ABServerSection, TEXT("ClientId"));
	ForwardKey(XsollaSection, TEXT("ServerClientSecret"),       ABServerSection, TEXT("ClientSecret"));
	ForwardKey(XsollaSection, TEXT("ServerNamespace"),          ABServerSection, TEXT("Namespace"));
	ForwardKey(XsollaSection, TEXT("ServerPublisherNamespace"), ABServerSection, TEXT("PublisherNamespace"));
	ForwardKey(XsollaSection, TEXT("ServerBaseUrl"),            ABServerSection, TEXT("BaseUrl"));
}

IMPLEMENT_MODULE(FXsollaBackendSdkConfigModule, XsollaBackendSdkConfig)

#undef LOCTEXT_NAMESPACE
