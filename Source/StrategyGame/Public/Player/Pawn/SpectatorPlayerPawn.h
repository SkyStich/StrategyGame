// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "SpectatorPlayerPawn.generated.h"

class USpectatorCameraComponent;

UCLASS()
class STRATEGYGAME_API ASpectatorPlayerPawn : public ASpectatorPawn
{
	GENERATED_BODY()

private:

	virtual void MoveForward(float Val) override;
	virtual void MoveRight(float Val) override;
	
public:

	ASpectatorPlayerPawn(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	USpectatorCameraComponent* GetSpectatorCameraComponent() const { return SpectatorCameraComponent; }

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraComponent", meta=(AllowPrivateAccess = "true"))
	USpectatorCameraComponent* SpectatorCameraComponent;
};
