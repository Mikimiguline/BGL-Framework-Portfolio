// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Actions/Types/BGL_UAIActionTypes.h"
#include "BGL_UAITask_ExecuteAction.generated.h"
class IBGL_AIMemoryInterface;
class IBGL_CustomTagAssetInterface;
class IGameplayTagAssetInterface;
class IBGL_PawnAssetDataInterface;
class IBGL_CoreEffectsInterface;
class UBGL_ActionComponent;
class IBGL_TokenProviderModuleInterface;
struct FExecuteBaseTaskMemory
{
    TWeakInterfacePtr<IBGL_AIMemoryInterface> CachedMemoryInterface;
    const FUAIActionConfig* CurrentAction = nullptr;
    TWeakInterfacePtr<IBGL_CustomTagAssetInterface> CachedCustomTagInterface;
    TWeakInterfacePtr <IGameplayTagAssetInterface> CachedTagInterface;
    TWeakInterfacePtr<IBGL_PawnAssetDataInterface> CachedPawnInterface;
    TWeakInterfacePtr<IBGL_CoreEffectsInterface> CachedEffectsInterface;
    TWeakInterfacePtr<IBGL_TokenProviderModuleInterface> TokenProviderInterface;
    FGameplayTag CachedTokenTag;
    UBGL_ActionComponent* ActionComponent;
    FDelegateHandle OnActionFinishedDelegate;
    AActor* CachedObjective;
    float IdealDistance;
    bool bTaskAlreadyFinished = false;
    float MotionWarpingMaxTranslation;
    int32 CurrentRepetition = 0;
    int32 TargetRepetitions = 0;
    FGameplayTag InProgressStateTag;
    FGameplayTag ActiveCooldownTag;
    FName ActionName;
    const FUAIActionConfig* CachedBestAction;
    float ActiveCooldownDuration = 0.0f;
    bool bActiveApplyCooldownOnInterrupt = false;
    const FUAIActionConfig* CachedActionConfig;
    float CachedAttackDistance;
};

UCLASS()
class UNIVERSALAI_API UBGL_UAITask_ExecuteAction : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
private:

    UBGL_UAITask_ExecuteAction();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector EvaluateTagToken;

    // Interval in seconds between abort checks (distance, angle, interruption) while the attack plays. 0 = every frame
    UPROPERTY(EditAnywhere, Category = "BGL|Tick", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AbortCheckTickInterval = 0.1f;

    // Called when the behavior tree aborts this task: stops the action and releases its tokens
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual uint16 GetInstanceMemorySize() const override
    {
        return sizeof(FExecuteBaseTaskMemory);
    }
    
public:

    // Bound to the action component's OnActionFinished: fires when the montage ends or is interrupted
    void OnActionMontageFinished(bool bInterrupted, UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory);
};

