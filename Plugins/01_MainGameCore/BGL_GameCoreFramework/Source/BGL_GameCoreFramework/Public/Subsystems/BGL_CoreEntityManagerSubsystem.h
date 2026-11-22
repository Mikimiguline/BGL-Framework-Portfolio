// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interfaces/BGL_EntityRegisterInterface.h"
#include "GameplayTagContainer.h"
#include "BGL_CoreEntityManagerSubsystem.generated.h"

class IBGL_RegistryReceiverInterface;

// An entity changed team at runtime (e.g. the player possessed it). Perception and targeting must re-evaluate it.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntityTeamChanged, AActor* /*ChangedActor*/);

USTRUCT()
struct FBGL_ActorSet
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<TObjectPtr<AActor>> RegisteredActors;
};
UCLASS()
class BGL_GAMECOREFRAMEWORK_API UBGL_CoreEntityManagerSubsystem : public UTickableWorldSubsystem, public IBGL_EntityRegisterInterface
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime)override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override{ RETURN_QUICK_DECLARE_CYCLE_STAT(UBGL_CoreEntityManagerSubsystem, STATGROUP_Tickables); }

	virtual bool IsTickable() const override;

	// Other subsystems (AI, combat) subscribe here to be notified of registrations
	virtual void SubscribeReceiver(IBGL_RegistryReceiverInterface* NewReceiver) override;
	virtual void UnsubscribeReceiver(IBGL_RegistryReceiverInterface* ReceiverToRemove) override;

	// Called by the actors themselves (enemy, player, trap) from BeginPlay/EndPlay
	virtual void RegisterEntity(AActor* InActor, const FGameplayTagContainer& InitialIDContainerTag)override;
	virtual void UnregisterEntity(AActor* InActor)override;

	// Moves an already registered actor to a new set of tag lists (e.g. its controller changed on possession).
	// Receivers are not notified: the actor stays alive and keeps its caches.
	void UpdateEntityTags(AActor* InActor, const FGameplayTagContainer& NewTags);

	FOnEntityTeamChanged OnEntityTeamChanged;

	UPROPERTY()
	TMap<TObjectPtr<AActor>, FGameplayTagContainer> ActorsPendingRegistration;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> ActorsPendingUnregistration;

	void ChangeOnTagContainer(AActor* InActor, const FGameplayTagContainer& Tag, bool bAdded);

	virtual const TSet<TObjectPtr<AActor>>& GetEntitiesByTag(const FGameplayTag& TagToFind) const override;

private:
	// Receivers are stored through the abstract interface, never as the plugin classes
	TArray<IBGL_RegistryReceiverInterface*> RegistratedReceivers;

	// Census of the match: every live registered actor
	UPROPERTY()
	TSet<TObjectPtr<AActor>> RegisteredEntities;

	// Processes the pending registration/unregistration buffers in per-frame batches
	void BufferProcessPendingActors();

	UPROPERTY()
	TMap<FGameplayTag, FBGL_ActorSet> RegisteredEntitiesByTag;

	TMap<TObjectPtr<AActor>, float> RegisteredEntitiesDistances;

	virtual TMap<TObjectPtr<AActor>, float>& GetGlobalSpatialRegistry() override{ return RegisteredEntitiesDistances; }

	TMap<TObjectPtr<AActor>, float> RegisteredEntitiesViewDots;

	virtual TMap<TObjectPtr<AActor>, float>& GetGlobalViewDotRegistry() override { return RegisteredEntitiesViewDots; }

};
