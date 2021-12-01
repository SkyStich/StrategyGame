// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpectatorPlayerController.generated.h"

class USpectatorCameraComponent;

UCLASS()
class STRATEGYGAME_API ASpectatorPlayerController : public APlayerController
{
	GENERATED_BODY()

	/** Helper for find spectator camera component from controlled pawn */
	USpectatorCameraComponent* GetSpectatorCameraComponent();

protected:

	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void BeginPlay() override;

public:

	ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void UpdateRotation(float DeltaTime) override;
	
};
