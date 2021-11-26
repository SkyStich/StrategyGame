// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/Components/SpectatorCameraComponent.h"

ASpectatorPlayerController::ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ASpectatorPlayerController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);

	FRotator ViewRotation(0.f);
	FRotator DeltaRot(0.f);

	if(PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}
	SetControlRotation(ViewRotation);
}
