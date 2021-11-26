// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Pawn/SpectatorPlayerPawn.h"
#include "Components/SphereComponent.h"
#include "Player/Components/SpectatorCameraComponent.h"

ASpectatorPlayerPawn::ASpectatorPlayerPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpectatorCameraComponent = CreateDefaultSubobject<USpectatorCameraComponent>(TEXT("SpectatorCameraComponent"));

	GetCollisionComponent()->SetCollisionProfileName("NoCollision");
	bAddDefaultMovementBindings = false;
}

void ASpectatorPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, SpectatorCameraComponent, &USpectatorCameraComponent::DecreaseZoomLevel);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, SpectatorCameraComponent, &USpectatorCameraComponent::IncreaseZoomLevel);
}
