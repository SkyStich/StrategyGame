// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "SelectedObjectInfoBase.generated.h"

USTRUCT(BlueprintType)
struct FSelectedObjectInfo : public FTableRowBase
{
	GENERATED_BODY()

	FSelectedObjectInfo() : DisplayName(FText()), Description(FText()), Icon(nullptr) {}
	FSelectedObjectInfo(const FText& Name, const FText& Desc, TAssetPtr<UTexture2D> Texture) : DisplayName(Name), Description(Desc), Icon(Texture) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedInfo")
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedInfo")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedInfo")
	TAssetPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FBuildingObjectInfo : public FSelectedObjectInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedInfo")
	TAssetSubclassOf<ABaseBuildingActor> BuildingClass; 
};

