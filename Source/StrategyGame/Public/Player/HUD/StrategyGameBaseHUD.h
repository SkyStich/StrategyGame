// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interfaces/MatchPlayerHUDInterface.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"
#include "Player/UI/HealthStatistics/HealthStatisticsBase.h"


#include "StrategyGameBaseHUD.generated.h"

class UBaseMatchWidget;

UCLASS(BlueprintType)
class STRATEGYGAME_API AStrategyGameBaseHUD : public AHUD, public IMatchPlayerHUDInterface
{
	GENERATED_BODY()

	UFUNCTION()
	void OnActionWithObjectReleased();

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

	/** Set visibility to ESlateVisible::Visibility */
	virtual void ShowHealthStatistics(AActor* Target);

	/** Set visibility to ESlateVisible::collapsed */
	virtual void HiddenHealthStatistics();
	
protected:

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	virtual void CreateMainWidget();
	virtual void RemoveMainWidget();

	virtual void CreateHealthStatisticsWidget();
	virtual void RemoveHealthStatisticsWidget();

	/** get spectator controller helper */
	ASpectatorPlayerController* GetSpectatorPlayerController();

	UFUNCTION()
	virtual void OnNewControlledPawn(APawn* NewPawn);

protected:

	UPROPERTY()
	TAssetSubclassOf<UBaseMatchWidget> MainWidgetClass;

	UPROPERTY()
	TAssetSubclassOf<UHealthStatisticsBase> HealthStatisticsWidgetClass;

	UPROPERTY()
	UHealthStatisticsBase* HealthStatisticsWidget;

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
	bool bGroupSelection;

public:
	
	FVector2D StartGroupSelectionPosition;
};
