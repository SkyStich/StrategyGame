// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "ResourceProducingBuildingsBase.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API AResourceProducingBuildingsBase : public ABaseBuildingActor
{
	GENERATED_BODY()

public:

	virtual void HighlightedActor_Implementation(APlayerController* PlayerController) override;
};
