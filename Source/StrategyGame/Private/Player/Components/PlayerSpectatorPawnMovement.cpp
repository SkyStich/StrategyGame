// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/PlayerSpectatorPawnMovement.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/Pawn/SpectatorPlayerPawn.h"
#include "Player/Components/SpectatorCameraComponent.h"

UPlayerSpectatorPawnMovement::UPlayerSpectatorPawnMovement()
{
	MaxSpeed = 5000.f;
	Acceleration = 5000.f;
	Deceleration = 4000.f;

//	SetTickableWhenPaused(false);
}

void UPlayerSpectatorPawnMovement::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerSpectatorPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PawnOwner || !UpdatedComponent)
	{
		return;
	}
	
	ASpectatorPlayerController* PlayerController = Cast<ASpectatorPlayerController>(PawnOwner->Controller);
	if(PlayerController && PlayerController->IsLocalController())
	{
		if(!bInitialLocationSet)
		{
			PawnOwner->SetActorLocation(PlayerController->GetSpawnLocation());
			PawnOwner->SetActorRotation(PlayerController->GetControlRotation());
			bInitialLocationSet = true;
		}
		/*FVector MyLocation = UpdatedComponent->GetComponentLocation();
		ASpectatorPlayerPawn* PlayerPawn = Cast<ASpectatorPlayerPawn>(PlayerController->GetPawnOrSpectator());
		if(PlayerPawn && PlayerPawn->GetSpectatorCameraComponent())
		{
			PlayerPawn->GetSpectatorCameraComponent()->Clamp
		}*/
	}
}
