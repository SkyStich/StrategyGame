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

protected:

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	TAssetSubclassOf<class ABaseBuildingActor> SpawnBuildClass;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	TAssetPtr<UTexture2D> SlotIcon;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	TArray<FResourcesData> ResourcesNeedToBuy;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	FName RowName;
};
