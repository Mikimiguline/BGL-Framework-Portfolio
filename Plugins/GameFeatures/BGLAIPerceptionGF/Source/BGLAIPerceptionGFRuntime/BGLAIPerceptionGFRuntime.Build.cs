// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

using UnrealBuildTool;

public class BGLAIPerceptionGFRuntime : ModuleRules
{
	public BGLAIPerceptionGFRuntime(ReadOnlyTargetRules Target) : base(Target)
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
				"UniversalAI",
                "Engine",
                "AIModule",
                "GameplayTags",
                "ModularGameplay",
				"BGL_GameCoreFramework"

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
