// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ActorComponents/Actors/ObjectHealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "HealthStatisticsBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STRATEGYGAME_API UHealthStatisticsBase : public UUserWidget
{
	GENERATED_BODY()

public:

	/**
	 * Set new target for display
	 *
	 * @param Target New target actor
	 */
	void SetTarget(AActor* Target);

	/** call before destroy */
	void ClearTarget();

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void Init();

	UFUNCTION(BlueprintImplementableEvent)
	void ClearTargetActor();

protected:

	UPROPERTY(BlueprintReadOnly)
	UObjectHealthComponent* HealthComponent;
};
