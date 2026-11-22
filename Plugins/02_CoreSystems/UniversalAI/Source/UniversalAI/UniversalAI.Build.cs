// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

using UnrealBuildTool;

public class UniversalAI : ModuleRules
{
	public UniversalAI(ReadOnlyTargetRules Target) : base(Target)
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
                "NavigationSystem", 
				"AIModule",
                "GameplayTags",
				"MotionWarping", 
				"GameFeatures",
				"ModularGameplay", "BGL_GameCoreFramework","DeveloperSettings"
				
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
	
				"Slate",
				"SlateCore",
				"GameplayTasks",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
