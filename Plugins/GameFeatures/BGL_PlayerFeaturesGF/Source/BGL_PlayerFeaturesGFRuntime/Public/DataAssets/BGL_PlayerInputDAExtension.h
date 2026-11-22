// Copyright (c) 2026 Miguel Angel Monreal. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/BGL_PawnDataAssetExtension.h"
#include "BGL_PlayerInputDAExtension.generated.h"

class UBGL_InputPlayerConfigDataAsset;

USTRUCT(BlueprintType)
struct FBGL_CameraLag
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag", meta = (ClampMin = "0.0"))
    float CameraLagTranslationSpeed = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
    bool bEnableCameraRotationLag = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag", meta = (EditCondition = "bEnableCameraRotationLag", EditConditionHides))
    float CameraLagRotationSpeed = 0;

  
    /** When true, caps how far the camera can lag behind its target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
    bool bUseCameraLagMaxDistance = false;

    /**
     * Maximum distance between the camera and its target while bUseCameraLagMaxDistance is true.
     * Keeps the character on screen at very high speeds.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag", meta = (EditCondition = "bUseCameraLagMaxDistance", ClampMin = "0.0"))
    float CameraLagMaxDistance = 300.0f;

    /**
     * When true, camera lag is computed in fixed-time substeps so that its smoothness
     * is the same at any frame rate.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
    bool bUseSubstepping = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag", meta = (EditCondition = "bUseSubstepping", ClampMin = "0.0"))
    float CameraLagMaxTimeStep = 0.016667;

};

USTRUCT(BlueprintType)
struct FBGL_CameraConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float TargetArmLength = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator TargetArmRotation = FRotator(0.0f, -50.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bUseAbsoluteRotation = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector SocketOffset = FVector(0.0f, 50.0f, 50.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bDoCollisionTest = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FieldOfView = 90.0f;

    // Lets the controller rotation turn the camera
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bUsePawnControlRotation = false;

    // On possess the view starts at TargetArmRotation measured from the body's facing (Pitch = tilt, Yaw = turn around the body)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bUsePawnControlRotation"))
    bool bResetViewOnPossess = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bEnableCameraLag = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bEnableCameraLag", EditConditionHides, ShowOnlyInnerProperties))
    FBGL_CameraLag CameraLag;


};



UCLASS(DefaultToInstanced, EditInlineNew, BlueprintType)
class BGL_PLAYERFEATURESGFRUNTIME_API UBGL_PlayerInputDAExtension : public UBGL_PawnDataAssetExtension
{
	GENERATED_BODY()
	

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Control | Input")
    UBGL_InputPlayerConfigDataAsset* InputPlayerConfigDataAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Control | Camera", meta = (ShowOnlyInnerProperties))
    FBGL_CameraConfig CameraSettings;


};
