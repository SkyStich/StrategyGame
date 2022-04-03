// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "ActionSpawnBuildingSlot.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STRATEGYGAME_API UActionSpawnBuildingSlot : public UActionBaseSlot
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SpawnBuilding();

	void SetIcon(TAssetPtr<UTexture2D> Icon) { SlotIcon = Icon; }
	void SetClass(TAssetSubclassOf<class ABaseBuildingActor> Class) { SpawnBuildClass = Class; }
	void SetRowName(const FName& Row) { RowName = Row; }
	void SetResourcesNeedToBuy(const TArray<FResourcesData>& Resources) { ResourcesNeedToBuy = Resources; }

	UFUNCTION(BlueprintImplementableEvent)
	void Init();
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TAssetSubclassOf<class ABaseBuildingActor> SpawnBuildClass;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TAssetPtr<UTexture2D> SlotIcon;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<FResourcesData> ResourcesNeedToBuy;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FName RowName;
};
