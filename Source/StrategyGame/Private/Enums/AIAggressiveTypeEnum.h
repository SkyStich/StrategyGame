// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAggressiveTypeEnum.generated.h"

UENUM(BlueprintType)
enum class EAIAggressiveType :  uint8
{
	Holding = 0, //follow a target for a very short distance возвращается на исходную точку
	Pursuit = 1, //chasing a target a short period of time не возвращается на исходную точку (остается на месте)
	BuildDestroyed = 2, //Priority to destroy infrastructure
	WaitCommand = 3, //do nothing without command
	Attack = 4 // Attack and chase the target without limit
};