// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "Core/XsollaInterfaceManager.h"

UXsollaInterfaceManager::UXsollaInterfaceManager()
{
}

void UXsollaInterfaceManager::Initialize(UGameInstanceSubsystem* GameSubsystem)
{
	XsollaBackendSdkGameSubsystem = GameSubsystem;
	XsollaAuth = NewObject<UXsollaAuth>(this);
	XsollaAuth->Initialize();
}

UXsollaAuth* UXsollaInterfaceManager::GetAuth() const
{
	return XsollaAuth;
}
