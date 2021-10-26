// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ArshDemo : ModuleRules
{
	public ArshDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
