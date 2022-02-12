// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/Base/BaseAIController.h"
#include "BaseAIAggressiveController.generated.h"

class ABaseAggressivePawn;

UCLASS()
class STRATEGYGAME_API ABaseAIAggressiveController : public ABaseAIController
{
	GENERATED_BODY()

private:

	/** get current aggressive controlled pawn helper */
	ABaseAggressivePawn* GetAggressivePawn() const;

protected:

	virtual void BeginPlay() override;
	virtual void StartLoseTimer();

	UFUNCTION()
	virtual void OnSinglePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

public:

	ABaseAIAggressiveController(const FObjectInitializer& ObjectInitializer);

protected:

	FTimerHandle LoseTargetHandle;

private:

	float LoseTargetTime;
};
