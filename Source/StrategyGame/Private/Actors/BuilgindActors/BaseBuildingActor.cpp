// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Singleton/SingletonClass.h"

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
}

// Called when the game starts or when spawned
void ABaseBuildingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

UTexture2D* ABaseBuildingActor::GetIcon() const
{
	FStreamableManager& AssetLoader = USingletonClass::Get().AssetLoader;
	if(Icon.IsValid())
	{
		FSoftObjectPath const ObjectPath = Icon.ToSoftObjectPath();
		return Cast<UTexture2D>(AssetLoader.LoadSynchronous(ObjectPath));
	}
	return nullptr;
}

