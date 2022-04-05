// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/ResourceProducingBuildingsBase.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

AResourceProducingBuildingsBase::AResourceProducingBuildingsBase()
{
	PrimaryActorTick.bCanEverTick = false;

	UpdateResourcesTime = 15.f;
}

void AResourceProducingBuildingsBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AResourceProducingBuildingsBase, UpdateResourcesTime, COND_OwnerOnly);
}

void AResourceProducingBuildingsBase::BeginPlay()
{
	Super::BeginPlay();
}

void AResourceProducingBuildingsBase::UpdateResources()
{
	if(GetLocalRole() != ROLE_Authority) return;
	
	if(GetOwnerController())
	{
		for(auto& ByArray : IncreaseResourcesData)
		{
			GetOwnerController()->AddResourcesByType(ByArray.Key, ByArray.Value);
		}
	}
}

void AResourceProducingBuildingsBase::ConstructionSucceeded()
{
	SetResourcesTimer();
}

void AResourceProducingBuildingsBase::IncreaseResourcesTime(const float Value)
{
	if(GetLocalRole() != ROLE_Authority) return;
	
	UpdateResourcesTime += Value;
	ClearResourcesTimer();
	SetResourcesTimer();
}

void AResourceProducingBuildingsBase::DecreaseResourcesTime(const float Value)
{
	if(GetLocalRole() != ROLE_Authority) return;
	
	UpdateResourcesTime -= Value;
	ClearResourcesTimer();
	SetResourcesTimer();
}

void AResourceProducingBuildingsBase::SetResourcesTimer()
{
	if(GetLocalRole() != ROLE_Authority) return;

	GetWorld()->GetTimerManager().SetTimer(UpdateResourceHandle, this, &AResourceProducingBuildingsBase::UpdateResources, UpdateResourcesTime, true);
	
}

void AResourceProducingBuildingsBase::ClearResourcesTimer()
{
	if(GetLocalRole() != ROLE_Authority) return;
	GetWorld()->GetTimerManager().ClearTimer(UpdateResourceHandle);
}



