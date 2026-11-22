// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameplayTagContainer.h"
#include "BGL_AIPerceptionComponent.generated.h"

class IGameplayTagAssetInterface;
class AUAI_AIController;
class UBrainComponent;
class UBehaviorTreeComponent;

UCLASS()
class BGLAIPERCEPTIONGFRUNTIME_API UBGL_AIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// Switches perception and the behavior tree off/on when the NPC dies, is pooled or has its senses blocked
	void HandleOnCharTagUpdate(AActor*, const FGameplayTagContainer& TagContainer, bool bAdded);

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	
	UFUNCTION(BlueprintCallable, Category = "AIPerception")
	void AIPerceptionActivation(bool bActivate);


protected:
	virtual void HandleExpiredStimulus(FAIStimulus& StimulusStore) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTreeComponent>CachedBehaviorTree;
	UPROPERTY(Transient)
	TObjectPtr<UBlackboardComponent> BlackboardComp;
	UPROPERTY(Transient)
	TObjectPtr<AUAI_AIController>MyController;

	void LoseCurrentObjective();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Someone changed team (possession): rebuild the team-filtered sight queries and drop the objective if it became an ally
	void HandleEntityTeamChanged(AActor* ChangedActor);
	FDelegateHandle EntityTeamChangedHandle;

	// True while Status.Senses.Disabled / Status.Senses.Lost keep the configured senses off.
	bool bSensesBlockedByTags = false;

};
