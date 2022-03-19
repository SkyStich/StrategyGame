// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "Player/Objects/PlayerImprovementCharacteristics.h"
#include "ImprovementData.generated.h"

USTRUCT(BlueprintType)
struct FImprovementLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RowName;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLevel;
};

USTRUCT(BlueprintType)
struct FImprovementDataByLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ImprovementTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FResourcesData> ResourcesNeedToBuy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FImprovementData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxLevelImprovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FImprovementDataByLevel> ImprovementDataByLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetSubclassOf<UPlayerImprovementCharacteristics> ImprovementClass;
};

USTRUCT(BlueprintType)
struct FImprovementQueue
{
	GENERATED_BODY()
	
	FImprovementQueue() : Key(""), Value() {}
	FImprovementQueue(const FName& NewKey, const FImprovementData& Data) : Key(NewKey), Value(Data) {} 

	UPROPERTY(BlueprintReadOnly)
	FName Key;

	FImprovementData Value;
};