// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "StrategyGameBaseHUD.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STRATEGYGAME_API AStrategyGameBaseHUD : public AHUD
{
	GENERATED_BODY()

public:

	AStrategyGameBaseHUD();
	
protected:

	virtual void BeginPlay() override;

	virtual void CreateBuildingGrid();
	virtual void RemoveBuildingGrid();

	UFUNCTION()
	virtual void OnNewControlledPawn(APawn* NewPawn);

protected:

	UPROPERTY()
	TAssetSubclassOf<UUserWidget> BuildingGridClassPtr;

	UPROPERTY()
	UUserWidget* BuildingGridWidget;
};
