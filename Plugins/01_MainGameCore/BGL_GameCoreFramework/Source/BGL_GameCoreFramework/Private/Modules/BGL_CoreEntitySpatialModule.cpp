// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.


#include "Modules/BGL_CoreEntitySpatialModule.h"
#include "kismet/GameplayStatics.h"
#include "Interfaces/BGL_EntityRegisterInterface.h"
#include "GameplayTagContainer.h"

void UBGL_CoreEntitySpatialModule::InitializeModule()
{

}

void UBGL_CoreEntitySpatialModule::InitializeModule(UUBGL_CoreModularManagerSubsystem* AUSubSystem)
{

}

void UBGL_CoreEntitySpatialModule::RunModule(IBGL_EntityRegisterInterface* SubsystemInterface, const FGameplayTagContainer& TargetActorTags, float DeltaTime)
{
	// Go through the player controller rather than the character
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	// The view target is whatever the player is currently looking through,
	// so this works for characters, pawns, free cameras and vehicles.
	const AActor* PlayerActor = PC->GetViewTarget();
	if (!PlayerActor) return;

	for (const FGameplayTag& GMTag : TargetActorTags) {

		TMap <TObjectPtr<AActor>, float>& TempSpatialRegistry = SubsystemInterface->GetGlobalSpatialRegistry();
		TMap <TObjectPtr<AActor>, float>& TempViewDots = SubsystemInterface->GetGlobalViewDotRegistry();
		CalculateDistance(PlayerActor, SubsystemInterface->GetEntitiesByTag(GMTag), TempSpatialRegistry, TempViewDots);
	}
	
	
}

void UBGL_CoreEntitySpatialModule::CalculateDistance(const AActor* PlayerActor, const TSet<TObjectPtr<AActor>>& TSetActors, TMap<TObjectPtr<AActor>, float>& EntityManagerDistances, TMap<TObjectPtr<AActor>, float>& EntityManagerViewDots)
{
	const FVector TempPlayerLocation = PlayerActor->GetActorLocation();

	for (AActor* Actor : TSetActors)
	{
		if (!Actor)
		{
			continue;
		}

		const FVector TempActorLocation = Actor->GetActorLocation();
		const FVector ToPlayer = TempPlayerLocation - TempActorLocation;
		const float DistSq = ToPlayer.SizeSquared();

		EntityManagerDistances.Add(Actor, DistSq);

		float ViewDot = 1.f;
		if (Actor != PlayerActor && DistSq > KINDA_SMALL_NUMBER)
		{
			ViewDot = FVector::DotProduct(Actor->GetActorForwardVector(), ToPlayer.GetSafeNormal());
		}
		EntityManagerViewDots.Add(Actor, ViewDot);

		ActivateActor(Actor, DistSq);
	}

}

void UBGL_CoreEntitySpatialModule::ActivateActor(const AActor* ActorActivate, float Distance)
{
	



}




