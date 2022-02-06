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
};
