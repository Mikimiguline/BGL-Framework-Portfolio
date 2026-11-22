// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"

class AActor;

/**
 * Per-AI CPU cost (Game Thread).
 *
 * Every tick we own (BT, AIController, CharacterMovement, Character, AnimInstance)
 * opens a BGL_AI_COST_SCOPE with the pawn it belongs to. The profiler accumulates the
 * EXCLUSIVE time of each scope (with nested scopes, the child is subtracted from the parent, so nothing
 * is counted twice) and every 1 s window publishes each AI's average ms per frame and its worst frame.
 *
 * - Layer 0: knows nothing about the AI. It only receives (Actor, Bucket, Cycles).
 * - When disabled it costs one bool check per scope. In Shipping the macro does not exist.
 * - It also emits the scope to Unreal Insights (TRACE_CPUPROFILER_EVENT_SCOPE) so the figures can be cross-checked.
 *
 * Does not include centralized systems that do not belong to a specific AI (AIPerceptionSystem,
 * CrowdManager, async NavMesh) nor animation evaluation on worker threads.
 */

#define BGL_WITH_AI_COST_PROFILER !UE_BUILD_SHIPPING

enum class EBGL_AICostBucket : uint8
{
	Brain,       // Behavior Tree: tasks, services and decorators
	Controller,  // AIController tick (control rotation / focus)
	Movement,    // CharacterMovementComponent
	Actor,       // Character tick
	Animation,   // NativeUpdateAnimation (Game Thread part of the animation)
	Num
};

struct BGL_GAMECOREFRAMEWORK_API FBGL_AICostEntry
{
	static constexpr int32 NumBuckets = static_cast<int32>(EBGL_AICostBucket::Num);

	// --- Current window ---
	uint64 WindowCycles[NumBuckets] = {};
	uint64 FrameCycles = 0;
	uint64 FrameNumber = 0;
	uint64 WindowPeakFrameCycles = 0;

	// --- Result of the last closed window (ms per frame) ---
	float AvgMs[NumBuckets] = {};
	float TotalMs = 0.0f;
	float PeakMs = 0.0f;
};

class BGL_GAMECOREFRAMEWORK_API FBGL_AICostProfiler
{
public:
	static bool IsEnabled() { return bEnabled; }

	// Disabling it clears the collected data
	static void SetEnabled(bool bInEnabled);

	static void AddCycles(const AActor* Actor, EBGL_AICostBucket Bucket, uint64 Cycles);

	// Closes the window once WindowSeconds have elapsed. Returns true if it closed it (new data available)
	static bool UpdateWindow(double WindowSeconds = 1.0);

	static const FBGL_AICostEntry* Find(const AActor* Actor);
	static const TMap<TWeakObjectPtr<const AActor>, FBGL_AICostEntry>& GetEntries();

	static const TCHAR* GetBucketShortName(EBGL_AICostBucket Bucket);

private:
	static bool bEnabled;
};

// RAII scope: measures exclusive time (child scopes subtracted) and adds it to the actor's bucket
class BGL_GAMECOREFRAMEWORK_API FBGL_AICostScope
{
public:
	FBGL_AICostScope(const AActor* InActor, EBGL_AICostBucket InBucket);
	~FBGL_AICostScope();

	FBGL_AICostScope(const FBGL_AICostScope&) = delete;
	FBGL_AICostScope& operator=(const FBGL_AICostScope&) = delete;

private:
	const AActor* Actor = nullptr;
	FBGL_AICostScope* Parent = nullptr;
	uint64 StartCycles = 0;
	uint64 ChildCycles = 0;
	EBGL_AICostBucket Bucket = EBGL_AICostBucket::Actor;
	bool bActive = false;
};

#if BGL_WITH_AI_COST_PROFILER
	#define BGL_AI_COST_SCOPE(ActorPtr, BucketName) \
		TRACE_CPUPROFILER_EVENT_SCOPE(BGL_AI_##BucketName); \
		FBGL_AICostScope ANONYMOUS_VARIABLE(BGLAICostScope_)(ActorPtr, EBGL_AICostBucket::BucketName)
#else
	#define BGL_AI_COST_SCOPE(ActorPtr, BucketName)
#endif
