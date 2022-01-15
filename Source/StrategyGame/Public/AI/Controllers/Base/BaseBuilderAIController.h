// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/Base/BaseAIController.h"
#include "BaseBuilderAIController.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API ABaseBuilderAIController : public ABaseAIController
{
	GENERATED_BODY()

protected:

	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
