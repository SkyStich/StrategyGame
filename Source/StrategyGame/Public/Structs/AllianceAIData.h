// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "AllianceAIData.generated.h"

USTRUCT(BlueprintType)
struct FAIBasePawnData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseRangeAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDelayBeforeAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseArmor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseSpeed;
};

USTRUCT(BlueprintType)
struct FAIPawnDescriptionData : public FAIBasePawnData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ObjectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcesData> ResourcesNeedToBuy;
};

USTRUCT(BlueprintType)
struct FAIPawnData : public FAIPawnDescriptionData
{
	GENERATED_BODY()
	
	FAIPawnData() : Id(""), PawnClass(nullptr), TimeBeforeSpawn(1.f), Icon(nullptr) {}
	FAIPawnData(const FName& Name, TAssetSubclassOf<class ABaseAIPawn> Class, float Time, TAssetPtr<UTexture2D> Texture, const TArray<FResourcesData>& Resources)
    : Id(Name), PawnClass(Class), TimeBeforeSpawn(Time), Icon(Texture) {}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Id;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetSubclassOf<class ABaseAIPawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeBeforeSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetPtr<UTexture2D> Icon;
};