// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

bool UActionSpawnPawnSlotBase::SpawnPawn()
{
	if(BuildOwner && PawnData)
	{
		const FString ContextString = FString(PawnData->GetName());
		auto const Row = PawnData->FindRow<FBaseSpawnPawnData>(RowId, *ContextString);
		if(Row)
		{
			if(Row->ResourcesNeedToBuy.Num() <= 0) return false;
			BuildOwner->SpawnPawn(PawnData, RowId);
		}
		return true;
	}
	return false;
}

FBaseSpawnPawnData UActionSpawnPawnSlotBase::GetPawnData() const
{
	if(!PawnData) return FBaseSpawnPawnData();
	const FString ContextString = FString(PawnData->GetName());
	auto const Row = PawnData->FindRow<FBaseSpawnPawnData>(RowId, *ContextString);
	return Row ? *Row : FBaseSpawnPawnData();
}
