// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/ConstructionBuildingBase.h"
#include "ActorComponents/Actors/ObjectHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AConstructionBuildingBase::AConstructionBuildingBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bBuildingConstruction = false;

	ObjectHealthComponent = CreateDefaultSubobject<UObjectHealthComponent>(TEXT("ObjectHealthComponent"));
}

// Called when the game starts or when spawned
void AConstructionBuildingBase::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority)
	{
		GetObjectHealthComponent()->OnStopRegeneration.AddDynamic(this, &AConstructionBuildingBase::OnStopRegeneration);
	}	
}

void AConstructionBuildingBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConstructionBuildingBase, bBuildingConstruction);
}

void AConstructionBuildingBase::OnStopRegeneration()
{
	if(GetObjectHealthComponent()->GetCurrentObjectHealth() >= GetObjectHealthComponent()->GetMaxObjectHealth())
	{
		bBuildingConstruction = true;
		ConstructionSucceeded();
	}
}

void AConstructionBuildingBase::StartConstruction(float const Value)
{
	ObjectHealthComponent->AddRegenerationValuePerSec(Value);
}

void AConstructionBuildingBase::StartObjectRepair_Implementation(float const Value)
{
	StartConstruction(Value);
}

void AConstructionBuildingBase::StopObjectRepair_Implementation(float const Value)
{
	ObjectHealthComponent->AddRegenerationValuePerSec(Value * -1);
}

