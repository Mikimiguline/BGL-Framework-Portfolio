// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "Effects/BGL_CoreEffectsTypes.h"
#include "CoreLibrary/BGL_CoreMovementTypes.h"
#include "InstancedStruct.h"
#include <StructUtils/InstancedStruct.h>
#include "BGL_UAIActionTypes.generated.h"

class UAnimMontage;




// ---------------------------------------------------------
// 0. EVALUATION CONTEXT (data passed from Layer 1 to Layer 2)
// ---------------------------------------------------------
USTRUCT(BlueprintType)
struct FUAIEvaluationContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    AActor* AIOwner = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    AActor* Objective = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    float DistanceToObjective = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    float ZHighDistance = 0.0f;


};


// ---------------------------------------------------------
// 1. SCORER BASE TYPES (Layer 1: mathematical filters)
// ---------------------------------------------------------
USTRUCT(BlueprintType)
struct FUAIActionScorerBase
{
    GENERATED_BODY()

    virtual ~FUAIActionScorerBase() = default;

   

    // Takes the score from the distance curve and returns the modified score
    virtual float CalculateModifier(float BaseScore, const FUAIEvaluationContext& Context) const
    {
        return BaseScore;
    }
};

USTRUCT(BlueprintType)
struct FUAIActionStateScorerBase
{
    GENERATED_BODY()

    virtual ~FUAIActionStateScorerBase() = default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
    FGameplayTag MovementSettingsTag;

   

    // Returns the score of this state for the given evaluation context
    virtual float CalculateStateScore(const FUAIEvaluationContext& Context) const
    {
        return 0.0f;
    }
};


USTRUCT(BlueprintType)
struct FAbortStructure
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    bool AbortAnimation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float PlusAbortAngle = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MinAbortDistance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MaxAbortDistance = 800;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    FRuntimeFloatCurve DistanceOverTimeToAbort;

};

USTRUCT(BlueprintType)
struct FAnimStructure
{
    GENERATED_BODY()

    // --- CORE RULES ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    FRuntimeFloatCurve DistanceCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float IdealDistance = 150;

    // true (the usual case, all regular attacks): Ideal Distance is the minimum distance required to start the attack.
    // If farther away the AI closes in to it; if already closer it attacks from where it stands, without backing off.
    // false: exact positioning. The AI moves to Ideal Distance (+/- Ideal Distance Tolerance) even if it has to back off.
    // Only for attacks that must start at a specific distance (e.g. a root-motion charge that Motion Warping cannot correct).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    bool bSetIdealMinimumDistance = true;

    // Margin (cm) around Ideal Distance.
    // Exact positioning (Set Ideal Minimum Distance unchecked): attacks when between Ideal - margin and Ideal + margin; outside that it repositions. With root motion, use what Motion Warping can correct.
    // Ranged Positioning: looks for a free slot in the rings between Ideal - margin and Ideal (or up to Ideal + margin with exact positioning).
    // Minimum distance without Ranged Positioning: unused.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules", meta = (ClampMin = "0.0"))
    float IdealDistanceTolerance = 50.0f;

    // Ranged attacks (bow, spell, throw...). The AI takes a slot in the same rings used by those waiting in the wait Move To (right-hand branch of the tree), without taking anyone's slot.
    // If its ring is full it waits for a free slot (or fires from where it stands if the attack Move To has 'Ranged Attack in Place if Slots Full' checked).
    // Leave unchecked for melee attacks, including long charges: those approach the target directly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    bool bRangedPositioning = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Warping")
    float MotionWarping_MaxTranslation = 150;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Warping")
    float MotionWarping_RotationOffsetYaw = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MinRepeatTimesAnimation = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MaxRepeatTimesAnimation = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MaxAngle = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    bool bApplyCooldownOnInterrupt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    FAbortStructure AbortConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MinCooldownTime = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    float MaxCooldownTime = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules")
    bool OverrideActionTokenTag = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules", meta = (EditCondition = "OverrideActionTokenTag", EditConditionHides, Categories = "Token"))
    FGameplayTag TokenActionTagOverride;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules", meta = (Categories = "State.Cooldown"))
    FGameplayTag CooldownTag;
    // Cooldown tags of other animations that this action also puts on cooldown
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules", meta = (Categories = "State.Cooldown"))
    FGameplayTagContainer OtherAnimTagsToApplyCooldown;

    // Tag applied to the character while this action is being performed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules | Interruption", meta = (EditCondition = Categories = "OverrideActionTokenTag", EditConditionHides, Categories = "Combat.RangedCombat.Attacking"))
    FGameplayTag ActionInProgressTag;

    //What movement settings will be set when finalizing this action
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules | Movement")
    FGameplayTag ActionMovementToApply;

    //if you want to create new movement settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules | Movement" )
    bool OverrideMovementSettings = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules | Movement", meta = (EditCondition = "OverrideMovementSettings", EditConditionHides))
    FBGL_MovementProfile CustomMovingSettings;

    // if true Interrupt GeneralTags wont work for this animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules | Interruption")
    bool OverrideGeneralInterruptMeTags = false;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules | Interruption", meta = (EditCondition = "OverrideGeneralInterruptMeTags", EditConditionHides, Categories = "Combat.RangedCombat.Attacking"))
    FGameplayTagContainer TagsCanInterruptMe;


    // FActionBase is not reachable from here, so the data asset copies its general interrupt tags into this field in PostLoad
    FGameplayTagContainer GeneralTagsCanInterruptMe;

    /** Montage time (seconds) at which the attack starts. 0 plays it from the beginning. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules", meta = (ClampMin = "0.0", DisplayName = "Begin Attack"))
    float BeginAttack = 0.f;

    /** Seconds trimmed from the end of the montage. 0 plays it to the end. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Core Rules", meta = (ClampMin = "0.0", DisplayName = "End Anim"))
    float EndAnim = 0.f;

};

USTRUCT(BlueprintType)
struct FUAIComboStep
{
    GENERATED_BODY()

    // Optional: if null, reuses the main Animation of the action (section mode).
    // If a montage is assigned, the step switches animation (nested montage mode).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Step")
    TObjectPtr<UAnimMontage> OverrideAnimation = nullptr;

    // Montage sections for this step, in order. Empty plays the whole montage.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Step", meta = (DisplayAfter = "OverrideAnimation", DisplayName = "Montage Play"))
    TArray<FName> MontageSections;

    // Chase distance for this specific hit of the combo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Step")
    float MotionWarping_MaxTranslation = 60;

    // If empty, reuses the ExecutionEffects of the opening hit. Otherwise uses these (e.g. a finisher with an area effect)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Step")
    TArray<FActionEffect_EffectSpawnConfiguration> OverrideEffects;

};

// ---------------------------------------------------------
// 3. MAIN CONTAINER (the one stored in the data asset)
// ---------------------------------------------------------
USTRUCT(BlueprintType)
struct FUAIActionConfig
{
    GENERATED_BODY()

    // --- VISUAL ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Visual")
    class UAnimMontage* Animation = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Config")
    FAnimStructure AnimConfig;

    // --- LAYER 1 (extra filters) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/UniversalAI.UAIActionScorerBase"), Category = "3. Conditionals")
    TArray<FInstancedStruct> ExtraScorers;

    // --- LAYER 2 (execution) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/UniversalAI.BGL_CoreEffectsBase"), Category = "4. Execution")
    TArray<FActionEffect_EffectSpawnConfiguration> ExecutionEffects;

    /** Montage sections, in order. Empty plays the whole montage. Several are chained one after another. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Visual", meta = (DisplayAfter = "Animation", DisplayName = "Montage Play"))
    TArray<FName> MontageSections;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Visual", meta = (DisplayAfter = "MontageSections"))
    TArray<FUAIComboStep> ComboSequence;
};



USTRUCT(BlueprintType)
struct FActionBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Config", meta = (Categories = "GameFeatures.AISystem.Combat"))
    FGameplayTag ActionGameplayTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Token Tag", meta = (Categories = "Token"))
    FGameplayTag TokenActionTag;

    UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/UniversalAI.UAIActionStateScorerBase"), Category = "2. Selection Criteria")
    TArray<FInstancedStruct> StateScorers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules | Interruption", meta = (Categories = "Reactions"))
    FGameplayTagContainer GeneralTagsCanInterruptMe;

    // Wait point (the wait Move To, right-hand branch of the tree) while this state is winning. The Combat Service rolls a distance between Min and Max
    // once when the state changes and passes it to the Move To through IBGL_AIMemoryInterface. WaitDistanceMax at 0 = no wait point (regular close ring).
    // With WaitDistanceMax <= the Ideal Distance of its attacks the AI attacks without moving; if it waits farther away it advances to Ideal Distance.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait Positioning", meta = (ClampMin = "0.0"))
    float WaitDistanceMin = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait Positioning", meta = (ClampMin = "0.0"))
    float WaitDistanceMax = 0.0f;

    // true (e.g. an archer): never leaves its band. It always holds a slot in the ring at its wait distance, with or without a token,
    // and backs off to its ring if the target closes in.
    // false: reaches its wait point (or stops earlier if already closer) and tries for the token. If that fails the state is exhausted:
    // another valid state at that distance wins or, if there is none, the AI falls back to the close wait.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait Positioning", meta = (EditCondition = "WaitDistanceMax > 0.0"))
    bool bHoldWaitPoint = false;

    bool GetWaitDistanceRange(float& OutMin, float& OutMax) const
    {
        if (WaitDistanceMax <= 0.0f)
        {
            return false;
        }
        OutMin = FMath::Min(WaitDistanceMin, WaitDistanceMax);
        OutMax = FMath::Max(WaitDistanceMin, WaitDistanceMax);
        return true;
    }

    // Micro layer: the actions available while this state is active
    UPROPERTY(EditAnywhere, Category = "Micro | Actions")
    TArray<FUAIActionConfig> ActionList;

};
