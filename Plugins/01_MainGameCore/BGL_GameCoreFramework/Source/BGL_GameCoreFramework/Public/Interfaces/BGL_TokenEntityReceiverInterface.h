// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "BGL_TokenEntityReceiverInterface.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTokenRevokedSignature, const FGameplayTag&);

UINTERFACE(MinimalAPI)
class UBGL_TokenEntityReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

class BGL_GAMECOREFRAMEWORK_API IBGL_TokenEntityReceiverInterface
{
	GENERATED_BODY()

public:

	// Called when the token module revokes this entity's token to give it to a higher-priority requester
	virtual void OnTokenRevoked(const FGameplayTag& RevokedTokenTag) = 0;

	virtual FOnTokenRevokedSignature& GetTokenDelegate() = 0;

};
