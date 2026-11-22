// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/BGL_PawnDataAssetExtension.h"
#include "BGL_AIPerceptionDataExtension.generated.h"

class UAISenseConfig;
class UAISense;

USTRUCT(BlueprintType)
struct FBGL_AIPerceptionConfig {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
	bool bIsDynamic = true;

	UPROPERTY(EditAnywhere, Instanced, Category = "AI Perception")
	TArray<TObjectPtr<UAISenseConfig>> SensesConfig;

	UPROPERTY(EditAnywhere, Category = "AI Perception")
	TSubclassOf<UAISense> DominantSense;

};

UCLASS()
class BGLAIPERCEPTIONGFRUNTIME_API UBGL_AIPerceptionDataExtension : public UBGL_PawnDataAssetExtension
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category ="AI Perception", meta = (ShowOnlyInnerProperties))
	FBGL_AIPerceptionConfig PerceptionConfig;


};
