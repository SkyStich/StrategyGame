// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerController/SpectatorPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Player/Components/SpectatorCameraComponent.h"

ASpectatorPlayerController::ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	NetUpdateFrequency = 5.f;

	/** Support mouse on desktop */
#if PLATFORM_DESKTOP
	bShowMouseCursor =  true;
#endif
}

void ASpectatorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalController())
	{
		SetInputMode(FInputModeGameOnly());
	}
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

USpectatorCameraComponent* ASpectatorPlayerController::GetSpectatorCameraComponent()
{
	if(!GetPawnOrSpectator())
	{
		UE_LOG(LogPlayerController, Error, TEXT("--Controlled pawn is null"), *GetFullName());
		return nullptr;
	}
	return GetPawnOrSpectator()->FindComponentByClass<USpectatorCameraComponent>();
}

void ASpectatorPlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
	USpectatorCameraComponent* SpectatorCameraComponent = GetSpectatorCameraComponent();
	if(SpectatorCameraComponent)
	{
		SpectatorCameraComponent->UpdateCameraMovement(this);
	}
}


