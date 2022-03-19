// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "ImprovementSlotBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STRATEGYGAME_API UImprovementSlotBase : public UActionBaseSlot
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	void OnSlotClicked();

public:

	void SetRowName(const FName& RowName) { ImprovementRowName = RowName; }
	void SetObjectImprovement(AActor* Actor) { ObjectForImprovement = Actor; }

	UFUNCTION(BlueprintImplementableEvent)
	void SetIcon(const TAssetPtr<UTexture2D>& Icon);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:

	UPROPERTY(BlueprintReadOnly)
	FName ImprovementRowName;

	UPROPERTY(BlueprintReadOnly)
	AActor* ObjectForImprovement;
};
