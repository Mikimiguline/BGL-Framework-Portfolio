// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

using UnrealBuildTool;

public class BGL_PlayerFeaturesGFRuntime : ModuleRules
{
	public BGL_PlayerFeaturesGFRuntime(ReadOnlyTargetRules Target) : base(Target)
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
                "EnhancedInput",
                "ModularGameplay",
                "GameplayTags", "BGL_GameCoreFramework","DeveloperSettings"
			}
			);
                


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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
