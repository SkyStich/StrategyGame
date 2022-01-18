// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Pawn/SpectatorPlayerPawn.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/PlayerSpectatorPawnMovement.h"
#include "Player/Components/SpectatorCameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ThreadTasks/AsyncLoadBuildingClassTask.h"

ASpectatorPlayerPawn::ASpectatorPlayerPawn(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerSpectatorPawnMovement>(Super::MovementComponentName))
{
	SpectatorCameraComponent = CreateDefaultSubobject<USpectatorCameraComponent>(TEXT("SpectatorCameraComponent"));

	GetCollisionComponent()->SetCollisionProfileName("NoCollision");
	bAddDefaultMovementBindings = false;
	bReplicates = true;
	NetUpdateFrequency = 10.f;
}

void ASpectatorPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	/** Zoom input binding */
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, SpectatorCameraComponent, &USpectatorCameraComponent::DecreaseZoomLevel);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, SpectatorCameraComponent, &USpectatorCameraComponent::IncreaseZoomLevel);

	/** Move Input binding */
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpectatorPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpectatorPlayerPawn::MoveRight);
}

void ASpectatorPlayerPawn::MoveForward(float Val)
{
	SpectatorCameraComponent->MoveForward(Val);
}

void ASpectatorPlayerPawn::MoveRight(float Val)
{
	SpectatorCameraComponent->MoveRight(Val);
}

void ASpectatorPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ASpectatorPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}


