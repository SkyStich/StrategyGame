// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Engine/DataTable.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "Structs/SelectedObjectInfoBase.h"
#include "ActionGridBase.generated.h"

UCLASS(Blueprintable, Abstract)
class STRATEGYGAME_API UActionGridBase : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void AddSlots(const TArray<UActionBaseSlot*>& BuildingSlots);

	UFUNCTION(BlueprintImplementableEvent)
	void AddActionSlot(UActionBaseSlot* ActionSlot);

	UFUNCTION(BlueprintImplementableEvent)
	void AddSlotByIndex(UActionBaseSlot* ActionSlot, const int32 Row, const int32 Column);

	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetUniformGridPanel() const;

	UFUNCTION(BlueprintCallable)
	void AttachImprovementSlots(const TArray<UActionBaseSlot*> Slots);

	UFUNCTION(BlueprintImplementableEvent)
	void ClearGrid();
};
