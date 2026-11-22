// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Interfaces/BGL_ModuleInterface.h"
#include "CoreLibrary/BGL_TokenTypes.h"
#include "Interfaces/BGL_TokenProviderModuleInterface.h"
#include "BGL_CoreTokenModule.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class BGL_GAMECOREFRAMEWORK_API UBGL_CoreTokenModule : public UObject, public IBGL_ModuleInterface, public IBGL_TokenProviderModuleInterface
{
	GENERATED_BODY()

public:

	virtual void InitializeModule()override;
	virtual void RunModule(IBGL_EntityRegisterInterface* CensusInterface, const FGameplayTagContainer& TargetTags, float DeltaTime = 0.0f) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, FBGL_TokenPool> TokenPools;

	virtual bool RequestToken(const FGameplayTag TokenTag, AActor* Requester)override;

	virtual void ReserveToken(const FGameplayTag TokenTag, AActor* Requester, float CooldownTimeNoToken)override;

	virtual void ReleaseToken(const FGameplayTag TokenTag, AActor* Requester, float ActionCooldownNoToken)override;

	float ActorUpdatedPriorityByDistance(AActor* Actor, float BasePriority, AActor* TargetObjective, float DistanceWeight) const;

	virtual int32 CheckIfRequesterHasToken(const FGameplayTag TokenTag, AActor* Requester)override;

	virtual int32 CheckWeakerCandidate(const FGameplayTag TokenTag, AActor* Requester, float RequesterPriority, AActor* TargetObjective) override;

	virtual bool TakeToken(int32 OwnerIndex, bool AlreadyAvailableSpace,int32 VictimIndex, FGameplayTag TokenTag, AActor* Requester, float RequesterPriority, AActor* TargetObjective)override;


	virtual bool CheckTokenAvailableSpace(const FGameplayTag TokenTag)override;

	void SetTokenCooldowns(FBGL_TokenPool& Pool, AActor* Holder, float CooldownTime);
	
	virtual bool RequestTokenWithPriority(const FGameplayTag TokenTag, AActor* Requester, float RequesterPriority, AActor* TargetObjective)override;
	
	virtual void LockToken(const FGameplayTag TokenTag, AActor* HolderActor)override;
};
