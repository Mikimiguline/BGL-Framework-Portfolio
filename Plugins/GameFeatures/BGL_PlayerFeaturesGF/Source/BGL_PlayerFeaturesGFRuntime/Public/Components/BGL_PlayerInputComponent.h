// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DataAssets/BGL_InputPlayerConfigDataAsset.h"
#include "BGL_PlayerInputComponent.generated.h"

class UBGL_InputPlayerConfigDataAsset;
class UInputMappingContext;
class UBGL_PlayerInputDAExtension;
class UBGL_MovementDataAssetExtension;
struct FInputActionValue;
class UEnhancedInputLocalPlayerSubsystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BGL_PLAYERFEATURESGFRUNTIME_API UBGL_PlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBGL_PlayerInputComponent();

	// Finds the input config in the data asset of the controlled pawn
	UBGL_InputPlayerConfigDataAsset* GetInputConfigFromOwner();

	// Single handler for every input action of the config; the tag identifies which one fired
	void Input_OnActionTriggered(const FInputActionValue& Value, FGameplayTag InputTag);

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BindInputActions(UBGL_InputPlayerConfigDataAsset* PlayerInputDA, APlayerController* PCin);

	// Possession: the controller now drives another body, which may bring another input config
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	// Adds the mapping context and bindings of the controlled body's input config (no-op if it is already applied)
	void ApplyInputConfig();

	// Removes the mapping context and every binding of this component
	void ClearInputConfig();

	APlayerController* GetInputPlayerController() const;


private:


	// Kept so the mapping can be removed when the config changes or the component is destroyed
	UPROPERTY()
	const UInputMappingContext* CurrentMappingContext = nullptr;

	// Config whose bindings are live right now
	UPROPERTY()
	TObjectPtr<UBGL_InputPlayerConfigDataAsset> CurrentInputConfig = nullptr;



};
