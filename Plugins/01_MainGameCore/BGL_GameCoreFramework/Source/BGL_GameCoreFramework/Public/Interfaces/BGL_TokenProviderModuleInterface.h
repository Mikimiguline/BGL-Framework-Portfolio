// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "BGL_TokenProviderModuleInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBGL_TokenProviderModuleInterface : public UInterface
{
	GENERATED_BODY()
};

class BGL_GAMECOREFRAMEWORK_API IBGL_TokenProviderModuleInterface
{
	GENERATED_BODY()

public:

	virtual bool RequestToken(FGameplayTag TokenTag, AActor* Requester) = 0;

	virtual void ReserveToken(const FGameplayTag TokenTag, AActor* Requester, float CooldownTimeNoToken) = 0;

	virtual int32 CheckIfRequesterHasToken(const FGameplayTag TokenTag, AActor* Requester) = 0;

	virtual bool CheckTokenAvailableSpace(const FGameplayTag TokenTag) = 0;

	virtual bool TakeToken(int32 OwnerIndex, bool AlreadyAvailableSpace, int32 VictimIndex, FGameplayTag TokenTag, AActor* Requester, float RequesterPriority, AActor* TargetObjective) = 0;

	virtual int32 CheckWeakerCandidate(const FGameplayTag TokenTag, AActor* Requester, float RequesterPriority, AActor* TargetObjective) = 0;
	
	virtual void ReleaseToken(const FGameplayTag TokenTag, AActor* Requester, float ActionCooldownNoToken) = 0;

	virtual bool RequestTokenWithPriority(const FGameplayTag TokenTag, AActor* Requester, float RequesterPriority, AActor* TargetObjective) = 0;

	virtual void LockToken(FGameplayTag TokenTag, AActor* HolderActor) = 0;
};
