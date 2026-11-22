// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DataAssets/BGL_PawnDataAssetExtension.h"
#include "BGL_HealthDataAssetExtension.generated.h"

USTRUCT(BlueprintType)
struct FBGL_HealthConfig
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float InitHealth = 100 ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100;

	// Logs the current health every time it changes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health Debug")
	bool DebugHealth = false;
};


UCLASS()
class BGLHEALTHSYSTEMGFRUNTIME_API UBGL_HealthDataAssetExtension : public UBGL_PawnDataAssetExtension
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Basic", meta = (ShowOnlyInnerProperties))
	FBGL_HealthConfig HealthConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Player", meta = (ShowOnlyInnerProperties))
	FBGL_HealthConfig PlayerHealthConfig;

};
