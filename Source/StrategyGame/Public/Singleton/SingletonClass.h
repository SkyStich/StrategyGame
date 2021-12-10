// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/StreamableManager.h"
#include "UObject/NoExportTypes.h"
#include "SingletonClass.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API USingletonClass : public UObject
{
	GENERATED_BODY()

public:

	static USingletonClass& Get();

	FStreamableManager AssetLoader;
};
