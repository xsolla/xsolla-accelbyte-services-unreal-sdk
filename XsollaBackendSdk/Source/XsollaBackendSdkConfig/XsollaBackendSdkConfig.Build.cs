// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

using UnrealBuildTool;

/// <summary>
/// Minimal module that forwards Xsolla config values into AccelByte ini sections
/// at LoadingPhase PostConfigInit — before AccelByteUe4Sdk reads them at PreDefault.
///
/// Intentionally depends ONLY on Core so that it can load at PostConfigInit
/// without pulling in engine subsystems that are not yet available.
/// </summary>
public class XsollaBackendSdkConfig : ModuleRules
{
    public XsollaBackendSdkConfig(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Only Core is needed — GConfig, FString, IModuleInterface all live here.
        // No CoreUObject dependency: this module does not use the UObject system,
        // which may not be fully initialized at PostConfigInit.
        PublicDependencyModuleNames.Add("Core");
    }
}
