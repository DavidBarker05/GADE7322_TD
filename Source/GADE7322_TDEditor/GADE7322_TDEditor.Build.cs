using UnrealBuildTool;

public class GADE7322_TDEditor : ModuleRules
{
    public GADE7322_TDEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "BlueprintGraph",
            "KismetCompiler",
            "GADE7322_TD"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PublicIncludePaths.AddRange(new string[]
        {
            "GADE7322_TDEditor",
            "GADE7322_TDEditor/Any"
        });
    }
}
