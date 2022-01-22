// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

bool UActionSpawnPawnSlotBase::SpawnPawn()
{
	if(BuildOwner)
	{
		BuildOwner->SpawnPawn(USyncLoadLibrary::SyncLoadClass(this, SpawnPawnClass), ResourcesNeedToBuy);
		return true;
	}
	return false;
}
