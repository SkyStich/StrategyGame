// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/StaticMeshActors/PreBuildingActor.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

APreBuildingActor::APreBuildingActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	RootComponent = BoxCollision;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, 515.f));
	MeshComponent->SetCollisionProfileName("NoCollision");
}

void APreBuildingActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(OwnerPlayerController)
	{
		SetActorLocation(OwnerPlayerController->GetMousePositionResult().ImpactPoint);
	}
}
