// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Runtime/Core/Public/Async/AsyncWork.h>
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Engine/DataTable.h"

/**
 * 
 */
class STRATEGYGAME_API AsyncLoadBuildingClassTask : public FNonAbandonableTask
{
public:
	AsyncLoadBuildingClassTask();
	AsyncLoadBuildingClassTask(UObject* Context, UDataTable* DataTable);
	
	FORCEINLINE TStatId GetStatId() const;

	void DoWork();

private:

	UObject* WorldContext;
	UDataTable* BuildingDataTable;
};
