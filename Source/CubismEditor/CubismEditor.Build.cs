// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CubismEditor : ModuleRules
{
	public CubismEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"MainFrame",
//				"WorkspaceMenuStructure",
			});

		PrivateIncludePaths.AddRange(
			new string[] {
				"CubismEditor/Private",
				"CubismEditor/Private/AssetTools",
				"CubismEditor/Private/Factories",
				"CubismEditor/Private/Shared",
				"CubismEditor/Private/Styles",
				"CubismEditor/Private/Toolkits",
				"CubismEditor/Private/Widgets",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"ContentBrowser",
				"Core",
				"CoreUObject",
				"DesktopWidgets",
				"EditorStyle",
				"Engine",
				"InputCore",
				"Projects",
				"Slate",
				"SlateCore",
				"Cubism",
				"UnrealEd",
			});

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"UnrealEd",
//				"WorkspaceMenuStructure",
			});
	}
}
