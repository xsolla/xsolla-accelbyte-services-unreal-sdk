// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaSettings.h"

UXsollaSettings::UXsollaSettings()
{
}

FName UXsollaSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName UXsollaSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FName UXsollaSettings::GetSectionName() const
{
	return TEXT("Xsolla Backend SDK");
}
