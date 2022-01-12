// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/BaseBuildingActor.h"

#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
// Sets default values
ABaseBuildingActor::ABaseBuildingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BoxCollision->SetRelativeLocation(FVector(0.f, 0.f, 515.f));
	RootComponent = BoxCollision;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseBuildingActor::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseBuildingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuildingActor::SetOwnerController(ASpectatorPlayerController* Controller)
{
	OwnerPlayerController = Controller;	
}

void ABaseBuildingActor::HighlightedActor_Implementation(APlayerController* PlayerController)
{
	PlayerController->GetHUD<AStrategyGameBaseHUD>()->CreateActionObjectGrid(ActionObjectGrid);
}
