// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AI/Controllers/Base/BaseAIController.h"
#include "Player/UI/ActionSlots/Base/ActionBaseSlot.h"
#include "ToggleTypeOfBehavior.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STRATEGYGAME_API UToggleTypeOfBehavior : public UActionBaseSlot
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void ToggleBehaviorType(EAIAggressiveType Type);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateIcon();
	
	void SetOwnerAIController(ABaseAIController* AIController);
	
protected:

	UPROPERTY(BlueprintReadOnly)
	ABaseAIController* OwnerAIController;

	UPROPERTY(BlueprintReadOnly)
	EAIAggressiveType CurrentType;

	FTimerHandle ChangeOrderDelayHandle;
};
