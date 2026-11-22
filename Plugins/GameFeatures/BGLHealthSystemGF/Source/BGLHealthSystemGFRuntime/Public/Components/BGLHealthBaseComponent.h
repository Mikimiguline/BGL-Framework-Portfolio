// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/BGL_ComponentPriorityInterface.h"
#include <StructUtils/InstancedStruct.h>
#include "BGLHealthBaseComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
class IBGL_CustomTagAssetInterface;
class ABGL_CharacterBase;
class UBGL_CoreEffectsTypes;
struct FBGL_CoreEffectContext;
class IBGL_EntityDataAssetInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BGLHEALTHSYSTEMGFRUNTIME_API UBGLHealthBaseComponent : public UActorComponent, public IBGL_ComponentPriorityInterface
{
	GENERATED_BODY()

public:	
	UBGLHealthBaseComponent();

protected:
	virtual void BeginPlay() override;

	void CheckDataAssetVariables(const IBGL_EntityDataAssetInterface* OwnerInterface);

	virtual void ProcessEffect(FBGL_EffectExchange& EffectExchange) override;

public:	

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Health Basic")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Basic")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Basic")
	bool bHealthDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Basic")
	bool Invulnerability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Basic")
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Priority")
	int32 PipelinePriority = 0;

	void HealthDamageCalculation(float HealthDamage);
	void HealthHealCalculation(float HealthHeal);

	virtual int32 GetPipelinePriority() const {return PipelinePriority;}

	
	UPROPERTY(BlueprintAssignable, Category = "BGL|Health|Events")
	FOnHealthChanged OnHealthChanged;

private:


	TObjectPtr<IBGL_CustomTagAssetInterface> CustomTagInterface;
		
};
