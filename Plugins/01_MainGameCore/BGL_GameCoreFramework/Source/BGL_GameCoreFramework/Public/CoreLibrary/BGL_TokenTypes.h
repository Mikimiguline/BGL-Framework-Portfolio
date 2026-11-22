// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BGL_TokenTypes.generated.h"


UENUM(BlueprintType)
enum class EBGL_TokenCooldownRule : uint8
{
    FixedDelay          UMETA(DisplayName = "Fixed Delay (Seconds)"),
    SyncWithActionDelay UMETA(DisplayName = "Use Action Cooldown")
};


USTRUCT(BlueprintType)
struct FBGL_TokenHolderInfo
{
    GENERATED_BODY()

    UPROPERTY()
    TWeakObjectPtr<AActor> Holder;

    // When it was granted (using GetTimeSeconds)
    float TimeGranted = 0.0f;

    // Priority with which it won the ticket (higher number means more right to keep it)
    float HolderPriority = 0.0f;

    UPROPERTY()
    float ReservationExpireTime = 0.0f;

    UPROPERTY()
    float CacheActionCooldown = 0.0f;

    // If true, someone with much higher priority can take it away (e.g. while aiming)
    // If false, it is already physically attacking and cannot be taken
    bool bCanBePreempted = true;

};

USTRUCT(BlueprintType)
struct FBGL_TokenPool
{
    GENERATED_BODY()

    // How many can fire at the same time (e.g. 2 arrows at once across the whole combat)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    int32 MaxConcurrentTokens = 6;

    // Maximum time someone can hold the token before it is forced to return
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    float MaxHoldDuration = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    bool UseCooldown = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action cooldown", meta = (EditCondition = "UseCooldown", EditConditionHides))
    EBGL_TokenCooldownRule CooldownRule = EBGL_TokenCooldownRule::SyncWithActionDelay;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action cooldown", meta = (EditCondition = "CooldownRule == EBGL_TokenCooldownRule::FixedDelay && UseCooldown", EditConditionHides))
    float CooldownDelay;

    // Minimum score difference required to steal someone else's token
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    float PreemptionThreshold = 50.0f;

    // Exclusive time the last attacker has to chain another attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    float HolderReservationTime = 1.5f;

    // Weight applied to the requester's proximity score when it competes for a token
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    float CandidateDistancePreferenceMult = 1;
    // Weight applied to a current holder's proximity score when it is evaluated as a preemption victim
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
    float VictimDistancePreferenceMult = 1;

   
    // Who currently holds the token
    UPROPERTY()
    TArray<FBGL_TokenHolderInfo> ActiveHolders;

    UPROPERTY()
    TArray<float> CooldownSlots;
};
