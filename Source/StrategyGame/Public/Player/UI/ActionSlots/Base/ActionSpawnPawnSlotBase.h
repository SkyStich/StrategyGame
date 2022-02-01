// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Engine/DataTable.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "Structs/AllianceBuildingStructures.h"
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

	UFUNCTION(BlueprintPure)
	FBaseSpawnPawnData GetPawnData() const;

protected:

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	class ABaseBuildingActor* BuildOwner;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	FName RowId;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	TAssetPtr<UTexture2D> SlotIcon;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
    UDataTable* PawnData;
};
