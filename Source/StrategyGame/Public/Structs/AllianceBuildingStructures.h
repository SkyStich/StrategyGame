// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Engine/DataTable.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "AllianceBuildingStructures.generated.h"

USTRUCT(BlueprintType)
struct FBaseBuilderData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcesData> ResourcesNeedToBuy;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;
};

USTRUCT(BlueprintType)
struct FAllianceBuildersData : public FBaseBuilderData
{	
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetSubclassOf<class ABaseBuildingActor> BuildClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetPtr<UTexture2D> Icon;
};
