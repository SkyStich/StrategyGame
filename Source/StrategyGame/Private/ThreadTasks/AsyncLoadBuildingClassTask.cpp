// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadTasks/AsyncLoadBuildingClassTask.h"


#include "BlueprintFunctionLibraries/AsyncLoadLibrary.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Structs/SelectedObjectInfoBase.h"

AsyncLoadBuildingClassTask::AsyncLoadBuildingClassTask()
	:	WorldContext(nullptr), BuildingDataTable(nullptr)
{
}

AsyncLoadBuildingClassTask::AsyncLoadBuildingClassTask(UObject* Context, UDataTable* DataTable)
	:	WorldContext(Context), BuildingDataTable(DataTable)
{
	
}

TStatId AsyncLoadBuildingClassTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(PrimeCalculationAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
}

void AsyncLoadBuildingClassTask::DoWork()
{
	TArray<FBuildingObjectInfo*> OutRows; 
	BuildingDataTable->GetAllRows<FBuildingObjectInfo>("DT_BuildingData", OutRows);

	for(auto ByArray : OutRows)
	{
		if(ByArray) USyncLoadLibrary::SyncLoadClass<ABaseBuildingActor>(WorldContext, ByArray->BuildingClass);
	}
}