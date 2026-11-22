// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "BGL_EntityDataAssetInterface.generated.h"
class UBGL_ActorDataAssetExtension;
// HealthModule subscription
DECLARE_MULTICAST_DELEGATE_OneParam(FSubscribedComponents, float Damage);

UINTERFACE(MinimalAPI)
class UBGL_EntityDataAssetInterface : public UInterface
{
	GENERATED_BODY()
};

class BGL_GAMECOREFRAMEWORK_API IBGL_EntityDataAssetInterface
{
	GENERATED_BODY()

public:

	virtual int32 GetTeamID() const = 0;
	virtual bool OverrideFeaturesStruct(const FGameplayTag& GameplayTag, FInstancedStruct& StructOut) = 0;
	virtual class USkeletalMeshComponent* GetSkeletalMesh() const = 0;
	
	virtual const UBGL_ActorDataAssetExtension* FindExtensionByClass(TSubclassOf<UBGL_ActorDataAssetExtension> ExtensionClass) const = 0;

	// Inline helper: typed template lookup with no runtime or memory overhead
	template<typename T>
	const T* FindExtension() const
	{
		return Cast<T>(FindExtensionByClass(T::StaticClass()));
	}

	virtual void HandleEntityDeath() = 0;
	
};
