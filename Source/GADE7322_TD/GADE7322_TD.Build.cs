// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// ReSharper disable RedundantExplicitArrayCreation
// ReSharper disable UseCollectionExpression
// ReSharper disable InconsistentNaming

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
			"EnhancedInput",
			"ProceduralMeshComponent",
			"GameplayTasks",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"AIModule",
			"NavigationSystem",
			"UMG",
			"Slate",
			"MeshDescription",
			"StaticMeshDescription",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		PublicIncludePaths.AddRange(new string[]
		{
			"GADE7322_TD",
			"GADE7322_TD/Any",
			"GADE7322_TD/Events",
			"GADE7322_TD/Player",
			"GADE7322_TD/Player/TowerDefence",
			"GADE7322_TD/Player/TowerDefence/Components",
			"GADE7322_TD/ProceduralGen",
			"GADE7322_TD/TowerDefencePawns",
			"GADE7322_TD/TowerDefencePawns/AI",
			"GADE7322_TD/TowerDefencePawns/AI/CommonTasks",
			"GADE7322_TD/TowerDefencePawns/AI/ProximityPerception",
			"GADE7322_TD/TowerDefencePawns/Attackers",
			"GADE7322_TD/TowerDefencePawns/Attackers/AI",
			"GADE7322_TD/TowerDefencePawns/Attackers/AI/CommonTasks",
			"GADE7322_TD/TowerDefencePawns/Attackers/Skeleton",
			"GADE7322_TD/TowerDefencePawns/Attackers/Skeleton/AI",
			"GADE7322_TD/TowerDefencePawns/Components",
			"GADE7322_TD/TowerDefencePawns/Defenders",
			"GADE7322_TD/TowerDefencePawns/Defenders/AI",
			"GADE7322_TD/TowerDefencePawns/Defenders/AI/CommonTasks",
			"GADE7322_TD/TowerDefencePawns/Defenders/Warrior",
			"GADE7322_TD/TowerDefencePawns/Defenders/Warrior/AI",
			"GADE7322_TD/TowerDefencePawns/Tower",
			"GADE7322_TD/UI",
			"GADE7322_TD/UI/TowerDefence",
			"GADE7322_TD/UI/TowerDefence/Widgets",
			"GADE7322_TD/UI/Widgets"
		});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}