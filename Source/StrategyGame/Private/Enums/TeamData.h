// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamData.generated.h"

UENUM(BlueprintType)
enum class EObjectTeam : uint8
{
	None = 5, //reserve id
	TeamA = 0,
	TeamB = 1
};