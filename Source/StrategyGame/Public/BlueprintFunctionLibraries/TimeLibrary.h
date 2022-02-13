// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimeLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API UTimeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "BlieprintFunctionLibrary|TimeLibrary")
	static float GetTimeRate(UObject* WorldContext, const FTimerHandle& Timer);
};

