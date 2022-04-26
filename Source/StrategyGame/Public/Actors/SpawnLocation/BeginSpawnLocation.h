// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "BeginSpawnLocation.generated.h"

UCLASS()
class STRATEGYGAME_API ABeginSpawnLocation : public ATargetPoint
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABeginSpawnLocation();
	
	int32 GetSpawnIndex() const { return SpawnIndex; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "SpawnIndex")
	int32 SpawnIndex;
};
