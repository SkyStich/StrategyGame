// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

#include "StrategyGameBaseHUD.generated.h"

class UBaseMatchWidget;

UCLASS(BlueprintType)
class STRATEGYGAME_API AStrategyGameBaseHUD : public AHUD
{
	GENERATED_BODY()

public:

	AStrategyGameBaseHUD();
	
	void SetGroupSelectionActive(bool const IsActive) { bGroupSelectionActive = IsActive; }
	void CreateActionObjectGrid(TAssetSubclassOf<UUserWidget> Grid);
	void RemoveActionObjectGrid();

	
protected:

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	virtual void CreateMainWidget();
	virtual void RemoveMainWidget();

	/** get spectator controller helper */
	ASpectatorPlayerController* GetSpectatorPlayerController();

	UFUNCTION()
	virtual void OnNewControlledPawn(APawn* NewPawn);

protected:

	UPROPERTY()
	TAssetSubclassOf<UBaseMatchWidget> MainWidgetClass;

	UPROPERTY()
	UBaseMatchWidget* MainWidgetWidget;

	UPROPERTY()
	UUserWidget* ActiveActionGrid;

private:

	bool bGroupSelectionActive;

public:
	
	FVector2D StartGroupSelectionPosition;
};
