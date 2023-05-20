// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tears_Recreation : ModuleRules
{
	public Tears_Recreation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
