// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CoreLibrary/BGL_CoreTypes.h"
#include "BGL_CoreGlobalModuleDataAsset.generated.h"


UCLASS()
class BGL_GAMECOREFRAMEWORK_API UBGL_CoreGlobalModuleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "AI Registry", meta = (DisplayName = "Module Master Catalog",Categories = "Subsystems"))
	TMap<FGameplayTag, FModuleStruct> ModuleCatalog;
	
};
