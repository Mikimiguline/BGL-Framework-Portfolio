// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#include "Components/BGLHealthBaseComponent.h"
#include "GameplayTagContainer.h"
#include "TagLibrary/BGL_UniversalAITags.h"
#include "TagLibrary/BGL_GlobalTags.h"
#include "GameplayTagAssetInterface.h"
#include "CoreLibrary/BGL_CoreTypes.h"
#include "CoreLibrary/BGL_CoreFunctionLibrary.h"
#include "Effects/BGL_CoreEffectsTypes.h"
#include "Interfaces/BGL_CustomTagAssetInterface.h"
#include "DataAssets/BGL_HealthDataAssetExtension.h"
#include "DataAssets/BGL_PawnDataAsset.h"
#include "Components/BGL_ActionComponent.h"
#include "Bases/BGL_CharacterBase.h"

UBGLHealthBaseComponent::UBGLHealthBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Health = 100;
	MaxHealth = 100;
}

void UBGLHealthBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	CustomTagInterface = Cast<IBGL_CustomTagAssetInterface>(GetOwner());
	// Read the config straight from the owner's data asset; nothing is cached on the component
	if (const IBGL_EntityDataAssetInterface* OwnerInterface = Cast<IBGL_EntityDataAssetInterface>(GetOwner()))
	{
		CheckDataAssetVariables(OwnerInterface);
	}

}

void UBGLHealthBaseComponent::CheckDataAssetVariables(const IBGL_EntityDataAssetInterface* OwnerInterface)
{
	if (!OwnerInterface) return;

	if (const UBGL_HealthDataAssetExtension* HealthData = OwnerInterface->FindExtension<UBGL_HealthDataAssetExtension>())
	{
		const FBGL_HealthConfig* SourceConfig = nullptr;
		if (const IGameplayTagAssetInterface* TagOwner = Cast<IGameplayTagAssetInterface>(GetOwner()))
		{
			if (TagOwner->HasMatchingGameplayTag(Tag_Controller_PlayerController))
			{
				SourceConfig = &HealthData->PlayerHealthConfig;
			}
			else {
				SourceConfig = &HealthData->HealthConfig;
			}
		}
		else { UE_LOG(LogTemp, Error, TEXT("UBGLHealthBaseComponent: owner does not implement IGameplayTagAssetInterface")); return; }

		if (SourceConfig == nullptr) { return; }

		bool bOverrideActive = false;
		const FBGL_HealthConfig OverriddenConfig = UBGL_CoreFunctionLibrary::OverrideInstStructDataModuleBool<FBGL_HealthConfig>(
			GetOwner(),
			Tag_GameFeatures_HealthSystem_BaseHealth,
			bOverrideActive,
			*SourceConfig
		);

		if (bOverrideActive)
		{
			SourceConfig = &OverriddenConfig;
		}

		Health = SourceConfig->InitHealth;
		MaxHealth = SourceConfig->MaxHealth;
		bHealthDebug = SourceConfig->DebugHealth;
	}
}

void UBGLHealthBaseComponent::ProcessEffect(FBGL_EffectExchange& EffectExchange)
{

	HealthDamageCalculation(EffectExchange.ComputedDamage);

}

void UBGLHealthBaseComponent::HealthDamageCalculation(float HealthDamage)
{
	
	// Nothing to apply
	if (HealthDamage == 0.0f) return;

	// Damage (> 0) is ignored while invulnerable or already dead
	if (HealthDamage > 0.0f && (Invulnerability || Health <= 0.0f)) return;

	// Healing (< 0) is ignored at full health
	if (HealthDamage < 0.0f && Health >= MaxHealth) return;

	Health = FMath::Clamp(Health - HealthDamage, 0.0f, MaxHealth);

	if(bHealthDebug)
		UE_LOG(LogTemp, Warning, TEXT("Health %f"),Health);

	OnHealthChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0 && CustomTagInterface) {
		CustomTagInterface->AddUniqueTag(Tag_Status_Dead);
	}
}

void UBGLHealthBaseComponent::HealthHealCalculation(float HealthHeal)
{
	// Nothing to apply
	if (HealthHeal == 0.0f) return;

	// Only a positive amount heals, and only below max health
	if (HealthHeal > 0.0f && Health < MaxHealth){

	Health = FMath::Clamp(Health + HealthHeal, 0.0f, MaxHealth);

	if (bHealthDebug)
		UE_LOG(LogTemp, Warning, TEXT("Health %f"), Health);

	OnHealthChanged.Broadcast(Health, MaxHealth);

	}

}

