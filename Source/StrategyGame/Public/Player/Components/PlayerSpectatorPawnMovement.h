// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "PlayerSpectatorPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API UPlayerSpectatorPawnMovement : public USpectatorPawnMovement
{
	GENERATED_BODY()
	
public:

	UPlayerSpectatorPawnMovement();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;

private:

	bool bInitialLocationSet;
};
