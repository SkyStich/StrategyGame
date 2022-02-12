// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

bool UActionSpawnPawnSlotBase::SpawnPawn()
{
	if(!BuildOwner) return false;
	
	BuildOwner->SpawnPawn(RowId);
	return true;
}

void UActionSpawnPawnSlotBase::Init_Implementation(ABaseBuildingActor* OwnerBuild, const FName& RowName, const TAssetPtr<UTexture2D>& Icon)
{
	RowId = RowName;
	BuildOwner = OwnerBuild;
}
