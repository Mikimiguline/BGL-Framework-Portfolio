// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagAssetInterface.h"
#include "BGL_CorePerformanceTypes.generated.h"

UENUM(BlueprintType)
enum class EBGL_OptimizationTier : uint8
{
	LOD_0_MaxQuality  UMETA(DisplayName = "Tier 0: Max Quality (Near)"),
	LOD_1_Reduced     UMETA(DisplayName = "Tier 1: Reduced (Mid)"),
	LOD_2_Background  UMETA(DisplayName = "Tier 2: Background (Far)"),
	LOD_3_OffCamera   UMETA(DisplayName = "Tier 3: Off Camera"),
	LOD_4_Hibernation UMETA(DisplayName = "Tier 4: Hibernation (Dead)"),
	// Always evaluated before the other tiers, because it overlaps with the distance-based ones
	LOD_5_Stopped     UMETA(DisplayName = "Tier 5: Stopped (Idle, Far)")
};

UENUM(BlueprintType)
enum class EBGL_PerceptionTier : uint8
{
	AlwaysOn UMETA(DisplayName = "Always On"),
	Scheduled UMETA(DisplayName = "Scheduled"),
	Off UMETA(DisplayName = "Off")
};


USTRUCT(BlueprintType)
struct FBGL_LODEvaluationContext
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AActor* ViewTarget = nullptr; // The player or the camera

	UPROPERTY()
	AActor* TargetActor = nullptr; // The entity being evaluated (door, NPC, pickup)

	UPROPERTY()
	float DistanceSq = 0.0f; // Squared distance, precomputed by the spatial registry

	UPROPERTY()
	float ViewDot = -1.f;
};




// Polymorphic base for LOD conditions
USTRUCT(BlueprintType)
struct FBGL_LODConditionBase
{
	GENERATED_BODY()
	virtual ~FBGL_LODConditionBase() = default;

	// Returns true when the condition is met.
	virtual bool EvaluateCondition(const FBGL_LODEvaluationContext& Context) const { return false; }
};

// Condition: distance range
USTRUCT(BlueprintType)
struct FBGL_LODCondition_Distance : public FBGL_LODConditionBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float MinDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float MaxDistance = 10000.0f;

	virtual bool EvaluateCondition(const FBGL_LODEvaluationContext& Context) const override
	{
		// Compare squared distances to avoid a square root
		return Context.DistanceSq >= (MinDistance * MinDistance) &&
			Context.DistanceSq <= (MaxDistance * MaxDistance);
	}
};

// Condition: recently rendered on screen
USTRUCT(BlueprintType)
struct FBGL_LODCondition_WasRendered : public FBGL_LODConditionBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bMustBeVisible = false; // true: must be visible. false: must not be visible

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float ToleranceSeconds = 0.2f;

	virtual bool EvaluateCondition(const FBGL_LODEvaluationContext& Context) const override
	{
		if (!Context.TargetActor) return false;
		bool bIsRendered = Context.TargetActor->WasRecentlyRendered(ToleranceSeconds);
		return bIsRendered == bMustBeVisible;
	}
};

// Condition: gameplay tag on the view target (e.g. a special vision mode)
USTRUCT(BlueprintType)
struct FBGL_LODCondition_PlayerHasTag : public FBGL_LODConditionBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTag RequiredTag;

	virtual bool EvaluateCondition(const FBGL_LODEvaluationContext& Context) const override
	{
		// Requires the view target to implement IGameplayTagAssetInterface
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Context.ViewTarget))
		{
			return TagInterface->HasMatchingGameplayTag(RequiredTag);
		}
		return false;
	}
	
};

USTRUCT(BlueprintType)
struct FBGL_LODCondition_ViewAngle : public FBGL_LODConditionBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float HalfAngleDegrees = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bMustBeInside = true;

	virtual bool EvaluateCondition(const FBGL_LODEvaluationContext& Context) const override
	{
		const float Threshold = FMath::Cos(FMath::DegreesToRadians(HalfAngleDegrees));
		const bool bInside = Context.ViewDot >= Threshold;
		return bInside == bMustBeInside;
	}
};

// Condition: stopped or moving
USTRUCT(BlueprintType)
struct FBGL_LODCondition_Movement : public FBGL_LODConditionBase
{
	GENERATED_BODY()

	// true: met when stopped. false: met when moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bMustBeStopped = true;

	// Speed (cm/s) below which the actor counts as stopped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0.0"))
	float SpeedThreshold = 10.0f;

	virtual bool EvaluateCondition(const FBGL_LODEvaluationContext& Context) const override
	{
		if (!Context.TargetActor) return false;
		const bool bStopped = Context.TargetActor->GetVelocity().SizeSquared2D() <= FMath::Square(SpeedThreshold);
		return bStopped == bMustBeStopped;
	}
};

USTRUCT(BlueprintType)
struct FBGL_LODPayloadBase
{
	GENERATED_BODY()
	virtual ~FBGL_LODPayloadBase() = default;

	// Applies this payload's effect to the entity
	virtual void ApplyPayload(AActor* TargetActor) const {}
};

USTRUCT(BlueprintType)
struct FBGL_LODPayload_General : public FBGL_LODPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bEnableTick = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bSetActorHidden = false;

	virtual void ApplyPayload(AActor* TargetActor) const override
	{
		if (!TargetActor) return;
		TargetActor->SetActorTickEnabled(bEnableTick);
		TargetActor->SetActorHiddenInGame(bSetActorHidden);
	}
};

USTRUCT(BlueprintType)
struct FBGL_LODPayload_ToggleLights : public FBGL_LODPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lights")
	bool bEnableLights = false;

	virtual void ApplyPayload(AActor* TargetActor) const override;

};

USTRUCT(BlueprintType)
struct FBGL_LODPayload_PlacementTick : public FBGL_LODPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement", meta = (EditCondition = "!bUsePerTick", EditConditionHides))
	float TickInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	int32 SimulationIterations = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	bool bEvaluatePose = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement", meta = (DisplayName = "Use Per Tick"))
	bool bUsePerTick = false;

	virtual void ApplyPayload(AActor* TargetActor) const override;
};

USTRUCT(BlueprintType, meta = (DisplayName = "BGL_LODPayload_AIPerceptionActivation"))
struct FBGL_LODPayload_AIPerceptionActivation : public FBGL_LODPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (DisplayName = "Always On"))
	bool bAlwaysOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate By Time Period", meta = (EditCondition = "!bAlwaysOn && !bDisableCompletely", EditConditionHides, ClampMin = "0.05"))
	float Period = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate By Time Period", meta = (EditCondition = "!bAlwaysOn && !bDisableCompletely", EditConditionHides, ClampMin = "0.0"))
	float ActiveWindow = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (DisplayName = "Disable Completely", EditCondition = "!bAlwaysOn", EditConditionHides))
	bool bDisableCompletely = false;
};

USTRUCT(BlueprintType)
struct FBGL_LODPayload_PerceptionRay : public FBGL_LODPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FGameplayTag TargetSocketTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (ClampMin = "0.0"))
	float Period = 2.f;
};

USTRUCT(BlueprintType)
struct FBGL_LODTierConfig
{
	GENERATED_BODY()

	// Polymorphic list of conditions; the designer picks which ones to add from a dropdown.
	// BaseStruct must name the module that declares the base struct.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules", meta = (BaseStruct = "/Script/BGL_GameCoreFramework.BGL_LODConditionBase"))
	TArray<FInstancedStruct> ActivationConditions;

	// Polymorphic list of payloads applied when the conditions are met
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payloads", meta = (BaseStruct = "/Script/BGL_GameCoreFramework.BGL_LODPayloadBase"))
	TArray<FInstancedStruct> PayloadsToApply;
};
