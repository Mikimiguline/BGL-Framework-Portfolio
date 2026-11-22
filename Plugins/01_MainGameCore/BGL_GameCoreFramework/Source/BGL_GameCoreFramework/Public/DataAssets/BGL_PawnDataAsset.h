// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TagLibrary/BGL_GlobalTags.h"
#include "BGL_PawnDataAssetExtension.h"
#include "BGL_PawnDataAsset.generated.h"

class AAIController;

UCLASS()
class BGL_GAMECOREFRAMEWORK_API UBGL_PawnDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Extensions")
	TArray<TObjectPtr<UBGL_PawnDataAssetExtension>> DataAssetExtension;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	FGameplayTagContainer GameFeaturesGameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (Categories = "State"))
	FGameplayTag InitialState = Tag_State_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	int AITeamID = 255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	int PlayerOnPossesTeamID = 0;

	// --- Possession ---

	// The player may take control of this body (it needs the player extensions: actions, input, camera).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Possession")
	bool bCanBePossessedByPlayer = false;

	// How far the player can reach to possess another body while controlling this one (soft target search).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Possession", meta = (ClampMin = "0.0", Units = "cm"))
	float PossessRange = 1500.f;

	// AI controller this body gets when the player leaves it and it has no AI controller of its own to return to.
	// Empty = the body stays idle without controller.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Possession")
	TSubclassOf<AAIController> AIControllerClassOnRelease;

};
