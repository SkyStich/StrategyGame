// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMatchWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API UBaseMatchWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void AttachWidgetToLeftBorder(UUserWidget* Widget);
};
