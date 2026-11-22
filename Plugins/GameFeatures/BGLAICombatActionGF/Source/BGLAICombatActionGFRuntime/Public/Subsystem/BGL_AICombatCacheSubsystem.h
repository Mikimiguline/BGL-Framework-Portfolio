// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interfaces/BGL_RegistryReceiverInterface.h"
#include "Actions/Types/BGL_UAIActionTypes.h"
#include "Interfaces/BGL_AIMemoryInterface.h"
#include "BGL_AICombatCacheSubsystem.generated.h"

class UBGL_AICombatDataAssetExtension;

UCLASS()
class BGLAICOMBATACTIONGFRUNTIME_API UBGL_AICombatCacheSubsystem : public UWorldSubsystem, public IBGL_RegistryReceiverInterface
{
	GENERATED_BODY()


	TMap<UBGL_AICombatDataAssetExtension*, TArray<FActionBase>> SharedCombatCache;
	TMap<APawn*, UBGL_AICombatDataAssetExtension*> FastPawnToAssetMap;
	TMap<APawn*, TArray<FActionBase>> IndividualCombatCache;
	TSet<FGameplayTag> RegisteredTags;
	virtual void OnActorRegistered(AActor* RegisteredActor) override;
	virtual void OnActorUnregistered(AActor* RegisteredActor) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:

	const TArray<FUAIActionConfig>* GetActionListCache(APawn* AIOwner,FGameplayTag GMTag);
	const FActionBase* GetCombatStateConfig(APawn* AIOwner, FGameplayTag GMTag);
	const TArray<FActionBase>& GetAllCombatStates(APawn* AIOwner) const;

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

};
