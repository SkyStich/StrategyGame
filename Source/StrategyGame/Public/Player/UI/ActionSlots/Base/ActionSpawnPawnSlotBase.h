// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "Structs/AllianceBuildingStructures.h"
#include "Structs/AllianceAIData.h"
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

	UFUNCTION(BlueprintNativeEvent)
	void Init(class ABaseBuildingActor* OwnerBuild, const FName& RowName, const TAssetPtr<UTexture2D>& Icon);

protected:

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	class ABaseBuildingActor* BuildOwner;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta = (ExposeOnSpawn = "true"))
	FName RowId;
};
