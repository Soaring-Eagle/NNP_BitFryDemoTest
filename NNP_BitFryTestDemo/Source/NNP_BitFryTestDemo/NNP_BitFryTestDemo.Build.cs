// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NNP_BitFryTestDemo : ModuleRules
{
	public NNP_BitFryTestDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		
		PublicFrameworks.AddRange(new string[] {"GameController", "CoreHaptics"});
	}
}
