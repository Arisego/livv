// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;


public class Cubism : ModuleRules
{
	public Cubism(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"Cubism/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Cubism/Private",
                "ThirdParty/CubismLibrary/include",
                "Cubism/Private/FrameWork/include"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
                "CoreUObject",
                "Projects",
                "RenderCore",
                "SlateCore"
            }
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        var ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/"));
        var CubismLibPath = ThirdPartyPath + "CubismLibrary/lib/";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicLibraryPaths.Add(CubismLibPath + "windows/x86_64");

            PublicAdditionalLibraries.Add(String.Format("Live2DCubismCore.lib"));
        }

    }
}
