// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#include "Debug/BGL_AICostProfiler.h"
#include "GameFramework/Actor.h"
#include "HAL/PlatformTime.h"

bool FBGL_AICostProfiler::bEnabled = false;

namespace BGLAICostProfiler
{
	struct FState
	{
		TMap<TWeakObjectPtr<const AActor>, FBGL_AICostEntry> Entries;
		double WindowStartSeconds = 0.0;
		uint64 WindowStartFrame = 0;
		bool bWindowStarted = false;
	};

	FState& GetState()
	{
		static FState State;
		return State;
	}

	// Stack of open scopes (Game Thread only) used to compute exclusive time
	FBGL_AICostScope* GCurrentScope = nullptr;
}

void FBGL_AICostProfiler::SetEnabled(bool bInEnabled)
{
	if (bEnabled == bInEnabled)
	{
		return;
	}

	bEnabled = bInEnabled;

	BGLAICostProfiler::FState& State = BGLAICostProfiler::GetState();
	State.Entries.Reset();
	State.bWindowStarted = false;
}

void FBGL_AICostProfiler::AddCycles(const AActor* Actor, EBGL_AICostBucket Bucket, uint64 Cycles)
{
	if (!bEnabled || !Actor)
	{
		return;
	}

	FBGL_AICostEntry& Entry = BGLAICostProfiler::GetState().Entries.FindOrAdd(Actor);
	Entry.WindowCycles[static_cast<int32>(Bucket)] += Cycles;

	// Whole-frame cost of this AI (feeds the window peak)
	if (Entry.FrameNumber != GFrameCounter)
	{
		Entry.WindowPeakFrameCycles = FMath::Max(Entry.WindowPeakFrameCycles, Entry.FrameCycles);
		Entry.FrameCycles = 0;
		Entry.FrameNumber = GFrameCounter;
	}
	Entry.FrameCycles += Cycles;
}

bool FBGL_AICostProfiler::UpdateWindow(double WindowSeconds)
{
	if (!bEnabled)
	{
		return false;
	}

	BGLAICostProfiler::FState& State = BGLAICostProfiler::GetState();
	const double Now = FPlatformTime::Seconds();

	if (!State.bWindowStarted)
	{
		State.bWindowStarted = true;
		State.WindowStartSeconds = Now;
		State.WindowStartFrame = GFrameCounter;
		return false;
	}

	const uint64 Frames = GFrameCounter - State.WindowStartFrame;
	if ((Now - State.WindowStartSeconds) < WindowSeconds || Frames == 0)
	{
		return false;
	}

	// Average per world frame: if an AI ticks every 2 frames (LOD), its cost is spread across both
	const double InvFrames = 1.0 / static_cast<double>(Frames);

	for (auto It = State.Entries.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		FBGL_AICostEntry& Entry = It.Value();

		double TotalMs = 0.0;
		for (int32 Index = 0; Index < FBGL_AICostEntry::NumBuckets; ++Index)
		{
			const double BucketMs = FPlatformTime::ToMilliseconds64(Entry.WindowCycles[Index]) * InvFrames;
			Entry.AvgMs[Index] = static_cast<float>(BucketMs);
			Entry.WindowCycles[Index] = 0;
			TotalMs += BucketMs;
		}
		Entry.TotalMs = static_cast<float>(TotalMs);

		const uint64 PeakCycles = FMath::Max(Entry.WindowPeakFrameCycles, Entry.FrameCycles);
		Entry.PeakMs = static_cast<float>(FPlatformTime::ToMilliseconds64(PeakCycles));
		Entry.WindowPeakFrameCycles = 0;
	}

	State.WindowStartSeconds = Now;
	State.WindowStartFrame = GFrameCounter;
	return true;
}

const FBGL_AICostEntry* FBGL_AICostProfiler::Find(const AActor* Actor)
{
	return Actor ? BGLAICostProfiler::GetState().Entries.Find(Actor) : nullptr;
}

const TMap<TWeakObjectPtr<const AActor>, FBGL_AICostEntry>& FBGL_AICostProfiler::GetEntries()
{
	return BGLAICostProfiler::GetState().Entries;
}

const TCHAR* FBGL_AICostProfiler::GetBucketShortName(EBGL_AICostBucket Bucket)
{
	switch (Bucket)
	{
	case EBGL_AICostBucket::Brain:      return TEXT("BT");
	case EBGL_AICostBucket::Controller: return TEXT("Ctrl");
	case EBGL_AICostBucket::Movement:   return TEXT("Mov");
	case EBGL_AICostBucket::Actor:      return TEXT("Actor");
	case EBGL_AICostBucket::Animation:  return TEXT("Anim");
	default:                            return TEXT("?");
	}
}

// ============================================================================
// SCOPE
// ============================================================================

FBGL_AICostScope::FBGL_AICostScope(const AActor* InActor, EBGL_AICostBucket InBucket)
	: Actor(InActor)
	, Bucket(InBucket)
{
	if (!FBGL_AICostProfiler::IsEnabled() || !InActor || !IsInGameThread())
	{
		return;
	}

	bActive = true;
	Parent = BGLAICostProfiler::GCurrentScope;
	BGLAICostProfiler::GCurrentScope = this;
	StartCycles = FPlatformTime::Cycles64();
}

FBGL_AICostScope::~FBGL_AICostScope()
{
	if (!bActive)
	{
		return;
	}

	const uint64 Elapsed = FPlatformTime::Cycles64() - StartCycles;
	const uint64 Exclusive = (Elapsed > ChildCycles) ? (Elapsed - ChildCycles) : 0;

	BGLAICostProfiler::GCurrentScope = Parent;
	if (Parent)
	{
		Parent->ChildCycles += Elapsed;
	}

	FBGL_AICostProfiler::AddCycles(Actor, Bucket, Exclusive);
}
