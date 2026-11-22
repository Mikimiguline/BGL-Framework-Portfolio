// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Actions/Types/BGL_UAIActionTypes.h"
#include "Evaluators/BGL_EvaluatorCombatBase.h"
#include "DataAssets/BGL_PawnDataAssetExtension.h"
#include "BGL_AICombatDataAssetExtension.generated.h"



USTRUCT(BlueprintType)
struct FBaseCombatConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage = 15.0f;


};

// Combat states of the pawn, each with its own scorers and action list
USTRUCT(BlueprintType)
struct FCombatConfig : public FBaseCombatConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat States")
	TArray<FActionBase> CombatStates;

};

UCLASS()
class BGLAICOMBATACTIONGFRUNTIME_API UBGL_AICombatDataAssetExtension : public UBGL_PawnDataAssetExtension
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats Base", meta = (ShowOnlyInnerProperties))
	FCombatConfig CombatConfig;

	virtual void PostLoad() override;

};
