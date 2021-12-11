// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Engine/DataTable.h"
#include "BuildingGridBase.generated.h"

class UBuildingSlotBase;
struct FBuildingObjectInfo;

UCLASS(Blueprintable, Abstract)
class STRATEGYGAME_API UBuildingGridBase : public UUserWidget
{
	GENERATED_BODY()

	/** Call when player clicked on building slot */
	void OnBuildingSlotClickedEvent(const FBuildingObjectInfo& SlotClicked);
	
protected:

	/** Creating building slot */
	UFUNCTION(BlueprintCallable, Category = "UI|Building")
	void CreateBuildingSlots(UUniformGridPanel* UniformGridPanel);

private:

	/** Data table with building structs */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|BuildingSlot", meta=(AllowPrivateAccess="true"))
	UDataTable* BuildingDataTable;

	UPROPERTY(EditDefaultsOnly)
	TAssetSubclassOf<UBuildingSlotBase> BuildingSlotBase;
};
