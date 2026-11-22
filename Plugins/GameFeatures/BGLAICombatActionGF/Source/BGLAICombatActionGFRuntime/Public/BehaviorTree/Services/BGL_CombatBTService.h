// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Actions/Types/BGL_UAIActionTypes.h"
#include "BGL_CombatBTService.generated.h"
class UBGL_PawnDataAsset;
class UBGL_AICombatDataAssetExtension;
class IBGL_AIMemoryInterface;

struct FCombatServiceMemory
{
	TArray<FActionBase> ActionBase;
	const UObject* CachedRawObjective = nullptr;
	AActor* CachedObjective = nullptr;
	// State the current wait distance was rolled for (rolled again only when the state changes)
	FGameplayTag WaitDistanceStateTag;
	// Movement profile of the state that is already applied (re-applied only when it changes)
	FGameplayTag AppliedMovementTag;
};


UCLASS()
class BGLAICOMBATACTIONGFRUNTIME_API UBGL_CombatBTService : public UBTService_BlackboardBase
{
	GENERATED_BODY()

	UBGL_CombatBTService();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector GameplayTagResult;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActionsToken;

	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (Categories = "Combat"))
	FGameplayTag SkipMovementProfileWhileTag;

	// A state without Hold Wait Point that reached its wait point and failed the token is discarded while another state is valid.
	// It competes again after these seconds or as soon as it stops being valid (leaves its range). 0 = only when it stops being valid.
	// It is never cleared because another state wins (that would make the AI oscillate between the two).
	UPROPERTY(EditAnywhere, Category = "Wait Positioning", meta = (ClampMin = "0.0"))
	float ExhaustedWaitStateExpireSeconds = 8.0f;

protected:

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Runs once when the branch is left */
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual uint16 GetInstanceMemorySize() const override;

	// FCombatServiceMemory holds a TArray: without these it is never constructed or destroyed properly
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;


};
