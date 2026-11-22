// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Types/BGL_UAIActionTypes.h"
#include "BGL_UAIActionScorers.generated.h"

class ACharacter;
// Example state modifier
USTRUCT(BlueprintType)
struct FUAIScorerTargetState : public FUAIActionScorerBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "State")
    FGameplayTag RequiredTag;


    UPROPERTY(EditAnywhere, Category = "State")
    float Multiplier = 2.0f;

    virtual float CalculateModifier(float BaseScore, const FUAIEvaluationContext& Context) const override;
};

USTRUCT(BlueprintType)
struct FBGL_ScorerActionTimeLocked : public FUAIActionScorerBase
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float TimeLimit;

    UPROPERTY(EditAnywhere, Category = "State")
    FGameplayTag GameplayActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    FRuntimeFloatCurve TimeMultiplierCurve;

	virtual float CalculateModifier(float BaseScore, const FUAIEvaluationContext& Context) const override;

};

USTRUCT(BlueprintType)
struct FBGL_ScorerStateByDistance : public FUAIActionStateScorerBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Config")
	float MinDist = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float MaxDist = 600.0f;

	// The wait point is configured under "Wait Positioning" on the state (FActionBase)

    virtual float CalculateStateScore(const FUAIEvaluationContext& Context) const override;

};