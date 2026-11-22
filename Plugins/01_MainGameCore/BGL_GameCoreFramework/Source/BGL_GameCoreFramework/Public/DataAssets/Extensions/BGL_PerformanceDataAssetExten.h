// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/BGL_PawnDataAssetExtension.h"
#include "CoreLibrary/BGL_CorePerformanceTypes.h"
#include "BGL_PerformanceDataAssetExten.generated.h"

class UBGL_PerformanceDataAsset;

UCLASS(DefaultToInstanced, EditInlineNew, BlueprintType)
class BGL_GAMECOREFRAMEWORK_API UBGL_PerformanceDataAssetExten : public UBGL_PawnDataAssetExtension
{
	GENERATED_BODY()

public:

	// Enabled: uses the configuration defined here. Disabled: uses the shared PerformanceDataAsset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization")
	bool bUseCustomPerformance = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization", meta = (EditCondition = "!bUseCustomPerformance", EditConditionHides))
	TObjectPtr<UBGL_PerformanceDataAsset> PerformanceDataAsset;

	// --- Custom configuration (only applied with bUseCustomPerformance). Do not read directly: use the getters ---
	// Always visible and editable, even with bUseCustomPerformance disabled, so the configuration can be copied to a PerformanceDataAsset

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization|Custom")
	TMap<EBGL_OptimizationTier, FBGL_LODTierConfig> LODTiers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization|Custom|Perception")
	TMap<EBGL_PerceptionTier, FBGL_LODTierConfig> PerceptionTiers;

	// Seconds the corpse stays visible after the death montage ends. It is then hidden and stops costing GPU time. 0 = never hidden
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization|Custom|Death", meta = (ClampMin = "0.0"))
	float CorpseHideDelay = 0.0f;

	// --- Resolved access: custom or shared asset depending on bUseCustomPerformance ---

	// False when not custom and no PerformanceDataAsset is assigned: the entity has no performance configuration
	bool HasPerformanceData() const;

	const TMap<EBGL_OptimizationTier, FBGL_LODTierConfig>& GetLODTiers() const;
	const TMap<EBGL_PerceptionTier, FBGL_LODTierConfig>& GetPerceptionTiers() const;
	float GetCorpseHideDelay() const;
};
