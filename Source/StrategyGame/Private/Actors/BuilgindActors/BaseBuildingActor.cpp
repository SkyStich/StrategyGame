// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Singleton/SingletonClass.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABaseBuildingActor::ABaseBuildingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxCollision;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UDataTable>BuildingDataTableFinder(TEXT("/Game/Blueprints/DataTables/DT_BuildingObjectInfo"));
	if(BuildingDataTableFinder.Succeeded())
	{
		SelectedObjectInfo = BuildingDataTableFinder.Object->FindRow<FBuildingObjectInfo>(RowName, "DT_BuildingObjectInfo");
	}
}

// Called when the game starts or when spawned
void ABaseBuildingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseBuildingActor::HighlightedActor_Implementation(APlayerController* PlayerController)
{
	
}
