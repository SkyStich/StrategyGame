// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/UI/SpawnProgress/SpawnProgressSlotBase.h"
#include "BuildingSpawnProgressSlotBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STRATEGYGAME_API UBuildingSpawnProgressSlotBase : public USpawnProgressSlotBase
{
	GENERATED_BODY()

public:

	void SetBuildOwner(ABaseBuildingActor* NewOwner) { BuildOwner = NewOwner; }
	virtual void RemoveSlotFromQueue() override;
	virtual void GetProgressTimer_Implementation(FTimerHandle& Timer) override { Timer = BuildOwner->GetProgressTimerHandle(); }
protected:

	UPROPERTY(BlueprintReadOnly)
	ABaseBuildingActor* BuildOwner;
};
