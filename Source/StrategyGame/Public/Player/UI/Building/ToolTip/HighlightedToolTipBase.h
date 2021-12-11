// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HighlightedToolTipBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Abstract)
class STRATEGYGAME_API UHighlightedToolTipBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FText BuildingName;

	UPROPERTY(BlueprintReadOnly)
	FText Description;
};
