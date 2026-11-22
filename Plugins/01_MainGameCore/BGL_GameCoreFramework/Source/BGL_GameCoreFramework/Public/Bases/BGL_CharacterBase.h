// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

// --- Unreal Engine Core & Interfaces ---
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Perception/AISightTargetInterface.h"
#include "StructUtils/InstancedStruct.h"

// --- Project Types & Core Libraries ---
#include "CoreLibrary/BGL_CoreMovementTypes.h"
#include "CoreLibrary/BGL_EntityCoreSharedFunctions.h"
#include "MotionWarpingComponent.h"

// --- Project Interfaces ---
#include "Interfaces/BGL_ActorLocalDataInterface.h"
#include "Interfaces/BGL_PawnAssetDataInterface.h"
#include "Interfaces/BGL_CoreEffectsInterface.h"
#include "Interfaces/BGL_CustomTagAssetInterface.h"
#include "Interfaces/BGL_EntityDataAssetInterface.h"
#include "Interfaces/BGL_TokenEntityReceiverInterface.h"
#include "Interfaces/BGL_OutcomeReceiverInterface.h"
#include "Interfaces/BGL_EntityPerformanceInterface.h"

#include "BGL_CharacterBase.generated.h"

// --- Forward Declarations ---
class AISense_Sight;
class UCharacterMovementComponent;
class UMotionWarpingComponent;
class UBGL_ActionComponent;
class UBGL_StateTagComponent;
class UBGL_PawnDataAsset;
class UBGL_ActorDataAssetExtension;
class UBGL_CoreVisualDataAssetExtension;
class UBGL_EffectSpecificDataAssetExten;
class UBGL_PawnCollisionDataAssetExten;
class UBGL_MovementDataAssetExtension;

// The data asset drives the whole character, so show it first in the details panel.
// The engine splits this list by spaces: only category names without spaces work here.
UCLASS(PrioritizeCategories = "Data")
class BGL_GAMECOREFRAMEWORK_API ABGL_CharacterBase : public ACharacter,
	public IGenericTeamAgentInterface,
	public IAISightTargetInterface,
	public IGameplayTagAssetInterface,
	public IBGL_CustomTagAssetInterface,
	public IBGL_EntityDataAssetInterface,
	public IBGL_PawnAssetDataInterface,
	public IBGL_CoreEffectsInterface,
	public IBGL_OutcomeReceiverInterface,
	public IBGL_EntityPerformanceInterface,
	public IBGL_TokenEntityReceiverInterface
{
	GENERATED_BODY()

	// =========================================================================
	// --- INITIALIZATION & LIFECYCLE ---
	// =========================================================================

#pragma region Initialization & Lifecycle

public:

	ABGL_CharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual int32 GetPipelinePriority() const { return 0; }

protected:

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	void RemoveActorFromList(const EEndPlayReason::Type EndPlayReason);

	// The body can be possessed several times (player <-> AI). Data asset components, effects and the
	// entity registration happen only on the first possession; later ones only swap the controller state.
	bool bCoreInitialized = false;
	bool bTeamChangedOnPossess = false;

	// Core Processing & Pipeline
	UPROPERTY()
	FBGL_EntityCoreSharedFunctions CoreFunctions;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Core")
	FBGL_EntityDynamicCoreSharedData EntityCoreSharedData;

	UPROPERTY()
	TArray<UActorComponent*> SortedPipeline;

	// Subscription delegate (Not in use / Legacy)
	FSubscribedComponents SubscribedComponents;
#pragma endregion



	// =========================================================================
	// --- MODULAR DATA & ASSET MANAGEMENT ---
	// =========================================================================

#pragma region Modular Data & Asset Management

public:
	// --- Data Asset Getters & Overrides ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	const UBGL_PawnDataAsset* DataAsset;

	virtual const UBGL_PawnDataAsset* GetPawnDataAsset() const override { return DataAsset; }
	virtual const TArray<TObjectPtr<UBGL_PawnDataAssetExtension>>* GetMainDataAsset() const override { if (DataAsset) return &DataAsset->DataAssetExtension; return nullptr; }
	virtual const UBGL_ActorDataAssetExtension* FindExtensionByClass(TSubclassOf<UBGL_ActorDataAssetExtension> ExtensionClass) const override;


	// --- Visual & Topology ---
	// Mesh where animation plays (AnimInstance, montages, root motion)
	virtual USkeletalMeshComponent* GetSkeletalMesh() const override { return AnimationMeshComponent ? AnimationMeshComponent.Get() : GetMesh(); }

	// Mesh the player sees (attachments, sockets, death collision)
	USkeletalMeshComponent* GetVisualMesh() const { return VisualMeshComponent ? VisualMeshComponent.Get() : GetMesh(); }

	// Called by the performance LOD payloads instead of touching GetMesh() directly,
	// so characters with a hidden animation driver can decide visibility from the visible mesh.
	virtual void ApplyAnimTickPolicy(float TickInterval, bool bEvaluatePoseWhenNotRendered);


	// --- Component Getters ---
	virtual UMotionWarpingComponent* GetMotionWarping() const override;
	virtual class UCharacterMovementComponent* GetCharacterMovementComponentInterface() const override { return GetCharacterMovement(); };
	virtual class UBGL_ActionComponent* GetActionComponent() const override { return ActionComponent; }


	// --- Movement & AI Profile Cache ---
	const FBGL_MovementProfile* CacheMovementStateProfile;
	virtual void ApplyAIMovementExtension(const FGameplayTag& TypeOfMovement) override;
	virtual void ApplyAIMovementExtensionWithStruct(const FBGL_MovementProfile& CustomMovingSettingsIn) override;

	// We need to save it to give it back to the Char if we change movement profile previously
	virtual void SaveAIMovementStateCache(const FGameplayTag& TypeOfMovement) override;

	virtual const FBGL_MovementProfile* GetSaveAIMovementStateCache() override;


	// --- Module Struct Overrides ---
	UPROPERTY(EditAnywhere, Category = "AI DataAsset override")
	TMap<FGameplayTag, FBGL_ModuleOverrideData> ModuleStructs;

	virtual bool OverrideFeaturesStruct(const FGameplayTag& GameplayTag, FInstancedStruct& StructOut) override
	{
		return EntityCoreSharedData.OverrideFeaturesStruct(GameplayTag, StructOut);
	}

protected:

	void InitializeDataAsset();
	void AddDAEffectToGlobalLibrary();


	// Visual Setup
	virtual void ApplyVisualExtension(const UBGL_CoreVisualDataAssetExtension* VisualData);

	// Intermediate mesh references, assigned in PostInitializeComponents.
	// Normal character: both point to GetMesh().
	// Retarget character: Animation = hidden driver, Visual = visible mesh.
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> AnimationMeshComponent;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> VisualMeshComponent;
	void SetVisualAttachments(const UBGL_CoreVisualDataAssetExtension& VisualData, USkeletalMeshComponent* MyMesh);

	// Translates a socket tag into the skeleton socket name
	virtual FName GetSocketNameFromTag_Implementation(FGameplayTag SocketTag) const override;

	UPROPERTY(Transient)
	const UBGL_CoreVisualDataAssetExtension* CachedVisualData;


	// Collision Setup
	void ApplyCollisionExtension(const UBGL_PawnCollisionDataAssetExten* CollisionData);
	virtual void HandleEntityDeath() override;

	UPROPERTY(Transient)
	TArray<UShapeComponent*> ActiveCollisionComponents;

	// Movement Extensions Setup
	void ApplySharedMovementExtension(const UBGL_MovementDataAssetExtension* MovementDAExt, UCharacterMovementComponent* MoveCompIn);
	void ApplyPlayerMovementExtension(const UBGL_MovementDataAssetExtension* CachedMovementData, UCharacterMovementComponent* MoveCompIn);

	// Core Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	UPROPERTY()
	UBGL_ActionComponent* ActionComponent;

#pragma endregion



	// =========================================================================
	// --- TAG MANAGEMENT ---
	// =========================================================================

#pragma region Tag Management

public:

	// Delegates & Broadcasters
	FOnGameComponentsUpdated OnAITagsUpdatedDelegate;
	virtual FOnGameComponentsUpdated& GetAITagsUpdatedDelegate() override;

	// Tags delegate
	FOnCharTagUpdate CharacterActorAndTagUpdateDelegate;
	
	//Returns the delegate any UObject can subscribe to in order to react to tag updates on the character

	virtual FOnCharTagUpdate& CharacterActorAndTagUpdate() override { return CharacterActorAndTagUpdateDelegate; }

	//Calls broadcast to notify subscribed UObjects
	UFUNCTION()
	void SendCharacterUpdatedTagsToListeners();

	// Tag Manipulation
	virtual void AddStateTag(const FGameplayTagContainer& TempInstanceAddedTags) override;
	virtual void RemoveStateTag(const FGameplayTagContainer& TempInstanceRemovedTags) override;
	virtual void AddUniqueTag(const FGameplayTag& TempGameplayTag) override;
	virtual void RemoveUniqueTag(const FGameplayTag& TempGameplayTagoverride) override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;


protected:
	// The component that has the Character Tags
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	TObjectPtr<UBGL_StateTagComponent> StateTagComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FGameplayTagContainer CapabilityTags;

#pragma endregion



	// =========================================================================
	// --- AI and PERCEPTION ---
	// =========================================================================

#pragma region AI and Perception

public:

	// TEAM AGENT

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Team ID")
	FGenericTeamId TeamID = FGenericTeamId(255);

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; }

	virtual int32 GetTeamID() const override { return TeamID.GetId(); }

	// SIGHT TARGET INTERFACE

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
	TEnumAsByte<ECollisionChannel> SightCollisionChannel = ECC_Visibility;

	UFUNCTION(BlueprintNativeEvent)
	bool CanBeSeenOverride();

	virtual bool CanBeSeenOverride_Implementation();

	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
		const FCanBeSeenFromContext& Context,
		FVector& OutSeenLocation,
		int32& OutNumberOfLoSChecksPerformed,
		int32& OutNumberOfAsyncLosCheckRequested,
		float& OutSightStrength,
		int32* UserData = nullptr,
		const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr
	) override;

#pragma endregion



	// =========================================================================
	// --- EFFECTS ---
	// =========================================================================

#pragma region Effects

public:

	// DELEGATES

	FOnPhysicalEffectTriggered OnPhysicalEffectTriggered;

	FOnPhysicalEffectTriggeredEnd OnPhysicalEffectTriggeredEnd;

	virtual FOnPhysicalEffectTriggered& GetPhysicalEffectDelegate() override { return OnPhysicalEffectTriggered; }

	virtual FOnPhysicalEffectTriggeredEnd& GetPhysicalEffectDelegateEnd() override { return OnPhysicalEffectTriggeredEnd; }

	// ANIM NOTIFY TRIGGERS
	// 
	//Called by the animation anim notify
	virtual void OnEffectTriggered_Implementation(FGameplayTag TriggerTag, FGameplayTagContainer SocketTagContainer)override 
	{ OnPhysicalEffectTriggered.Broadcast(TriggerTag, SocketTagContainer); }

	//Called by the animation anim notify
	virtual void OnEffectTriggeredEnd_Implementation(FGameplayTag TriggerTag)override 
	{ OnPhysicalEffectTriggeredEnd.Broadcast(TriggerTag); }
	
	// OUTCOMES & REACTIONS

	virtual void ReceiveOutcomes_Implementation(const FGameplayTagContainer& OutcomeTags, const FBGL_EffectExchangeContext& Context) override;

	void EvaluateDamageReaction();

#pragma endregion



	// =========================================================================
	// --- TOKENS & OPTIMIZATION ---
	// =========================================================================

#pragma region Tokens & Optimization

public:

	// TOKEN MANAGEMENT (IBGL_TokenEntityReceiverInterface)

	FOnTokenRevokedSignature TokenDelegate;

	virtual FOnTokenRevokedSignature& GetTokenDelegate() override;

	virtual void OnTokenRevoked(const FGameplayTag& RevokedTokenTag) override;

	// OPTIMIZATION & LOD (IBGL_EntityPerformanceInterface)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optimization")
	EBGL_OptimizationTier CurrentTier = EBGL_OptimizationTier::LOD_0_MaxQuality;

	virtual EBGL_OptimizationTier GetCurrentTier() const override { return CurrentTier; }

	virtual void OnOptimizationTierChanged(EBGL_OptimizationTier NewTier) override { CurrentTier = NewTier; }

#pragma endregion
};
