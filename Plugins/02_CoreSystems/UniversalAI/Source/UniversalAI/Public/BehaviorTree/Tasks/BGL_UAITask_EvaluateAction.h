// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actions/Types/BGL_UAIActionTypes.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BGL_UAITask_EvaluateAction.generated.h"


class IBGL_AIMemoryInterface;
class IBGL_CustomTagAssetInterface;
class IGameplayTagAssetInterface;
class UBlackboardComponent;
class UBehaviorTreeComponent;
class IBGL_PawnAssetDataInterface;
class UBGL_UAIManagerWorldSubsystem;
class IBGL_TokenProviderModuleInterface;

USTRUCT()
struct FInterfaces {

   GENERATED_BODY()

   IBGL_CustomTagAssetInterface* CustomTagInterface;
   IGameplayTagAssetInterface* GameplayTagInterface;
   IBGL_AIMemoryInterface* MemoryInterface;
   IBGL_PawnAssetDataInterface* PawnDataInterface;
   TWeakInterfacePtr <IBGL_TokenProviderModuleInterface> TokenProviderInterface;
   APawn* AIPawn;

};


UENUM(BlueprintType)
enum class ETagOverrideContainerInterruptsRule : uint8
{
    CombineInterruptTags       UMETA(DisplayName = "Combine DataAsset and Node Interrupt Tags"),
    UseDataAssetInterruptTags UMETA(DisplayName = "Only DataAsset Interrupt Tags"),
    OverrideDataAssetTags  UMETA(DisplayName = "Override Data Asset Interrupt Tags")
};

UENUM(BlueprintType)
enum class ETagOverrideActionInProgressRule : uint8
{
    UseDataAssetActionInProgressTag UMETA(DisplayName = "Use Only DataAsset Action In Progress Tags"),
    OverrideOnlyDAEmptyInProgressTags  UMETA(DisplayName = "Override Only DataAsset Empty In Progress Tags"),
    OverrideAllDAInProgressTags UMETA(DisplayName = "Override all DataAsset In Progress Tags")
};

UCLASS()
class UNIVERSALAI_API UBGL_UAITask_EvaluateAction : public UBTTask_BlackboardBase

{
    GENERATED_BODY()
private:

    UBGL_UAITask_EvaluateAction();



protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    const FUAIActionConfig* EvaluateBestAction(UBehaviorTreeComponent& OwnerComp, const TArray<FUAIActionConfig>& ActionList, FInterfaces Interfaces, FGameplayTag InGameplayTagAction);

    virtual TArray<FUAIActionConfig> GetActions(APawn* AIOwner) const PURE_VIRTUAL(UBGL_UAITask_ExecuteBase::GetActions, return TArray<FUAIActionConfig>(););


    FGameplayTag GetActionInprogressTagRule(ETagOverrideActionInProgressRule InProgressTagRule, const FUAIActionConfig * inBestAction);

    FGameplayTagContainer GetRulesWeCanInterrupt(ETagOverrideContainerInterruptsRule InInterruptionRule, const FUAIActionConfig * inCheckAction);

    const FUAIActionConfig* SelectBestActionForLoop(bool bCheckNoDistance,
        const TArray<FUAIActionConfig>& ActionList,
        FUAIEvaluationContext& EvaluationContext,
        FInterfaces& Interfaces,
        const FGameplayTag& TokenTag,
        int32& OutBestIndex,
        FGameplayTag& OutInProgressTag,
        FGameplayTagContainer& OutInterruptTags);

   public:

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Distance;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FGameplayTag GameplayTokenTag;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector FinalTagToken;

    UPROPERTY(EditAnywhere, Category = "Token")
    bool ReserveToken = true;

    UPROPERTY(EditAnywhere, Category = "Interruption Rules")
    ETagOverrideActionInProgressRule ActionInProgressRule;

    UPROPERTY(EditAnywhere, Category = "Interruption Rules")
    FGameplayTag ActionInProgressTag;

    const TArray<FGameplayTag>* GMArrayTag;

    UPROPERTY(EditAnywhere, Category = "Interruption Rules")
    FGameplayTag GameplayTagAction;

    // Node-wide interrupt rule (e.g. close combat can interrupt "Drinking" and "Patrolling")
    UPROPERTY(EditAnywhere, Category = "Interruption Rules")
    FGameplayTagContainer NodeTagsCanInterruptMe;

    // How a conflict between the node and the data asset is resolved
    UPROPERTY(EditAnywhere, Category = "Interruption Rules")
    ETagOverrideContainerInterruptsRule ContainerInterruptionMeRule = ETagOverrideContainerInterruptsRule::CombineInterruptTags;

    // Tokens accepted from the Data Asset (read from FinalTagToken key). Falls back to GameplayTokenTag when no match
    UPROPERTY(EditAnywhere, Category = "Token", meta = (Categories = "Token", DisplayName = "Actions Tokens", DisplayAfter = "GameplayTokenTag"))
    FGameplayTagContainer GameplayTokenTags;
	
};
