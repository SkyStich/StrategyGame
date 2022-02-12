// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseAIPawn.h"
#include "AI/Controllers/Base/BaseAIController.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABaseAIPawn::ABaseAIPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;
	NetUpdateFrequency = 15.f;
	AIControllerClass = ABaseAIController::StaticClass();
	
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->MovementState.bCanJump = false;
	GetCharacterMovement()->MovementState.bCanCrouch = false;

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ABaseAIPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseAIPawn, OwnerTeam);
}

void ABaseAIPawn::GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		auto const AIController = GetController<ABaseAIController>();
		if(AIController)
		{
			AIController->MoveToGiveOrder(LocationToMove, ActorToMove);
		}
	}
}
