// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineBaseTypes.h"
#include "Interfaces/BGL_ModuleInterface.h"
#include "BGL_CoreAIDebugModule.generated.h"

class IBGL_EntityRegisterInterface;
class UBGL_CoreTokenModule;
class APawn;
class UWorld;
class UCanvas;
class APlayerController;
struct FBGL_AICostEntry;

// --- ROTATION DEBUG (independent of the BT / token / distance text) ---

// Other modules (e.g. UniversalAI) append their own lines to each AI's rotation text
DECLARE_MULTICAST_DELEGATE_TwoParams(FBGLCollectAIDebugLines, const APawn* /*Pawn*/, TArray<FString>& /*OutLines*/);

// Counts direction reversals of an angle (turning one way, then the other) during the 1 s window
struct FBGLYawFlipCounter
{
	float PrevValue = 0.0f;
	float PrevStep = 0.0f;
	bool bHasPrev = false;
	int32 Flips = 0;
	int32 Shown = 0;

	void Add(float Value);
	void CloseWindow() { Shown = Flips; Flips = 0; }
};

// Counts how many times a value changes during the 1 s window
struct FBGLChangeCounter
{
	int64 PrevValue = 0;
	bool bHasPrev = false;
	int32 Changes = 0;
	int32 Shown = 0;

	void Add(int64 Value);
	void CloseWindow() { Shown = Changes; Changes = 0; }
};

// Rotation debug state of one AI (not reflected)
struct FBGLRotationDebugState
{
	float WindowStartTime = -1.0f;

	FBGLYawFlipCounter ActorYaw;
	FBGLYawFlipCounter ControlYaw;
	FBGLYawFlipCounter RootYaw;

	FBGLChangeCounter Orient;
	FBGLChangeCounter Desired;
	FBGLChangeCounter UseCtrlYaw;
	FBGLChangeCounter Focus;
	FBGLChangeCounter Node;
	FBGLChangeCounter PathFollowing;
	FBGLChangeCounter Rate;

	// BT nodes that were active during the 1 s window (shows which ones it alternates between)
	TArray<FString> NodesInWindow;
	TArray<FString> NodesShown;

	// This frame's text (drawn on the Canvas, above the head and growing upwards)
	FString DrawText;
	FColor DrawColor = FColor::White;
	FVector DrawAnchor = FVector::ZeroVector;
	uint64 DrawFrame = 0;
};

/**
 * Centralized debug module for AI entities.
 * Gathers and displays above each character's head:
 * 1. Active Behavior Tree node / task.
 * 2. Combat token state (whether it has a turn assigned and which token it holds).
 * 3. Real-time distance to the target / player.
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class BGL_GAMECOREFRAMEWORK_API UBGL_CoreAIDebugModule : public UObject, public IBGL_ModuleInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeModule() override;
	virtual void RunModule(IBGL_EntityRegisterInterface* SubsystemInterface, const FGameplayTagContainer& TargetActorTags, float DeltaTime = 0.0f) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	bool bEnableDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	float HeightOffset = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	float FontScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	float DrawDuration = 0.06f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	float MaxDrawDistance = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	float MultDuration = 2.23f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	FColor DebugTextColor = FColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	FColor TokenSuccessColor = FColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config")
	FColor FailureColor = FColor::Red;

	// --- PER-AI COST (Game Thread ms). Independent of bEnableDebug (BT / token text). 1 s average per frame + worst frame ---
	// With both enabled the cost goes above the BT / token text; with cost only, it takes that text's position.
	// Measures BT, AIController, CharacterMovement, Character and AnimInstance (GT). See FBGL_AICostProfiler

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost")
	bool bShowAICost = false;

	// Cost above each character (total, peak and BT / Mov / Ctrl / Actor / Anim breakdown). Off = on-screen summary only
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost"))
	bool bShowAICostBreakdown = true;

	// On-screen summary: agents, total cost, average per AI and the most expensive AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost"))
	bool bShowAICostSummary = true;

	// Writes the summary to the Output Log once per second ([BGL_AICost])
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost"))
	bool bLogAICostSummary = false;

	// The cost is drawn above the BT / token text (in screen lines). Extra world-space offset, normally 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost"))
	float AICostTextOffset = 0.0f;

	// From this average cost (ms) upwards the AI is shown in the warning color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost", ClampMin = "0.0"))
	float AICostWarningMs = 0.03f;

	// From this average cost (ms) upwards the AI is shown in the critical color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost", ClampMin = "0.0"))
	float AICostCriticalMs = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost")
	FColor AICostOkColor = FColor::Cyan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost")
	FColor AICostWarningColor = FColor::Orange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost")
	FColor AICostCriticalColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|AI Cost", meta = (EditCondition = "bShowAICost", ClampMin = "0.1"))
	float AICostSummaryScale = 1.2f;

	// --- ROTATION: for finding AIs that jitter. Separate from the BT / token / distance text (does not modify it).
	// Sampled every frame even if the module has a ModuleTick. Does not depend on bEnableDebug ---

	// Draws, as a separate text, the rotation flags, focus, PathFollowing and how many times they change per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation")
	bool bShowRotationDebug = false;

	// Only draws the rotation text of AIs that are jittering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug"))
	bool bOnlyShowJitteringActors = false;

	// Turn direction reversals per second from which the AI is flagged as jittering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug", ClampMin = "1"))
	int32 JitterFlipsPerSecond = 4;

	// Height above the head (top of the capsule) where the rotation text starts. The text grows upwards
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug"))
	float RotationTextHeightOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug"))
	float RotationFontScale = 0.9f;

	// Opacity of the rotation text (0 = invisible, 1 = opaque)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug", ClampMin = "0.0", ClampMax = "1.0"))
	float RotationTextOpacity = 0.8f;

	// Opacity of the black background behind the rotation text (0 = no background)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug", ClampMin = "0.0", ClampMax = "1.0"))
	float RotationBackgroundOpacity = 0.3f;

	// Arrows: green = actor facing, blue = control rotation (focus), yellow = velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug"))
	bool bDrawRotationArrows = true;

	// Writes the jittering AIs to the Output Log once per second, with all their values ([BGL_RotDebug])
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation", meta = (EditCondition = "bShowRotationDebug"))
	bool bLogJitteringActors = false;

	// Experiment: the Custom Move To keeps its 'face the target while walking' hysteresis between executions (cvar bgl.MoveTo.KeepFacingHysteresis)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation")
	bool bKeepMoveToFacingHysteresis = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation")
	FColor RotationTextColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation")
	FColor JitterActorColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Config|Rotation")
	FColor JitterAnimationColor = FColor::Orange;

	static FBGLCollectAIDebugLines& OnCollectRotationDebugLines();

	virtual void BeginDestroy() override;

protected:
	FString GetBTNodeDescription(AActor* InActor) const;
	bool CheckActorHasToken(AActor* InActor, const UBGL_CoreTokenModule* TokenModule, FString& OutTokenNames) const;

	// Per-AI cost: overhead text and on-screen / log summary
	FString BuildAICostText(const FBGL_AICostEntry& Cost) const;
	FColor GetAICostColor(float CostMs) const;
	void ShowAICostSummary(const AActor* PlayerActor) const;

	// Hooks / unhooks the per-frame sampling according to bShowRotationDebug
	void UpdateRotationSampling();
	void OnWorldPostActorTick(UWorld* InWorld, ELevelTick TickType, float DeltaSeconds);
	void ApplyMoveToHysteresisOption() const;
	// Draws the rotation texts on the Canvas: centered above the head, bottom to top and with transparency
	void DrawRotationDebug(UCanvas* Canvas, APlayerController* PC);

	TMap<TWeakObjectPtr<const APawn>, FBGLRotationDebugState> RotationStates;
	FDelegateHandle PostActorTickHandle;
	FDelegateHandle DebugDrawHandle;
};
