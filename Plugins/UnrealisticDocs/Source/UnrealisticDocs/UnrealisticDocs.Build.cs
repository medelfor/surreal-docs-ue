/* Copyright © 2022, Medelför OÜ. All rights reserved. */

using System;
using UnrealBuildTool;
using System.IO;

public class UnrealisticDocs : ModuleRules
{
	public UnrealisticDocs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		bUseUnity = false;

		String Private = Path.Combine(ModuleDirectory, "Private");

		PrivateIncludePaths.Add(Private);
		PrivateIncludePaths.Add(Path.Combine(Private, "Enumeration"));

		//@todo remove RHI
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UnrealEd",
				"PropertyEditor",
				"EditorStyle",
				"BlueprintGraph",
				"GraphEditor",
				"MainFrame",
				"Json",
				"JsonUtilities",
				"DeveloperSettings",
				"UMG"
			}
		);
	}
}
