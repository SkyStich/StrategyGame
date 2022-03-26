// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/SpawnProgress/SpawnProgressSlotBase.h"

void USpawnProgressSlotBase::RemoveFromParent()
{
	Super::RemoveFromParent();
}

void USpawnProgressSlotBase::RemoveSlotFromQueue()
{
	RemoveFromParent();
}
