// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/Building/BuildingGridBase.h"
#include "Player/UI/Building/BuildingSlotBase.h"
#include "Structs/SelectedObjectInfoBase.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBuildingGridBase::CreateBuildingSlots(UUniformGridPanel* UniformGridPanel)
{
	static const FString ContextString("DT_BuildingData");
	TArray<FBuildingObjectInfo*> DataStructs;
	BuildingDataTable->GetAllRows<FBuildingObjectInfo>(ContextString, DataStructs);
	
	for(int i = 0; i < DataStructs.Num(); i++)
	{
		if(DataStructs[i])
		{
			UBuildingSlotBase* BuildingSlot = USyncLoadLibrary::SyncLoadWidget<UBuildingSlotBase>(this, BuildingSlotBase, GetOwningPlayer());
			if(BuildingSlot)
			{
				BuildingSlot->SetSlotInfo(DataStructs[i]);
				BuildingSlot->OnBuildingSlotClicked.BindUFunction(this, "OnBuildingSlotClicked");
				BuildingSlot->AddToViewport();
				UniformGridPanel->AddChildToUniformGrid(BuildingSlot, i / 4, i % 4);
			}
		}
	} 
}

void UBuildingGridBase::OnBuildingSlotClickedEvent(const FBuildingObjectInfo& SlotClicked)
{
	
}

