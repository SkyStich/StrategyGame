// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Pawn/SpectatorPlayerPawn.h"
#include "Player/Components/SpectatorCameraComponent.h"

ASpectatorPlayerPawn::ASpectatorPlayerPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpectatorCameraComponent = CreateDefaultSubobject<USpectatorCameraComponent>(TEXT("SpectatorCameraComponent"));
}
