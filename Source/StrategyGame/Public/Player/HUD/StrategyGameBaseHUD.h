// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interfaces/MatchPlayerHUDInterface.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"

#include "StrategyGameBaseHUD.generated.h"

class UBaseMatchWidget;

UCLASS(BlueprintType)
class STRATEGYGAME_API AStrategyGameBaseHUD : public AHUD, public IMatchPlayerHUDInterface
{
	GENERATED_BODY() 

public:

	AStrategyGameBaseHUD();
	
	void SetGroupSelectionActive(bool const IsActive) { bGroupSelectionActive = IsActive; }
	virtual UBaseMatchWidget* GetMainMatchWidget_Implementation() override { return MainWidget; }

	UFUNCTION(BlueprintCallable)
	void CreateActionGrid(const TArray<class UActionBaseSlot*>& Slots);
	void ClearActionGrid();
	void RemoveActionGrid();
	void GroupSelectingReleased();
	bool GetGroupSelectionActive() const { return bGroupSelectionActive; }

	UBaseMatchWidget* GetMainWidget() const { return MainWidget; }
	TAssetSubclassOf<UActionSpawnPawnSlotBase> GetActionSpawnPawnSlotClass() const { return ActionSpawnPawnSlotClass; }	
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
	UBaseMatchWidget* MainWidget;

	UPROPERTY()
	TAssetSubclassOf<UActionSpawnPawnSlotBase> ActionSpawnPawnSlotClass;

	UPROPERTY()
	class UActionGridBase* ActionGrid;

	UPROPERTY()
	TAssetSubclassOf<class UActionGridBase> ActionGridClass;

private:

	bool bGroupSelectionActive;

public:
	
	FVector2D StartGroupSelectionPosition;
};
