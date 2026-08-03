// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GADE7322_TD : ModuleRules
{
	public GADE7322_TD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[]
		{
			"GADE7322_TD",
			"GADE7322_TD/Any",
			"GADE7322_TD/Events",
			"GADE7322_TD/Pawns",
			"GADE7322_TD/Pawns/PlayerPawns"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}