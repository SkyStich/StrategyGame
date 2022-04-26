// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MatchGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API AMatchGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AMatchGameModeBase();

protected:

	/** Test logic */
	void InitPlayerBuilding();

	virtual void BeginPlay() override;
};
