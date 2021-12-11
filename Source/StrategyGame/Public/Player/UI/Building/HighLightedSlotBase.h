// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HighLightedSlotBase.generated.h"

class UHighlightedToolTipBase;

UCLASS(Abstract, BlueprintType)
class STRATEGYGAME_API UHighLightedSlotBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UHighLightedSlotBase(const FObjectInitializer& ObjectInitializer);

protected:

	UFUNCTION(BlueprintPure)
	UUserWidget* OnToolTipWidgetEvent();

	UFUNCTION(BlueprintPure, Category = "UI|HighlightedSlot")
	virtual FText GetObjectName() const { return FText(); }

	UFUNCTION(BlueprintPure, Category = "UI|HighlightedSlot")
	virtual FText GetObjectDescription() const { return FText(); }

	/** Sync load icon in memory */
	UFUNCTION(BlueprintPure, Category = "UI|HighlightedSlot")
	UTexture2D* GetIcon();

	virtual TAssetPtr<UTexture2D> GetAssetIconPtr() const { return nullptr; }

private:

	TAssetSubclassOf<UHighlightedToolTipBase> HighlightedToolTip;
};
