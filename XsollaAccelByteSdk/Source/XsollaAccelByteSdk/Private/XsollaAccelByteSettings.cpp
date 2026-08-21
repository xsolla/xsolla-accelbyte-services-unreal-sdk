// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "XsollaAccelByteSettings.h"

UXsollaAccelByteSettings::UXsollaAccelByteSettings()
{
}

FName UXsollaAccelByteSettings::GetContainerName() const
{
    return TEXT("Project");
}

FName UXsollaAccelByteSettings::GetCategoryName() const
{
    return TEXT("Plugins");
}

FName UXsollaAccelByteSettings::GetSectionName() const
{
    return TEXT("Xsolla AccelByte SDK");
}
