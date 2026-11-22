// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class BGL_GameCoreFramework : ModuleRules
{
	public BGL_GameCoreFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "AIModule",
                "GameplayTags",
                "GameFeatures",
                "ModularGameplay",
                "MotionWarping",
                "AnimGraphRuntime",
                "EnhancedInput",
                "AnimGraphRuntime",
                "DeveloperSettings"
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
