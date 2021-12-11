// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HighLightedSlotBase.h"
#include "Structs/SelectedObjectInfoBase.h"
#include "BuildingSlotBase.generated.h"

class UDataTable;

DECLARE_DYNAMIC_DELEGATE_OneParam(FBuildingSlotClicked, const FBuildingObjectInfo&, Slot);

UCLASS(BlueprintType, Abstract)
class STRATEGYGAME_API UBuildingSlotBase : public UHighLightedSlotBase
{
	GENERATED_BODY()

public:

	/** Write slot info */
	void SetSlotInfo(FBuildingObjectInfo* Info) { SlotInfo = Info; }

protected:

	virtual FText GetObjectName() const override final { return SlotInfo->DisplayName; }
	virtual FText GetObjectDescription() const override final { return SlotInfo->Description; }
	virtual TAssetPtr<UTexture2D> GetAssetIconPtr() const override final { return SlotInfo->Icon; } 
	
private:

	/** Slot info from building data table */
	FBuildingObjectInfo* SlotInfo;

public:

	/** execute when player click on this slot */
	FBuildingSlotClicked OnBuildingSlotClicked;
};
