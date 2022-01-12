// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourcesData.generated.h"

UENUM(BlueprintType)
enum class EResourcesType : uint8
{
	Food,
    Metal,
    Fuel
};