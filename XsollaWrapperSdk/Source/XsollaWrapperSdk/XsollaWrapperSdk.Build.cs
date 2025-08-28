using UnrealBuildTool;

public class XsollaWrapperSdk : ModuleRules
{
    public XsollaWrapperSdk(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "OnlineSubsystem",
                "AccelByteUe4Sdk",
                "AccelByteNetworkUtilities",
                "OnlineSubsystemAccelByte",
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
