// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "ActionSpawnPawnSlotBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STRATEGYGAME_API UActionSpawnPawnSlotBase : public UActionBaseSlot
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	bool SpawnPawn();

protected:

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	class ABaseBuildingActor* BuildOwner;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	TAssetSubclassOf<ABaseAIPawn> SpawnPawnClass;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	TAssetPtr<UTexture2D> SlotIcon;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
    TArray<FResourcesData> ResourcesNeedToBuy;
};
