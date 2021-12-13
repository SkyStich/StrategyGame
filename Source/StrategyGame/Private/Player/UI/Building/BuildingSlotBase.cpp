// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/Building/BuildingSlotBase.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

FReply UBuildingSlotBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnBuildingSlotClicked.Execute(*SlotInfo);
	return FReply::Handled();
}
