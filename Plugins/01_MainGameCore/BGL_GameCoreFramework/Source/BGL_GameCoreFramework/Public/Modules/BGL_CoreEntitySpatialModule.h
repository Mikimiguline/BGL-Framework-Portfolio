// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/BGL_ModuleInterface.h"
#include "Subsystems/UBGL_CoreModularManagerSubsystem.h"
#include "BGL_CoreEntitySpatialModule.generated.h"

class IBGL_EntityRegisterInterface;
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class BGL_GAMECOREFRAMEWORK_API UBGL_CoreEntitySpatialModule : public UObject, public IBGL_ModuleInterface
{
	GENERATED_BODY()
	
	virtual void InitializeModule() override;

	void InitializeModule(UUBGL_CoreModularManagerSubsystem* AUSubSystem);
	virtual void RunModule(IBGL_EntityRegisterInterface* SubsystemInterface, const FGameplayTagContainer& TargetActorTags, float DeltaTime = 0.0f) override;

	void CalculateDistance(const AActor* PlayerActor, const TSet<TObjectPtr<AActor>>& TSetActors, TMap<TObjectPtr<AActor>, float>& EntityManagerDistances, TMap<TObjectPtr<AActor>, float>& EntityManagerViewDots);
	void ActivateActor(const AActor* ActorActivate, float Distance);


};
