// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "SpectatorCameraComponent.generated.h"

class APlayerController;
class APawn;

UCLASS(Config=game, BlueprintType, HideCategories=Trigger, meta=(BlueprintSpawnableComponent))
class STRATEGYGAME_API USpectatorCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

private:

	/** Get owner player controller helper */
	APlayerController* GetPlayerController();

	/** Get owner pawn helper */
	APawn* GetOwnerPawn();

	/** Update zoom alpha */
	void UpdateZoomAlpha(float const NewAlpha);

public:

	USpectatorCameraComponent(const FObjectInitializer& ObjectInitializer);

	/** UCameraComponent interface */
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	/** decrease current amount of the camera zoom */
	void DecreaseZoomLevel();

	/** Increase current amount of the camera zoom */
	void IncreaseZoomLevel();

	/** Update camera movement with mouse support */
	void UpdateCameraMovement(APlayerController* InPlayerController);

	/** Move forward owner pawn */
	void MoveForward(const float Value);
	
	/** Move right owner pawn */
	void MoveRight(const float Value);

public:

	/** The minimum offset of the camera */
	UPROPERTY(Config)
	float MinCameraOffset;
	
	/** The maximum offset of the camera */
	UPROPERTY(Config)
	float MaxCameraOffset;
	
	/** The angle to look down on the map. */
	UPROPERTY(config)
	FRotator FixedCameraAngle;
	
	/** Size of the area at the edge of the screen that will trigger camera scrolling. */
    UPROPERTY(config)
    uint32 CameraActiveBorder;
    
	/** How fast the camera moves around when the mouse is at the edge of the screen. */
	UPROPERTY(config)
	float CameraScrollSpeed;

private:
	/** Current amount of the camera zoom */
	float ZoomAlpha;

	/** min amount of the camera zoom */
	float MinZoomLevel;

	/** max amount of the camera zoom */
	float MaxZoomLevel;
};
