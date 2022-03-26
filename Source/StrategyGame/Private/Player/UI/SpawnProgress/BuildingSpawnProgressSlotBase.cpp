// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/SpawnProgress/BuildingSpawnProgressSlotBase.h"


void UBuildingSpawnProgressSlotBase::RemoveSlotFromQueue()
{
	BuildOwner->RemoveSlotFromQueue(this);
	Super::RemoveSlotFromQueue();
}

