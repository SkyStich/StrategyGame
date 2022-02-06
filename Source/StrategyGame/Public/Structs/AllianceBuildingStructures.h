// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Engine/DataTable.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "AllianceBuildingStructures.generated.h"

USTRUCT(BlueprintType)
struct FAllianceBaseData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcesData> ResourcesNeedToBuy;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeBeforeSpawn;
};

USTRUCT(BlueprintType)
struct FBaseSpawnPawnData : public FAllianceBaseData
{
	GENERATED_BODY()
	
	FBaseSpawnPawnData() : BuilderClass(ABaseAIPawn::StaticClass()), Icon(nullptr) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetSubclassOf<ABaseAIPawn> BuilderClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FAllianceBuildersData : public FAllianceBaseData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetSubclassOf<class ABaseBuildingActor> BuildClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetPtr<UTexture2D> Icon;
};