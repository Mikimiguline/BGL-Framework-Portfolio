// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Interfaces/BGL_ModuleInterface.h"
#include "CoreLibrary/BGL_CorePerformanceTypes.h"
#include "BGL_EntityOptimizationModule.generated.h"

class IBGL_EntityRegisterInterface;

struct FBGL_PerceptionClockEntry
{
	double CycleOrigin = 0.0;
	double NextWorldTime = 0.0;
	float Period = 2.f;
	float ActiveWindow = 0.2f;
	bool bActivePhase = false;
	bool bUseRay = false;
	FGameplayTag TargetSocketTag;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class BGL_GAMECOREFRAMEWORK_API UBGL_EntityOptimizationModule : public UObject, public IBGL_ModuleInterface
{
	GENERATED_BODY()
	
public:

	virtual void InitializeModule()override;
	virtual void RunModule(IBGL_EntityRegisterInterface* RegistryInterface, const FGameplayTagContainer& TargetTags, float DeltaTime = 0.0f) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (ClampMin = "1"))
	int32 MaxPhaseChangesPerPass = 8;

	// Maximum number of visible corpses in the world. Beyond it, the oldest one that is off camera gets hidden. 0 = no limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization|Death", meta = (ClampMin = "0"))
	int32 MaxCorpses = 100;

	void RegisterCorpse(AActor* Corpse);

private:
	TMap<TWeakObjectPtr<AActor>, EBGL_PerceptionTier> AppliedPerceptionTiers;
	TMap<TWeakObjectPtr<AActor>, FBGL_PerceptionClockEntry> PerceptionClock;
	TWeakObjectPtr<AActor> PerceptionClockResumeActor;
	// Oldest to newest
	TArray<TWeakObjectPtr<AActor>> Corpses;

	void RunPerception(IBGL_EntityRegisterInterface* RegistryInterface, const FGameplayTagContainer& TargetTags, AActor* ViewTarget);
	static bool DesiredActivePhase(const FBGL_PerceptionClockEntry& Entry, double WorldTime);
	static double ComputeNextWorldTime(const FBGL_PerceptionClockEntry& Entry, double WorldTime, bool bActivePhase);
	static void ReadSchedule(const FBGL_LODTierConfig* Config, float& OutPeriod, float& OutActiveWindow);
	void SetSensesDisabledTag(AActor* Actor, bool bDisabled) const;
	void ApplyPerceptionTier(AActor* Actor, EBGL_PerceptionTier NewTier, const FBGL_LODTierConfig* Config, double WorldTime, AActor* ViewTarget, bool bKeepObjective);
	void AdvanceDueClocks(double WorldTime, AActor* ViewTarget);
	bool HasLineToPlayerSocket(AActor* Npc, AActor* ViewTarget, FGameplayTag SocketTag) const;
	void SetPlayerObjective(AActor* Npc, AActor* ViewTarget, bool bSeen) const;
	void SetSensesLostTag(AActor* Actor, bool bLost) const;
};
