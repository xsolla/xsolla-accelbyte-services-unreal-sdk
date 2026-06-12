// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * Minimal config-forwarding module for the Xsolla Backend SDK.
 *
 * This module loads at PostConfigInit — the earliest phase where GConfig is
 * available. Its sole purpose is to copy non-empty values from the Xsolla
 * settings ini section into the AccelByte UE SDK ini sections, so that when
 * AccelByteUe4Sdk loads at PreDefault it picks up the Xsolla-authored values.
 *
 * Source section (populated by UXsollaSettings via DefaultEngine.ini):
 *   [/Script/XsollaBackendSdk.XsollaSettings]
 *
 * Target sections:
 *   [/Script/AccelByteUe4Sdk.AccelByteSettings]        — client
 *   [/Script/AccelByteUe4Sdk.AccelByteServerSettings]  — server
 *
 * MODULE LOAD ORDERING MUST BE VALIDATED ON A REAL UE5.7 BUILD — confirm
 * AccelByte reads these keys AFTER this forwarding runs.
 *
 * Design choice: a separate module is used instead of moving XsollaBackendSdk
 * to PostConfigInit because the main module depends on Engine, OnlineSubsystem,
 * AccelByteUe4Sdk, and other heavy modules that are NOT available at
 * PostConfigInit. This tiny module depends only on Core and CoreUObject, making
 * it safe to load that early.
 */
class FXsollaBackendSdkConfigModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/**
	 * Reads Xsolla settings from GConfig and writes them into the AccelByte
	 * ini sections. Only non-empty Xsolla values are forwarded — blank fields
	 * never clobber existing AccelByte defaults.
	 */
	void ForwardConfig();
};
