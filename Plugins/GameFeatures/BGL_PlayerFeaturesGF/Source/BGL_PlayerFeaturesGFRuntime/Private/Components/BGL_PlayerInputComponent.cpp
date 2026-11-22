// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.


#include "Components/BGL_PlayerInputComponent.h"
#include "DataAssets/BGL_PlayerInputDAExtension.h"
#include "DataAssets/BGL_InputPlayerConfigDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Bases/BGL_PlayerControllerBase.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "TagLibrary/BGL_GlobalTags.h"
#include "InputMappingContext.h"
#include "Interfaces/BGL_PawnAssetDataInterface.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBGL_PlayerInputComponent::UBGL_PlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}


UBGL_InputPlayerConfigDataAsset* UBGL_PlayerInputComponent::GetInputConfigFromOwner(){


	// The input config lives in the controlled pawn's data asset, as a UBGL_PlayerInputDAExtension
	ABGL_PlayerControllerBase* PC = Cast<ABGL_PlayerControllerBase>(GetOwner());
	if (!PC) return nullptr;
	IBGL_PawnAssetDataInterface* PawnDataInterface = Cast<IBGL_PawnAssetDataInterface>(PC->GetPawn());
	if (!PawnDataInterface)return nullptr;
	const TArray<TObjectPtr<UBGL_PawnDataAssetExtension>>* ArrayPawnDataExtension = PawnDataInterface->GetMainDataAsset();
	if (!ArrayPawnDataExtension || ArrayPawnDataExtension->IsEmpty()) return nullptr;

	const UBGL_PlayerInputDAExtension* PlayerDataExtension = nullptr;


	for (TObjectPtr<UBGL_PawnDataAssetExtension> PawnDataExtension : *ArrayPawnDataExtension) {
		PlayerDataExtension = Cast<UBGL_PlayerInputDAExtension>(PawnDataExtension);

		if (PlayerDataExtension)
			break;
	}

	if (!PlayerDataExtension)return nullptr;

	return PlayerDataExtension->InputPlayerConfigDataAsset;

}

void UBGL_PlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// Possession can swap the body (and its input config) at any time
	if (APlayerController* OwnerPC = Cast<APlayerController>(GetOwner()))
	{
		OwnerPC->OnPossessedPawnChanged.AddUniqueDynamic(this, &UBGL_PlayerInputComponent::HandlePossessedPawnChanged);
	}

	ApplyInputConfig();
}

APlayerController* UBGL_PlayerInputComponent::GetInputPlayerController() const
{
	// The owner is normally the player controller; otherwise fall back to the first one in the world
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC && GetWorld())
		PC = GetWorld()->GetFirstPlayerController();
	return PC;
}

void UBGL_PlayerInputComponent::ApplyInputConfig()
{
	UBGL_InputPlayerConfigDataAsset* Config = GetInputConfigFromOwner();
	// A body without input config keeps the current controls (so the player is never left without input)
	if (!Config || !Config->DefaultMappingContext) return;
	if (Config == CurrentInputConfig) return;

	APlayerController* PC = GetInputPlayerController();
	if (!PC) return;

	ClearInputConfig();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		// Priority 0: menus can be layered on top with a higher priority
		Subsystem->AddMappingContext(Config->DefaultMappingContext, 0);

		// Kept so the context can be removed later
		CurrentMappingContext = Config->DefaultMappingContext;
	}

	BindInputActions(Config, PC);
	CurrentInputConfig = Config;
}

void UBGL_PlayerInputComponent::ClearInputConfig()
{
	APlayerController* PC = GetInputPlayerController();
	if (!PC) return;

	// Remove the key mappings
	if (CurrentMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(CurrentMappingContext);
		}
	}

	// Remove every BindAction that targets this component
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		EnhancedInput->ClearBindingsForObject(this);
	}

	CurrentMappingContext = nullptr;
	CurrentInputConfig = nullptr;
}

void UBGL_PlayerInputComponent::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// The swap usually comes from one of our own bindings (the Possess input): rebinding while the
	// input component is dispatching is not safe, so we wait one frame
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(this, &UBGL_PlayerInputComponent::ApplyInputConfig);
	}
}

void UBGL_PlayerInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerController* OwnerPC = Cast<APlayerController>(GetOwner()))
	{
		OwnerPC->OnPossessedPawnChanged.RemoveDynamic(this, &UBGL_PlayerInputComponent::HandlePossessedPawnChanged);
	}

	ClearInputConfig();

	Super::EndPlay(EndPlayReason);
}

void UBGL_PlayerInputComponent::BindInputActions(UBGL_InputPlayerConfigDataAsset* PlayerInputDA, APlayerController* PCin)
{


	// The bindings live on the controller's input component, so the pawn knows nothing about input
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PCin->InputComponent))
	{
		for (const FBGL_InputMapping& ActionMap : PlayerInputDA->TaggedInputMapping)
		{
			if (ActionMap.InputAction && ActionMap.InputTag.IsValid())
			{
				// Every action goes to the same handler, with its GameplayTag as payload
				EnhancedInput->BindAction(
					ActionMap.InputAction,
					ActionMap.TriggerEvent,
					this,
					&UBGL_PlayerInputComponent::Input_OnActionTriggered,
					ActionMap.InputTag
				);
			}
		}
	}


}





void UBGL_PlayerInputComponent::Input_OnActionTriggered(const FInputActionValue& Value, FGameplayTag InputTag)
{
	AController* PC = Cast<AController>(GetOwner());
	if (!PC) return;

	APawn* ControlledPawn = PC->GetPawn();
	if (!ControlledPawn) return;

	// The input tag decides what the action does
	if (InputTag.MatchesTag(Tag_Controller_PlayerController_Move))
	{

		// 2D value: W/S is Y, A/D is X
		FVector2D MovementVector = Value.Get<FVector2D>();

		// Move relative to the control (camera) yaw
		const FRotator Rotation = PC->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);

		FVector WorldMoveDir = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal2D();

		// The controller decides whether this move direction cancels the attack in progress
		IBGL_PlayerLocalDataInterface* ActorInterface = Cast<IBGL_PlayerLocalDataInterface>(PC);
		if (ActorInterface && ActorInterface->ShouldCancelAttackOnMove(WorldMoveDir))
		{
			ActorInterface->StopAnimation(InputTag);
		}

		return;
	}

	// First/third person camera: X turns (yaw), Y tilts (pitch)
	if (InputTag.MatchesTag(Tag_Controller_PlayerController_Look))
	{
		const FVector2D LookVector = Value.Get<FVector2D>();
		ControlledPawn->AddControllerYawInput(LookVector.X);
		ControlledPawn->AddControllerPitchInput(LookVector.Y);
		return;
	}

	if (InputTag.MatchesTag(Tag_Controller_PlayerController_Jump))
	{
		if (ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn))
		{
			ControlledCharacter->Jump();
		}
		return;
	}

	// Take the body in front of us
	if (InputTag.MatchesTag(Tag_Controller_PlayerController_Possess))
	{
		if (ABGL_PlayerControllerBase* BGLPC = Cast<ABGL_PlayerControllerBase>(PC))
		{
			BGLPC->TryPossessSoftTarget();
		}
		return;
	}

	// Action / attack: forwarded to the controller with its tag
	if (InputTag.MatchesTag(Tag_Controller_PlayerController_Action))
	{
		IBGL_PlayerLocalDataInterface* ActorInterface = Cast<IBGL_PlayerLocalDataInterface>(PC);
		if (ActorInterface)
		{
			ActorInterface->ExecuteRequestPlayerAction(InputTag);
		}
	}
}


