// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GiveOrderToTargetPawns.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGiveOrderToTargetPawns : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STRATEGYGAME_API IGiveOrderToTargetPawns
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Interface on give order to target pawn
	 *
	 *@param Controller	Controller who give order
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void GiveOrderToTargetPawn();
};
