// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseAIPawn.h"
#include "AI/Controllers/Base/BaseAIController.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseAIPawn::ABaseAIPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	NetUpdateFrequency = 10.f;
	AIControllerClass = ABaseAIController::StaticClass();

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapuseComponent"));
	RootComponent = CapsuleComponent;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMesh->SetupAttachment(RootComponent);
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


void ABaseAIPawn::HighlightedActor_Implementation(APlayerController* PlayerController)
{
	PlayerController->GetHUD<AStrategyGameBaseHUD>()->CreateActionObjectGrid(ActionObjectGrid);
}
