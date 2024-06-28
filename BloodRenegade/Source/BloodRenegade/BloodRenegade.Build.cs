// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BloodRenegade : ModuleRules
{
	public BloodRenegade(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule", "GameplayTasks" ,"UMG", "SlateCore", "GameplayAbilities", "GameplayTags", "NavigationSystem", "Niagara", "ImGui", "ApplicationCore", "GenericGraphRuntime", "AkAudio", "PhysicsCore" });
		
		// Tell the compiler we want to import the ImPlot symbols when linking against ImGui plugin 
		PrivateDefinitions.Add("IMPLOT_API=DLLIMPORT");
	}
}
