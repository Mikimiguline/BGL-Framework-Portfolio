// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "BGL_EntityRegisterInterface.generated.h"

class IBGL_RegistryReceiverInterface;
UINTERFACE(MinimalAPI)
class UBGL_EntityRegisterInterface : public UInterface
{
	GENERATED_BODY()
};

class BGL_GAMECOREFRAMEWORK_API IBGL_EntityRegisterInterface
{
	GENERATED_BODY()

public:

	virtual void SubscribeReceiver(IBGL_RegistryReceiverInterface* NewReceiver)= 0;
	

	virtual void UnsubscribeReceiver(IBGL_RegistryReceiverInterface* ReceiverToRemove)= 0;
	

	virtual void RegisterEntity(AActor* InActor, const FGameplayTagContainer& InitialIDContainerTag) = 0;
	virtual void UnregisterEntity(AActor* InActor) = 0;

	virtual const TSet<TObjectPtr<AActor>>& GetEntitiesByTag(const FGameplayTag& TagToFind) const  = 0;
	virtual TMap<TObjectPtr<AActor>, float>& GetGlobalSpatialRegistry() = 0;
	virtual TMap<TObjectPtr<AActor>, float>& GetGlobalViewDotRegistry() = 0;
};
