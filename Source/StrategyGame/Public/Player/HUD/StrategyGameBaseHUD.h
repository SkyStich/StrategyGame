// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interfaces/MatchPlayerHUDInterface.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/UI/ActionSlots/Base/ActionSpawnBuildingSlot.h"
#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"
#include "Player/UI/ActionSlots/Base/DestroyHighlightedObjectSlot.h"
#include "Player/UI/ActionSlots/Base/ToggleTypeOfBehavior.h"
#include "Player/UI/HealthStatistics/HealthStatisticsBase.h"
#include "StrategyGameBaseHUD.generated.h"

class UImprovementSlotBase;
class UBaseMatchWidget;

UCLASS(BlueprintType)
class STRATEGYGAME_API AStrategyGameBaseHUD : public AHUD, public IMatchPlayerHUDInterface
{
	GENERATED_BODY()

	UFUNCTION()
	void OnActionWithObjectReleased();

public:

	AStrategyGameBaseHUD();

	void AddActionSlot(UActionBaseSlot* Slot);
	void AddActionSlots(const TArray<class UActionBaseSlot*>& Slots);
	void AddActionSlotByIndex(UActionBaseSlot* Slot, const int32 Row, const int32 Column);
	void SetGroupSelectionActive(bool const IsActive) { bGroupSelectionActive = IsActive; }
	virtual UBaseMatchWidget* GetMainMatchWidget_Implementation() override { return MainWidget; }

	UFUNCTION(BlueprintCallable)
	void CreateActionGrid();

	void AddImprovementWidgetToGrid(const TArray<class UActionBaseSlot*>& Slots);
	void ClearActionGrid();
	void RemoveActionGrid();
	void GroupSelectingReleased();
	bool GetGroupSelectionActive() const { return bGroupSelectionActive; }

	UBaseMatchWidget* GetMainWidget() const { return MainWidget; }
	TAssetSubclassOf<UActionSpawnPawnSlotBase> GetActionSpawnPawnSlotClass() const { return ActionSpawnPawnSlotClass; }
	TAssetSubclassOf<UImprovementSlotBase> GetImprovementSlotClass() const { return ImprovementSlotClass; }
	TSubclassOf<class UBuildingSpawnProgressSlotBase> GetBuildingProgressSlotClass() const { return BuildingProgressSlotClass; }
	TSubclassOf<class UBuildingSpawnProgressSlotBase> GetBuildingImprovementProgressSlotClass() const { return BuildingImprovementProgressSlotClass; }
	TSubclassOf<UActionSpawnBuildingSlot> GetSpawnBuildingSlotClass() const { return SpawnBuildingSlotClass; }
	TSubclassOf<UDestroyHighlightedObjectSlot> GetDestroyObjectSlot() const { return DestroyHighlightedObjectSlotClass; }
	TSubclassOf<UToggleTypeOfBehavior> GetToggleBehaviorType() const { return ToggleTypeBehaviorSlotClass; }
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

	/** Improvement */
	UPROPERTY()
	TAssetSubclassOf<UImprovementSlotBase> ImprovementSlotClass;
	
	UPROPERTY()
	TSubclassOf<class UBuildingSpawnProgressSlotBase> BuildingImprovementProgressSlotClass;
	
	/** action slots */
	UPROPERTY()
	TSubclassOf<UActionSpawnBuildingSlot> SpawnBuildingSlotClass;

	UPROPERTY()
	TAssetSubclassOf<UActionSpawnPawnSlotBase> ActionSpawnPawnSlotClass;

	UPROPERTY()
	TAssetSubclassOf<class UActionGridBase> ActionGridClass;

	UPROPERTY()
	TSubclassOf<UDestroyHighlightedObjectSlot> DestroyHighlightedObjectSlotClass;

	UPROPERTY()
	TSubclassOf<UToggleTypeOfBehavior> ToggleTypeBehaviorSlotClass;

	/** progress slots */
	UPROPERTY()
	TSubclassOf<class UBuildingSpawnProgressSlotBase> BuildingProgressSlotClass;

	UPROPERTY()
	UHealthStatisticsBase* HealthStatisticsWidget;

	UPROPERTY()
	UBaseMatchWidget* MainWidget;

	UPROPERTY()
	class UActionGridBase* ActionGrid;

private:

	bool bGroupSelectionActive;
	bool bGroupSelection;

public:
	
	FVector2D StartGroupSelectionPosition;
};
