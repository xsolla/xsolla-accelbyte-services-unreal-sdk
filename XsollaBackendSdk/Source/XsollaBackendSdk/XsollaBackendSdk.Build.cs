// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

using UnrealBuildTool;

public class XsollaBackendSdk : ModuleRules
{
    public XsollaBackendSdk(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "DeveloperSettings",
                "OnlineSubsystem",
                "UMG",
                "AccelByteUe4Sdk",
                "AccelByteNetworkUtilities",
                "OnlineSubsystemAccelByte",
                "OnlineSubsystemSteam",
                "XsollaLogin"
            }
        );
            
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
            }
        );
    }
}
