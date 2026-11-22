// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction.h"
#include "Engine/World.h"
#include "BGL_GFAddByTagReactive.generated.h"
class IGameplayTagAssetInterface;
class UBehaviorTree;

USTRUCT()
struct FComponentTagRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UActorComponent> ComponentClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> ActorToInjectClass;

	UPROPERTY(EditAnywhere)
	FGameplayTag RequiredCapabilityTag;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UBehaviorTree> BehaviorTreeAsset;

};
USTRUCT()
struct FModuleTagRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Subsystem Injection | Local Modules")
	TArray<TSubclassOf<UObject>> LocalModulesToInject;

	UPROPERTY(EditAnywhere, Category = "Subsystem Injection")
	TSubclassOf<USubsystem> TargetManagerClass;
};

UCLASS()
class BGL_GAMECOREFRAMEWORK_API UBGL_GFAddByTagReactive : public UGameFeatureAction
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Injection")
	TArray<FComponentTagRequest> TagInjectionGamefeaturesRules;

	UPROPERTY(EditAnywhere, Category = "Injection")
	TArray<FModuleTagRequest> TagInjectionComponentRules;

	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

private:
	void HandleActorExtension(AActor* Actor, FName EventName);
	void CheckAndApplyInjection(AActor* Actor, const FComponentTagRequest& TagRule);
	void InjectBehaviourTree(AActor* TreeOwner, const FComponentTagRequest& TagRule, IGameplayTagAssetInterface* TagInterface);

	// Hooks into the game instance without depending on Lyra
	void AddToGameInstance(UGameInstance* GameInstance);
	void OnGameInstanceStart(UGameInstance* GameInstance);
	// In Standalone the engine loads the map (spawning actors, PossessedBy...) BEFORE firing OnStartGameInstance, so their
	// ExtensionAdded events were lost. Each game world hooks its GameInstance on initialization, before its actors are created
	void OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

	void InjectSubsystemModules(UGameInstance* GameInstance);

	// Kept so the delegates can be removed on deactivation (e.g. pressing Stop in the editor)
	FDelegateHandle GameInstanceStartHandle;
	FDelegateHandle WorldInitHandle;
	// GameInstances that already have the handlers (avoids registering them twice: in PIE both delegates fire)
	TArray<TWeakObjectPtr<UGameInstance>> RegisteredGameInstances;
	TArray<TSharedPtr<FComponentRequestHandle>> ExtensionHandlers;
};
