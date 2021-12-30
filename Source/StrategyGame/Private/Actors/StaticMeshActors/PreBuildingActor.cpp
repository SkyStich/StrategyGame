// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/StaticMeshActors/PreBuildingActor.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

APreBuildingActor::APreBuildingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxCollision->SetHiddenInGame(false);
	BoxCollision->SetLinearDamping(30.f);
	BoxCollision->SetCollisionProfileName("Custom...");
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollision->SetRelativeLocation(FVector(0.f, 0.f, 515.f));
	BoxCollision->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMesh"));
	MeshComponent->SetupAttachment(BoxCollision);
	MeshComponent->SetCollisionProfileName("NoCollision");
}

void APreBuildingActor::BeginPlay()
{
	Super::BeginPlay();
}

void APreBuildingActor::SetOwnerController(ASpectatorPlayerController* Controller)
{
	OwnerPlayerController = Controller;
	OwnerPlayerController->OnActionWithObjectPressedEvent.AddDynamic(this, &APreBuildingActor::OnSpawnBuilding);
}

void APreBuildingActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(OwnerPlayerController)
	{
		SetActorLocation(OwnerPlayerController->GetMousePositionResult().ImpactPoint);
	}
}

void APreBuildingActor::OnSpawnBuilding()
{
	TArray<AActor*> OverlappedActors;
	BoxCollision->GetOverlappingActors(OverlappedActors);
	if(OverlappedActors.Num() <= 0)
	{
		OwnerPlayerController->SpawnBuilding(BuildingActorClass);
	}
	Destroy();
}

