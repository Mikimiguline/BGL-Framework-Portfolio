// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "UBGL_CoreModularManagerSubsystem.generated.h"

struct FModuleStruct;
USTRUCT(BlueprintType)
struct FActiveModuleData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UObject> ModuleInstance;

	// Timer that drives this module
	FTimerHandle ModuleTimerHandle;


};
UCLASS()
class BGL_GAMECOREFRAMEWORK_API UUBGL_CoreModularManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnWorldBeginPlay(UWorld& InWorld)override;

	void ActivateGlobalModules(TMap<FGameplayTag, UDataAsset*> InTmap, UObject* EntitySubsystem);

	void InjectLocalModule(const FModuleStruct& NewModule, const FGameplayTag& ModuleTag, UObject* EntitySubsystem);

	void InitializeModuleSubsystems(UObject* inModule, const FModuleStruct& ModuleStruct, UObject* EntitySubsystem);
	
	void GetWorldSubsystemsTmap(UWorld& InWorld, TMap<FGameplayTag, UDataAsset*> InTmap);

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UObject>> ModuleExtension;

};
