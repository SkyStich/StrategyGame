// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpectatorPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API ASpectatorPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void UpdateRotation(float DeltaTime) override;
};
