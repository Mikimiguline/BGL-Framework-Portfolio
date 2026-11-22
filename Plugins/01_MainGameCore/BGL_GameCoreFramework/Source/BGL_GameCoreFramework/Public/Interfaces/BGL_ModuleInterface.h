// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "BGL_ModuleInterface.generated.h"

class IBGL_EntityRegisterInterface;
UINTERFACE(MinimalAPI)
class UBGL_ModuleInterface : public UInterface
{
	GENERATED_BODY()
};

class BGL_GAMECOREFRAMEWORK_API IBGL_ModuleInterface
{
	GENERATED_BODY()

public:

	virtual void InitializeModule() = 0;
	virtual void RunModule(IBGL_EntityRegisterInterface* SubsystemInterface,  const FGameplayTagContainer& TargetActorTags, float DeltaTime = 0.0f) = 0;
};
